// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "io/import/cifClasses.h"
#include "io/import/cif.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/coreData.h"
#include "classes/empiricalFormula.h"
#include "classes/molecule.h"
#include "classes/species.h"
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


CIFSpecies::CIFSpecies(CIFImport& cifImporter, CoreData &coreData) : cifImporter_(cifImporter), coreData_(coreData) {}

Species* CIFSpecies::createStructuralSpecies(double tolerance, bool calculateBonding, bool preventMetallicBonding)
{
    // Create temporary atom types corresponding to the unique atom labels
    for (auto &a : cifImporter_.assemblies())
        for (auto &g : a.groups())
            if (g.active())
                for (auto &i : g.atoms())
                    if (!coreData_.findAtomType(i.label()))
                    {
                        auto at = coreData_.addAtomType(i.Z());
                        at->setName(i.label());
                    }

    // Generate a single species containing the entire crystal
    auto* sp = coreData_.addSpecies();
    sp->setName("Crystal");

    // -- Set unit cell
    auto cellLengths = cifImporter_.getCellLengths();
    if (!cellLengths)
        return nullptr;
    auto cellAngles = cifImporter_.getCellAngles();
    if (!cellAngles)
        return nullptr;
    sp->createBox(cellLengths.value(), cellAngles.value());
    auto *box = sp->box();

    // -- Generate atoms
    auto symmetryGenerators = SpaceGroups::symmetryOperators(cifImporter_.spaceGroup());
    for (const auto &generator : symmetryGenerators)
        for (auto &a : cifImporter_.assemblies())
            for (auto &g : a.groups())
                if (g.active())
                    for (auto &unique : g.atoms())
                    {
                        // Generate folded atomic position in real space
                        auto r = generator * unique.rFrac();
                        box->toReal(r);
                        r = box->fold(r);

                        // If this atom overlaps with another in the box, don't add it as it's a symmetry-related copy
                        if (std::any_of(sp->atoms().begin(), sp->atoms().end(),
                                        [&r, box, tolerance](const auto &j)
                                        { return box->minimumDistance(r, j.r()) < tolerance; }))
                            continue;

                        // Create the new atom
                        auto i = sp->addAtom(unique.Z(), r);
                        sp->atom(i).setAtomType(coreData_.findAtomType(unique.label()));
                    }

    // Bonding
    if (calculateBonding)
        sp->addMissingBonds(1.1, preventMetallicBonding);
    else
        applyCIFBonding(sp, preventMetallicBonding);

    return sp;
}

void CIFSpecies::applyCIFBonding(Species* sp, bool preventMetallicBonding)
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
        auto r = cifImporter_.bondDistance(i.atomType()->name(), j.atomType()->name());
        if (!r)
            continue;
        else if (fabs(box->minimumDistance(i.r(), j.r()) - r.value()) < 1.0e-2)
            sp->addBond(&i, &j);
    }
}

Species* CIFSpecies::createCleanedSpecies(Species *refSp, bool removeAtomsOfSingleMoiety, bool removeWaterMoleculesOfSingleMoiety, std::optional<NETADefinition> moietyNETA, std::optional<bool> removeEntireFragment)
{
    auto* sp = coreData_.addSpecies();
    sp->setName("Crystal (Cleaned)");
    if (!refSp)
        return nullptr;
    sp->copyBasic(refSp);

    // -- Set unit cell
    auto cellLengths = cifImporter_.getCellLengths();
    if (!cellLengths)
        return nullptr;
    auto cellAngles = cifImporter_.getCellAngles();
    if (!cellAngles)
        return nullptr;
    sp->createBox(cellLengths.value(), cellAngles.value());
    auto *box = sp->box();

    if (removeAtomsOfSingleMoiety)
    {
        std::vector<int> indicesToRemove;
        for (const auto &i : sp->atoms())
            if (i.nBonds() == 0)
                indicesToRemove.push_back(i.index());
        Messenger::print("Atomic removal deleted {} atoms.\n", indicesToRemove.size());

        // Remove selected atoms
        sp->removeAtoms(indicesToRemove);
    }

    if (removeWaterMoleculesOfSingleMoiety)
    {
        NETADefinition waterVacuum("?O,nbonds=1,nh<=1|?O,nbonds>=2,-H(nbonds=1,-O)");
        if (!waterVacuum.isValid())
        {
            Messenger::error("NETA definition for water removal is invalid.\n");
            return nullptr;
        }

        std::vector<int> indicesToRemove;
        for (const auto &i : sp->atoms())
            if (waterVacuum.matches(&i))
                indicesToRemove.push_back(i.index());
        Messenger::print("Water removal deleted {} atoms.\n", indicesToRemove.size());

        // Remove selected atoms
        sp->removeAtoms(indicesToRemove);
    }

    if (moietyNETA.has_value() && moietyNETA.value().isValid())
    {
        // Select all atoms that are in moieties where one of its atoms matches our NETA definition
        std::vector<int> indicesToRemove;
        for (auto &i : sp->atoms())
            if (moietyNETA.value().matches(&i))
            {
                // Select all atoms that are part of the same moiety?
                if (removeEntireFragment.has_value() && removeEntireFragment.value())
                {
                    sp->clearAtomSelection();
                    auto selection = sp->fragment(i.index());
                    std::copy(selection.begin(), selection.end(), std::back_inserter(indicesToRemove));
                }
                else
                    indicesToRemove.push_back(i.index());
            }
        Messenger::print("Moiety removal deleted {} atoms.\n", indicesToRemove.size());

        // Remove selected atoms
        sp->removeAtoms(indicesToRemove);
    }

    return sp;
}

std::vector<CIFSpecies::CIFMolecularSpecies*> CIFSpecies::createMolecularSpecies(Species *refSp)
{
    std::vector<CIFSpecies::CIFMolecularSpecies*> distinctSpecies;

    auto cfg = coreData_.addConfiguration();
    cfg->setName(cifImporter_.chemicalFormula());

    // Grab the generator
    auto &generator = cfg->generator();

    // Add Box
    auto boxNode = generator.createRootNode<BoxProcedureNode>({});
    auto cellLengths = cifImporter_.getCellLengths().value();
    auto cellAngles = cifImporter_.getCellAngles().value();
    boxNode->keywords().set("Lengths", Vec3<NodeValue>(cellLengths.get(0), cellLengths.get(1), cellLengths.get(2)));
    boxNode->keywords().set("Angles", Vec3<NodeValue>(cellAngles.get(0), cellAngles.get(1), cellAngles.get(2)));

    std::vector<int> indices(refSp->nAtoms());
    std::iota(indices.begin(), indices.end(), 0);

    // Find all CIFSpecies, and their instances
    auto idx = 0;
    while (!indices.empty())
    {
        auto* species = new CIFMolecularSpecies;
        // Choose a fragment
        auto fragment = refSp->fragment(idx);
        std::sort(fragment.begin(), fragment.end());

        // Set up the CIF species
        auto *sp = coreData_.addSpecies();
        sp->copyBasic(refSp);

        // Empty the species of all atoms, except those in the reference instance
        std::vector<int> allIndices(sp->nAtoms());
        std::iota(allIndices.begin(), allIndices.end(), 0);
        std::vector<int> indicesToRemove;
        std::set_difference(allIndices.begin(), allIndices.end(), fragment.begin(), fragment.end(),
                            std::back_inserter(indicesToRemove));
        sp->removeAtoms(indicesToRemove);

        auto neta = uniqueNETADefinition(sp);
        if (!neta.has_value())
            return {};

        auto copies = instances(refSp, neta.value());

        // Remove the current fragment, and all instances of the underlying CIFMolecularSpecies
        for (auto& copy : copies)
        {
            indices.erase(std::remove_if(indices.begin(), indices.end(),
                                         [&](int value)
                                         { return std::find(copy.begin(), copy.end(), value) != copy.end(); }),
                          indices.end());
        }

        auto coords = coordinates(refSp, copies);

        fixGeometry(sp, refSp->box());

        species->species = sp;
        species->netaString = neta->definitionString();
        species->instances = std::move(copies);
        species->coordinates = std::move(coords);
        distinctSpecies.push_back(species);
        idx = *std::min_element(indices.begin(), indices.end());
        /*
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
        coordsNode->setSets(coords);

        // Add
        auto addNode = generator.createRootNode<AddProcedureNode>(fmt::format("Add_{}", uniqueSuffix), coordsNode);
        addNode->keywords().set("Population", NodeValue(int(coords.size())));
        addNode->keywords().setEnumeration("Positioning", AddProcedureNode::PositioningType::Current);
        addNode->keywords().set("Rotate", false);
        addNode->keywords().setEnumeration("BoxAction", AddProcedureNode::BoxActionStyle::None);

        species_.push_back(sp);
        */
    }
    return distinctSpecies;
}

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
        nMatches =
            std::count_if(sp->atoms().begin(), sp->atoms().end(), [&](const auto &i) { return neta.matches(&i); });
    }
    return neta;
}

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

Species *CIFSpecies::createSupercellSpecies(Species *refSp, Vec3<int> repeat, bool calculateBonding)
{
    auto *sp = coreData_.addSpecies();
    sp->setName("Supercell");

    if (!refSp)
        return nullptr;

    auto supercellLengths = refSp->box()->axisLengths();
    supercellLengths.multiply(repeat.x, repeat.y, repeat.z);
    sp->createBox(supercellLengths, refSp->box()->axisAngles(), false);

    // Copy atoms from the Crystal species - we'll do the bonding afterwards
    sp->atoms().reserve(repeat.x * repeat.y * repeat.z * refSp->nAtoms());
    for (auto ix = 0; ix < repeat.x; ++ix)
        for (auto iy = 0; iy < repeat.y; ++iy)
            for (auto iz = 0; iz < repeat.z; ++iz)
            {
                Vec3<double> deltaR = refSp->box()->axes() * Vec3<double>(ix, iy, iz);
                for (const auto &i : refSp->atoms())
                    sp->addAtom(i.Z(), i.r() + deltaR, 0.0, i.atomType());
            }

    if (calculateBonding)
        sp->addMissingBonds();
    else
        applyCIFBonding(refSp, false);

    return sp;
}

Configuration* CIFSpecies::generateConfiguration(Species *sp, std::string name)
{
    // Create a configuration
    auto* cfg = coreData_.addConfiguration();
    cfg->setName(name);

    auto cellLengths = cifImporter_.getCellLengths();
    if (!cellLengths)
        return nullptr;
    auto cellAngles = cifImporter_.getCellAngles();
    if (!cellAngles)
        return nullptr;

    cfg->createBoxAndCells(cellLengths.value(), cellAngles.value(), false, 1.0);

    // Add the species to the configuration
    cfg->addMolecule(sp);
    cfg->updateObjectRelationships();

    return cfg;
}