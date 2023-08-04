// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "io/import/cifClasses.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/coreData.h"
#include "classes/empiricalFormula.h"
#include "classes/molecule.h"
#include "classes/species.h"
#include "io/import/cif.h"
#include "neta/neta.h"
#include "procedure/nodes/add.h"
#include "procedure/nodes/box.h"
#include "procedure/nodes/coordinateSets.h"

/*
 * CIF Symmetry-Unique Atom
 */

CIFSymmetryAtom::CIFSymmetryAtom(std::string_view label, Elements::Element Z, Vec3<double> rFrac, double occ)
    : label_{label}, Z_(Z), rFrac_(rFrac), occupancy_(occ){};

// Return label (from _atom_site_label)
std::string_view CIFSymmetryAtom::label() const { return label_; }

// Return element (from  _atom_site_type_symbol)
Elements::Element CIFSymmetryAtom::Z() const { return Z_; }

// Return fractional coordinate of atom (from _atom_site_fract_[xyz])
Vec3<double> CIFSymmetryAtom::rFrac() const { return rFrac_; }

// Return site occupancy (from _atom_site_occupancy)
double CIFSymmetryAtom::occupancy() const { return occupancy_; }

/*
 * CIF Bonding Pair
 */

CIFBondingPair::CIFBondingPair(std::string_view labelI, std::string_view labelJ, double r)
    : labelI_{labelI}, labelJ_{labelJ}, r_(r){};

// Return labels of involved atom i (from _geom_bond_atom_site_label_1)
std::string_view CIFBondingPair::labelI() const { return labelI_; }

// Return labels of involved atom i (from _geom_bond_atom_site_label_2)
std::string_view CIFBondingPair::labelJ() const { return labelJ_; }

// Return distance between bonded pair (from _geom_bond_distance)
double CIFBondingPair::r() const { return r_; }

/*
 * CIF Atom Group
 */

CIFAtomGroup::CIFAtomGroup(std::string_view name) : name_{name} {};

// Group name (from _atom_site_disorder_group)
std::string_view CIFAtomGroup::name() const { return name_; }

// Add atom to the group
void CIFAtomGroup::addAtom(const CIFSymmetryAtom &i) { atoms_.emplace_back(i); }

// Return atoms in the group
const std::vector<CIFSymmetryAtom> &CIFAtomGroup::atoms() const { return atoms_; }

// Set whether the group is active (included in unit cell generation)
void CIFAtomGroup::setActive(bool b) { active_ = b; }

// Return whether the group is active (included in unit cell generation)
bool CIFAtomGroup::active() const { return active_; }

/*
 * CIF Assembly
 */

CIFAssembly::CIFAssembly(std::string_view name) : name_{name} {}

// Return name of the assembly (from _atom_site_disorder_assembly or 'Global')
std::string_view CIFAssembly::name() const { return name_; }

// Return all groups
std::vector<CIFAtomGroup> &CIFAssembly::groups() { return groups_; }
const std::vector<CIFAtomGroup> &CIFAssembly::groups() const { return groups_; }

// Get (add or retrieve) named group
CIFAtomGroup &CIFAssembly::getGroup(std::string_view groupName)
{
    auto it =
        std::find_if(groups_.begin(), groups_.end(), [groupName](const auto &group) { return group.name() == groupName; });
    if (it != groups_.end())
        return *it;

    return groups_.emplace_back(groupName);
}

// Return the number of defined groups
int CIFAssembly::nGroups() const { return groups_.size(); }

CIFSpecies::CIFSpecies(CIFHandler &cifHandler, CoreData &coreData) : cifHandler_(cifHandler), coreData_(coreData)
{
    structuralConfiguration_ = coreData.addConfiguration();
    structuralConfiguration_->setName("Crystal");
    cleanedConfiguration_ = coreData.addConfiguration();
    cleanedConfiguration_->setName("Crystal (cleaned)");
    supercellConfiguration_ = coreData.addConfiguration();
    supercellConfiguration_->setName("Supercell");
}

/*
 * Creation
 */

// Create a structural species
bool CIFSpecies::createStructuralSpecies(double tolerance, bool calculateBonding, bool preventMetallicBonding)
{
    // Create temporary atom types corresponding to the unique atom labels
    for (auto &a : cifHandler_.assemblies())
        for (auto &g : a.groups())
            if (g.active())
                for (auto &i : g.atoms())
                    if (!coreData_.findAtomType(i.label()))
                    {
                        auto at = coreData_.addAtomType(i.Z());
                        at->setName(i.label());
                    }

    // Remove the old Structural species if it exists
    if (structuralSpecies_)
        coreData_.removeSpecies(structuralSpecies_);

    // Generate a single species containing the entire crystal
    structuralSpecies_ = coreData_.addSpecies();
    structuralSpecies_->setName("Crystal");

    // -- Set unit cell
    auto cellLengths = cifHandler_.getCellLengths();
    if (!cellLengths)
        return false;
    auto cellAngles = cifHandler_.getCellAngles();
    if (!cellAngles)
        return false;
    structuralSpecies_->createBox(cellLengths.value(), cellAngles.value());
    auto *box = structuralSpecies_->box();

    // Configuration
    structuralConfiguration_->empty();
    structuralConfiguration_->createBoxAndCells(cellLengths.value(), cellAngles.value(), false, 1.0);

    // -- Generate atoms
    auto symmetryGenerators = SpaceGroups::symmetryOperators(cifHandler_.spaceGroup());
    for (const auto &generator : symmetryGenerators)
        for (auto &a : cifHandler_.assemblies())
            for (auto &g : a.groups())
                if (g.active())
                    for (auto &unique : g.atoms())
                    {
                        // Generate folded atomic position in real space
                        auto r = generator * unique.rFrac();
                        box->toReal(r);
                        r = box->fold(r);

                        // If this atom overlaps with another in the box, don't add it as it's a symmetry-related copy
                        if (std::any_of(structuralSpecies_->atoms().begin(), structuralSpecies_->atoms().end(),
                                        [&r, box, tolerance](const auto &j)
                                        { return box->minimumDistance(r, j.r()) < tolerance; }))
                            continue;

                        // Create the new atom
                        auto i = structuralSpecies_->addAtom(unique.Z(), r);
                        structuralSpecies_->atom(i).setAtomType(coreData_.findAtomType(unique.label()));
                    }

    // Bonding
    if (calculateBonding)
        structuralSpecies_->addMissingBonds(1.1, preventMetallicBonding);
    else
        applyCIFBonding(structuralSpecies_, preventMetallicBonding);

    structuralConfiguration_->addMolecule(structuralSpecies_);
    structuralConfiguration_->updateObjectRelationships();

    return true;
}

// Create a cleaned structural species
bool CIFSpecies::createCleanedSpecies(bool removeAtomsOfSingleMoiety, bool removeWaterMoleculesOfSingleMoiety,
                                      std::optional<NETADefinition> moietyNETA, std::optional<bool> removeEntireFragment)
{
    if (!structuralSpecies_)
        return false;

    // Remove the old Cleaned species if it exists
    if (cleanedSpecies_)
        coreData_.removeSpecies(cleanedSpecies_);

    cleanedSpecies_ = coreData_.addSpecies();
    cleanedSpecies_->setName("Crystal (Cleaned)");
    cleanedSpecies_->copyBasic(structuralSpecies_);

    // -- Set unit cell
    auto cellLengths = cifHandler_.getCellLengths();
    if (!cellLengths)
        return false;
    auto cellAngles = cifHandler_.getCellAngles();
    if (!cellAngles)
        return false;
    cleanedSpecies_->createBox(cellLengths.value(), cellAngles.value());
    auto *box = cleanedSpecies_->box();

    // Configuration
    cleanedConfiguration_->empty();
    cleanedConfiguration_->createBoxAndCells(cellLengths.value(), cellAngles.value(), false, 1.0);

    if (removeAtomsOfSingleMoiety)
    {
        std::vector<int> indicesToRemove;
        for (const auto &i : cleanedSpecies_->atoms())
            if (i.nBonds() == 0)
                indicesToRemove.push_back(i.index());
        Messenger::print("Atomic removal deleted {} atoms.\n", indicesToRemove.size());

        // Remove selected atoms
        cleanedSpecies_->removeAtoms(indicesToRemove);
    }

    if (removeWaterMoleculesOfSingleMoiety)
    {
        NETADefinition waterVacuum("?O,nbonds=1,nh<=1|?O,nbonds>=2,-H(nbonds=1,-O)");
        if (!waterVacuum.isValid())
        {
            Messenger::error("NETA definition for water removal is invalid.\n");
            return false;
        }

        std::vector<int> indicesToRemove;
        for (const auto &i : cleanedSpecies_->atoms())
            if (waterVacuum.matches(&i))
                indicesToRemove.push_back(i.index());
        Messenger::print("Water removal deleted {} atoms.\n", indicesToRemove.size());

        // Remove selected atoms
        cleanedSpecies_->removeAtoms(indicesToRemove);
    }

    if (moietyNETA.has_value() && moietyNETA.value().isValid())
    {
        // Select all atoms that are in moieties where one of its atoms matches our NETA definition
        std::vector<int> indicesToRemove;
        for (auto &i : cleanedSpecies_->atoms())
            if (moietyNETA.value().matches(&i))
            {
                // Select all atoms that are part of the same moiety?
                if (removeEntireFragment.has_value() && removeEntireFragment.value())
                {
                    cleanedSpecies_->clearAtomSelection();
                    auto selection = cleanedSpecies_->fragment(i.index());
                    std::copy(selection.begin(), selection.end(), std::back_inserter(indicesToRemove));
                }
                else
                    indicesToRemove.push_back(i.index());
            }
        Messenger::print("Moiety removal deleted {} atoms.\n", indicesToRemove.size());

        // Remove selected atoms
        cleanedSpecies_->removeAtoms(indicesToRemove);
    }

    cleanedConfiguration_->addMolecule(cleanedSpecies_);
    cleanedConfiguration_->updateObjectRelationships();

    return true;
}

// Create molecular species
bool CIFSpecies::createMolecularSpecies()
{
    if (!cleanedSpecies_)
        return false;

    molecularSpecies_.clear();

    std::vector<int> indices(cleanedSpecies_->nAtoms());
    std::iota(indices.begin(), indices.end(), 0);

    // Find all CIFSpecies, and their instances
    auto idx = 0;
    while (!indices.empty())
    {
        auto *species = new CIFMolecularSpecies;
        // Choose a fragment
        auto fragment = cleanedSpecies_->fragment(idx);
        std::sort(fragment.begin(), fragment.end());

        // Set up the CIF species
        auto *sp = coreData_.addSpecies();
        sp->copyBasic(cleanedSpecies_);

        // Empty the species of all atoms, except those in the reference instance
        std::vector<int> allIndices(sp->nAtoms());
        std::iota(allIndices.begin(), allIndices.end(), 0);
        std::vector<int> indicesToRemove;
        std::set_difference(allIndices.begin(), allIndices.end(), fragment.begin(), fragment.end(),
                            std::back_inserter(indicesToRemove));
        sp->removeAtoms(indicesToRemove);

        // Determine a unique NETA definition describing the fragment
        auto neta = uniqueNETADefinition(sp);
        if (!neta.has_value())
            return false;

        // Find copies
        auto copies = instances(cleanedSpecies_, neta.value());

        // Remove the current fragment and all copies
        for (auto &copy : copies)
        {
            indices.erase(std::remove_if(indices.begin(), indices.end(),
                                         [&](int value) { return std::find(copy.begin(), copy.end(), value) != copy.end(); }),
                          indices.end());
        }

        // Determine coordinates
        auto coords = coordinates(cleanedSpecies_, copies);

        // Fix geometry
        fixGeometry(sp, cleanedSpecies_->box());

        // Give the species a name
        sp->setName(EmpiricalFormula::formula(sp->atoms(), [&](const auto &at) { return at.Z(); }));

        species->species = sp;
        species->netaString = neta->definitionString();
        species->instances = std::move(copies);
        species->coordinates = std::move(coords);
        molecularSpecies_.push_back(species);
        idx = *std::min_element(indices.begin(), indices.end());
    }
    return true;
}

// Create configuration that composes molecular species
bool CIFSpecies::createMolecularConfiguration(OptionalReferenceWrapper<CoreData> coreData)
{
    CoreData &localCoreData = coreData ? coreData->get() : coreData_;

    // Create a configuration
    molecularConfiguration_ = localCoreData.addConfiguration();
    molecularConfiguration_->setName(cifHandler_.chemicalFormula());

    // Grab the generator
    auto &generator = molecularConfiguration_->generator();

    // Add Box
    auto boxNode = generator.createRootNode<BoxProcedureNode>({});
    auto cellLengths = cifHandler_.getCellLengths().value();
    auto cellAngles = cifHandler_.getCellAngles().value();
    boxNode->keywords().set("Lengths", Vec3<NodeValue>(cellLengths.get(0), cellLengths.get(1), cellLengths.get(2)));
    boxNode->keywords().set("Angles", Vec3<NodeValue>(cellAngles.get(0), cellAngles.get(1), cellAngles.get(2)));

    for (auto &cifMolecularSp : molecularSpecies_)
    {
        auto *sp = cifMolecularSp->species;
        // Add the species if it doesn't already exist
        if (!localCoreData.findSpecies(sp->name()))
            sp = localCoreData.copySpecies(cifMolecularSp->species);

        // Determine a unique suffix
        auto base = sp->name();
        std::string uniqueSuffix{base};
        if (!generator.nodes().empty())
        {
            // Start from the last root node
            auto root = generator.nodes().back();
            auto suffix = 0;

            // We use 'CoordinateSets' here, because in this instance we are working with (CoordinateSet, Add) pairs
            while (generator.rootSequence().nodeInScope(root, fmt::format("SymmetryCopies_{}", uniqueSuffix)) != nullptr)
                uniqueSuffix = fmt::format("{}_{:02d}", base, ++suffix);
        }

        // CoordinateSets
        auto coordsNode =
            generator.createRootNode<CoordinateSetsProcedureNode>(fmt::format("SymmetryCopies_{}", uniqueSuffix), sp);
        coordsNode->keywords().setEnumeration("Source", CoordinateSetsProcedureNode::CoordinateSetSource::File);
        coordsNode->setSets(cifMolecularSp->coordinates);

        // Add
        auto addNode = generator.createRootNode<AddProcedureNode>(fmt::format("Add_{}", uniqueSuffix), coordsNode);
        addNode->keywords().set("Population", NodeValue(int(cifMolecularSp->coordinates.size())));
        addNode->keywords().setEnumeration("Positioning", AddProcedureNode::PositioningType::Current);
        addNode->keywords().set("Rotate", false);
        addNode->keywords().setEnumeration("BoxAction", AddProcedureNode::BoxActionStyle::None);
    }

    return true;
}

// Create supercell species
bool CIFSpecies::createSupercellSpecies(Vec3<int> repeat, bool calculateBonding, bool preventMetallicBonding)
{
    if (!cleanedSpecies_)
        return false;

    // Remove the old Supercell species if it exists
    if (supercellSpecies_)
        coreData_.removeSpecies(supercellSpecies_);

    supercellSpecies_ = coreData_.addSpecies();
    supercellSpecies_->setName("Supercell");

    auto supercellLengths = cleanedSpecies_->box()->axisLengths();
    supercellLengths.multiply(repeat.x, repeat.y, repeat.z);
    supercellSpecies_->createBox(supercellLengths, cleanedSpecies_->box()->axisAngles(), false);

    // Configuration
    supercellConfiguration_->empty();
    supercellConfiguration_->createBoxAndCells(supercellLengths, cleanedSpecies_->box()->axisAngles(), false, 1.0);

    // Copy atoms from the Crystal species - we'll do the bonding afterwards
    supercellSpecies_->atoms().reserve(repeat.x * repeat.y * repeat.z * cleanedSpecies_->nAtoms());
    for (auto ix = 0; ix < repeat.x; ++ix)
        for (auto iy = 0; iy < repeat.y; ++iy)
            for (auto iz = 0; iz < repeat.z; ++iz)
            {
                Vec3<double> deltaR = cleanedSpecies_->box()->axes() * Vec3<double>(ix, iy, iz);
                for (const auto &i : cleanedSpecies_->atoms())
                    supercellSpecies_->addAtom(i.Z(), i.r() + deltaR, 0.0, i.atomType());
            }

    if (calculateBonding)
        supercellSpecies_->addMissingBonds();
    else
        applyCIFBonding(supercellSpecies_, preventMetallicBonding);

    // Add the structural species to the configuration
    supercellConfiguration_->addMolecule(supercellSpecies_);
    supercellConfiguration_->updateObjectRelationships();

    return true;
}

/*
 * Helpers
 */

// Apply CIF bonding to a given species
void CIFSpecies::applyCIFBonding(Species *sp, bool preventMetallicBonding)
{
    auto *box = sp->box();
    auto pairs = PairIterator(sp->nAtoms());
    for (auto pair : pairs)
    {
        // Grab indices and atom references
        auto [indexI, indexJ] = pair;
        if (indexI == indexJ)
            continue;

        auto &i = sp->atom(indexI);
        auto &j = sp->atom(indexJ);

        // Prevent metallic bonding?
        if (preventMetallicBonding && Elements::isMetallic(i.Z()) && Elements::isMetallic(j.Z()))
            continue;

        // Retrieve distance
        auto r = cifHandler_.bondDistance(i.atomType()->name(), j.atomType()->name());
        if (!r)
            continue;
        else if (fabs(box->minimumDistance(i.r(), j.r()) - r.value()) < 1.0e-2)
            sp->addBond(&i, &j);
    }
}

// Determine a unique NETA definition corresponding to a given species
std::optional<NETADefinition> CIFSpecies::uniqueNETADefinition(Species *sp)
{
    NETADefinition neta;
    auto nMatches = 0, idx = 0;
    while (nMatches != 1)
    {
        if (idx >= sp->nAtoms())
            return std::nullopt;
        neta.create(&sp->atom(idx++), std::nullopt,
                    Flags<NETADefinition::NETACreationFlags>(NETADefinition::NETACreationFlags::ExplicitHydrogens,
                                                             NETADefinition::NETACreationFlags::IncludeRootElement));
        nMatches = std::count_if(sp->atoms().begin(), sp->atoms().end(), [&](const auto &i) { return neta.matches(&i); });
    }
    return neta;
}

// Determine instances of a NETA definition in a given species
std::vector<std::vector<int>> CIFSpecies::instances(Species *sp, NETADefinition neta)
{
    std::vector<std::vector<int>> instances;
    for (auto &i : sp->atoms())
    {
        if (neta.matches(&i))
        {
            auto matchedGroup = neta.matchedPath(&i);
            auto matchedAtoms = matchedGroup.set();
            std::vector<int> indices(matchedAtoms.size());
            std::transform(matchedAtoms.begin(), matchedAtoms.end(), indices.begin(),
                           [](const auto &atom) { return atom->index(); });
            std::sort(indices.begin(), indices.end());
            instances.push_back(std::move(indices));
        }
    }
    return instances;
}

// Determine coordinates of instances in a given species
std::vector<std::vector<Vec3<double>>> CIFSpecies::coordinates(Species *sp, std::vector<std::vector<int>> instances)
{
    std::vector<std::vector<Vec3<double>>> coordinates;
    for (auto &instance : instances)
    {
        std::vector<Vec3<double>> coords(instance.size());
        std::transform(instance.begin(), instance.end(), coords.begin(), [&](auto i) { return sp->atom(i).r(); });
        coordinates.push_back(std::move(coords));
    }
    return coordinates;
}

// 'Fix' the geometry of a given species
void CIFSpecies::fixGeometry(Species *sp, const Box *box)
{
    // 'Fix' the geometry of the species
    // Construct a temporary molecule, which is just the species.
    std::shared_ptr<Molecule> mol = std::make_shared<Molecule>();
    std::vector<Atom> molAtoms(sp->nAtoms());
    for (auto &&[spAtom, atom] : zip(sp->atoms(), molAtoms))
    {
        atom.setSpeciesAtom(&spAtom);
        atom.setCoordinates(spAtom.r());
        mol->addAtom(&atom);
    }

    // Unfold the molecule
    mol->unFold(box);

    // Update the coordinates of the species atoms
    for (auto &&[spAtom, atom] : zip(sp->atoms(), molAtoms))
        spAtom.setCoordinates(atom.r());

    // Set the centre of geometry of the species to be at the origin.
    sp->setCentre(box, {0., 0., 0.});
}

/*
 * Retrieval
 */

// Structural
Species *CIFSpecies::structuralSpecies() { return structuralSpecies_; }
Configuration *CIFSpecies::structuralConfiguration() { return structuralConfiguration_; }

// Cleaned
Species *CIFSpecies::cleanedSpecies() { return cleanedSpecies_; }
Configuration *CIFSpecies::cleanedConfiguration() { return cleanedConfiguration_; }

// Molecular
std::vector<Species *> CIFSpecies::molecularSpecies()
{
    std::vector<Species *> molecularSpecies;
    for (auto &cifMolecularSpecies : molecularSpecies_)
        molecularSpecies.push_back(cifMolecularSpecies->species);
    return molecularSpecies;
}
Configuration *CIFSpecies::molecularConfiguration() { return molecularConfiguration_; }

// Supercell
Species *CIFSpecies::supercellSpecies() { return supercellSpecies_; }
Configuration *CIFSpecies::supercellConfiguration() { return supercellConfiguration_; }