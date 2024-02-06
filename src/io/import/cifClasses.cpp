// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "io/import/cifClasses.h"
#include "classes/empiricalFormula.h"
#include "classes/molecule.h"
#include "classes/species.h"

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

/*
 * CIF Local Molecule
 */

CIFLocalMolecule::CIFLocalMolecule(const Species *sp)
{
    species_ = sp;

    localAtoms_.resize(sp->nAtoms());
    atoms_.resize(sp->nAtoms());
    unitCellIndices_.resize(sp->nAtoms());
    std::transform(localAtoms_.begin(), localAtoms_.end(), atoms_.begin(), [](auto &atom) { return &atom; });

    auto parent = shared_from_this();
    for (auto &&[atom, spAtom] : zip(localAtoms_, species_->atoms()))
    {
        atom.setMolecule(parent);
        atom.setSpeciesAtom(&spAtom);
    }
}

// Set Species that this Molecule represents
void CIFLocalMolecule::setSpecies(const Species *sp) { throw(std::runtime_error("Can't setSpecies() in a LocalMolecule.\n")); }

// Add Atom to Molecule
void CIFLocalMolecule::addAtom(Atom *atom) { throw(std::runtime_error("Can't addAtom() in a LocalMolecule.\n")); }

// Update local atom pointers from main vector
void CIFLocalMolecule::updateAtoms(std::vector<Atom> &mainAtoms, int offset)
{
    throw(std::runtime_error("Can't updateAtoms() in a LocalMolecule.\n"));
}

// Set coordinates and local unit cell index of the specified atom
void CIFLocalMolecule::setAtom(int index, const Vec3<double> &r, int unitCellIndex)
{
    localAtoms_[index].setCoordinates(r);
    unitCellIndices_[index] = unitCellIndex;
}

// Return local atoms
std::vector<Atom> &CIFLocalMolecule::localAtoms() { return localAtoms_; }
const std::vector<Atom> &CIFLocalMolecule::localAtoms() const { return localAtoms_; };

// Return local unit cell indices for the atoms
const std::vector<int> &CIFLocalMolecule::unitCellIndices() const { return unitCellIndices_; }

/*
 * CIF Molecular Species
 */

CIFMolecularSpecies::CIFMolecularSpecies(const Species *targetSpecies, std::string_view netaString,
                                         std::vector<CIFLocalMolecule> instances)
    : species_(targetSpecies), netaString_(netaString), instances_(std::move(instances))
{
}

// Return species to which the definitions relate
const Species *CIFMolecularSpecies::species() const { return species_; };

// Return NETA string for the species
std::string_view CIFMolecularSpecies::netaString() const { return netaString_; };

// Return molecule instances
const std::vector<CIFLocalMolecule> &CIFMolecularSpecies::instances() const { return instances_; }
std::vector<CIFLocalMolecule> &CIFMolecularSpecies::instances() { return instances_; }

// Append supplied instances to our vector
void CIFMolecularSpecies::appendInstances(const std::vector<CIFLocalMolecule> &newInstances)
{
    // Increase our reservation
    instances_.reserve(instances_.size() + newInstances.size());
    instances_.insert(instances_.end(), newInstances.begin(), newInstances.end());
}

// Return coordinates for all instances as a vector of vectors
std::vector<std::vector<Vec3<double>>> CIFMolecularSpecies::allInstanceCoordinates() const
{
    std::vector<std::vector<Vec3<double>>> coordinates;

    for (auto &instance : instances_)
    {
        std::vector<Vec3<double>> instanceCoords;
        instanceCoords.reserve(species_->nAtoms());
        for (auto &atom : instance.localAtoms())
            instanceCoords.emplace_back(atom.r());

        coordinates.emplace_back(instanceCoords);
    }

    return coordinates;
}
