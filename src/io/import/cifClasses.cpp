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


CIFStructuralSpecies::CIFStructuralSpecies(CoreData &coreData) : coreData_(coreData){}

Species *CIFStructuralSpecies::species()
{
    return species_;
}

Configuration* CIFStructuralSpecies::configuration()
{
    return configuration_;
}

bool CIFStructuralSpecies::create(CIFImport cifImporter, double tolerance, bool calculateBonding, bool preventMetallicBonding)
{
    // Create temporary atom types corresponding to the unique atom labels
    for (auto &a : cifImporter.assemblies())
        for (auto &g : a.groups())
            if (g.active())
                for (auto &i : g.atoms())
                    if (!coreData_.findAtomType(i.label()))
                    {
                        auto at = coreData_.addAtomType(i.Z());
                        at->setName(i.label());
                    }

    // Generate a single species containing the entire crystal
    species_ = coreData_.addSpecies();
    species_->setName("Crystal");

    // Generate a configuration
    configuration_ = coreData_.addConfiguration();
    configuration_->setName("Structure");

    // -- Set unit cell
    auto cellLengths = cifImporter.getCellLengths();
    if (!cellLengths)
        return false;
    auto cellAngles = cifImporter.getCellAngles();
    if (!cellAngles)
        return false;
    species_->createBox(cellLengths.value(), cellAngles.value());
    auto *box = species_->box();
    configuration_->createBoxAndCells(cellLengths.value(), cellAngles.value(), false, 1.0);

    // -- Generate atoms
    auto symmetryGenerators = SpaceGroups::symmetryOperators(cifImporter.spaceGroup());
    for (const auto &generator : symmetryGenerators)
        for (auto &a : cifImporter.assemblies())
            for (auto &g : a.groups())
                if (g.active())
                    for (auto &unique : g.atoms())
                    {
                        // Generate folded atomic position in real space
                        auto r = generator * unique.rFrac();
                        box->toReal(r);
                        r = box->fold(r);

                        // If this atom overlaps with another in the box, don't add it as it's a symmetry-related copy
                        if (std::any_of(species_->atoms().begin(), species_->atoms().end(),
                                        [&r, box, tolerance](const auto &j)
                                        { return box->minimumDistance(r, j.r()) < tolerance; }))
                            continue;

                        // Create the new atom
                        auto i = species_->addAtom(unique.Z(), r);
                        species_->atom(i).setAtomType(coreData_.findAtomType(unique.label()));
                    }

    // Bonding
    if (calculateBonding)
        species_->addMissingBonds(1.1, preventMetallicBonding);
    else
        applyCIFBonding(cifImporter, preventMetallicBonding);

    // Add the structural species to the configuration
    configuration_->addMolecule(species_);
    configuration_->updateObjectRelationships();

    return true;
}

void CIFStructuralSpecies::applyCIFBonding(CIFImport cifImporter, bool preventMetallicBonding)
{
    auto *box = species_->box();
    auto pairs = PairIterator(species_->nAtoms());
    for (auto pair : pairs)
    {
        // Grab indices and atom references
        auto [indexI, indexJ] = pair;
        if (indexI == indexJ)
            continue;
        auto &i = species_->atom(indexI);
        auto &j = species_->atom(indexJ);

        // Prevent metallic bonding?
        if (preventMetallicBonding && Elements::isMetallic(i.Z()) && Elements::isMetallic(j.Z()))
            continue;

        // Retrieve distance
        auto r = cifImporter.bondDistance(i.atomType()->name(), j.atomType()->name());
        if (!r)
            continue;
        else if (fabs(box->minimumDistance(i.r(), j.r()) - r.value()) < 1.0e-2)
            species_->addBond(&i, &j);
    }
}

/*
 * CIF Species
 */

CIFSpecies::CIFSpecies(Species *spRef, Species *sp, std::vector<int> referenceInstance)
    : speciesRef_(spRef), species_(sp), referenceInstance_(std::move(referenceInstance))
{
    // Empty the species of all atoms, except those in the reference instance.
    std::vector<int> allIndices(species_->nAtoms());
    std::iota(allIndices.begin(), allIndices.end(), 0);
    std::vector<int> indicesToRemove;
    std::set_difference(allIndices.begin(), allIndices.end(), referenceInstance_.begin(), referenceInstance_.end(),
                        std::back_inserter(indicesToRemove));
    species_->removeAtoms(indicesToRemove);

    // Find instances
    hasSymmetry_ = !findInstances();

    // Fix geometry
    fixGeometry(speciesRef_->box());

    // Determine coordinates
    determineCoordinates();

    // Give the species a name
    species_->setName(EmpiricalFormula::formula(species_->atoms(), [&](const auto &at) { return at.Z(); }));
}

// Return the output species
const Species *CIFSpecies::species() const { return species_; }

// Return the NETA definition string that uniquely describes the reference instance
const std::string CIFSpecies::netaString() const { return netaString_; }

// Return the reference instance
const std::vector<int> &CIFSpecies::referenceInstance() const { return referenceInstance_; }

// Return all found instances
const std::vector<std::vector<int>> &CIFSpecies::instances() const { return instances_; }

// Return the coordinates corresponding to the instances
const std::vector<std::vector<Vec3<double>>> &CIFSpecies::coordinates() const { return coordinates_; }

// Return whether the reference instance contains symmetry.
bool CIFSpecies::hasSymmetry() const { return hasSymmetry_; }

// Inclusively, find all instances of the reference instance
bool CIFSpecies::findInstances()
{
    NETADefinition neta;
    auto nMatches = 0, idx = 0;
    while (nMatches != 1)
    {
        if (idx >= species_->nAtoms())
            return false;
        neta.create(&species_->atom(idx++), std::nullopt,
                    Flags<NETADefinition::NETACreationFlags>(NETADefinition::NETACreationFlags::ExplicitHydrogens,
                                                             NETADefinition::NETACreationFlags::IncludeRootElement));
        nMatches =
            std::count_if(species_->atoms().begin(), species_->atoms().end(), [&](const auto &i) { return neta.matches(&i); });
    }

    netaString_ = neta.definitionString();

    for (auto &i : speciesRef_->atoms())
    {
        if (neta.matches(&i))
        {
            auto matchedGroup = neta.matchedPath(&i);
            auto matchedAtoms = matchedGroup.set();
            std::vector<int> indices(matchedAtoms.size());
            std::transform(matchedAtoms.begin(), matchedAtoms.end(), indices.begin(),
                           [](const auto &atom) { return atom->index(); });
            std::sort(indices.begin(), indices.end());
            instances_.push_back(std::move(indices));
        }
    }

    return true;
}

// Determine the coordinates corresponding to the instances
void CIFSpecies::determineCoordinates()
{
    for (auto &instance : instances_)
    {
        std::vector<Vec3<double>> coords(instance.size());
        std::transform(instance.begin(), instance.end(), coords.begin(), [&](auto i) { return speciesRef_->atom(i).r(); });
        coordinates_.push_back(std::move(coords));
    }
}

// Fix the geometry of the output species, by unfolding it
void CIFSpecies::fixGeometry(const Box *box)
{
    // 'Fix' the geometry of the species
    // Construct a temporary molecule, which is just the species.
    std::shared_ptr<Molecule> mol = std::make_shared<Molecule>();
    std::vector<Atom> molAtoms(species_->nAtoms());
    for (auto &&[spAtom, atom] : zip(species_->atoms(), molAtoms))
    {
        atom.setSpeciesAtom(&spAtom);
        atom.setCoordinates(spAtom.r());
        mol->addAtom(&atom);
    }

    // Unfold the molecule
    mol->unFold(box);

    // Update the coordinates of the species atoms
    for (auto &&[spAtom, atom] : zip(species_->atoms(), molAtoms))
        spAtom.setCoordinates(atom.r());

    // Set the centre of geometry of the species to be at the origin.
    species_->setCentre(box, {0., 0., 0.});
}