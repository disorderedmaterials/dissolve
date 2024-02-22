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

CIFLocalMolecule::CIFLocalMolecule(const CIFLocalMolecule &copyFrom) { copyData(copyFrom); }

CIFLocalMolecule::CIFLocalMolecule(CIFLocalMolecule &&moveFrom) { copyData(moveFrom); }

CIFLocalMolecule &CIFLocalMolecule::operator=(const CIFLocalMolecule &copyFrom)
{
    copyData(copyFrom);
    return *this;
}

CIFLocalMolecule &CIFLocalMolecule::operator=(CIFLocalMolecule &&moveFrom)
{
    copyData(moveFrom);
    return *this;
}

// Copy data from specified object
void CIFLocalMolecule::copyData(const CIFLocalMolecule &object)
{
    species_ = object.species_;

    localAtoms_ = object.localAtoms_;
    atoms_.resize(localAtoms_.size());
    std::transform(localAtoms_.begin(), localAtoms_.end(), atoms_.begin(), [](auto &atom) { return &atom; });
}

// Set Species that this Molecule represents
void CIFLocalMolecule::setSpecies(const Species *sp)
{
    species_ = sp;

    localAtoms_.resize(sp->nAtoms());
    atoms_.resize(sp->nAtoms());
    std::transform(localAtoms_.begin(), localAtoms_.end(), atoms_.begin(), [](auto &atom) { return &atom; });

    for (auto &&[atom, spAtom] : zip(localAtoms_, species_->atoms()))
        atom.setSpeciesAtom(&spAtom);
}

// Add Atom to Molecule
void CIFLocalMolecule::addAtom(Atom *atom) { throw(std::runtime_error("Can't addAtom() in a LocalMolecule.\n")); }

// Update local atom pointers from main vector
void CIFLocalMolecule::updateAtoms(std::vector<Atom> &mainAtoms, int offset)
{
    throw(std::runtime_error("Can't updateAtoms() in a LocalMolecule.\n"));
}

// Return nth local atom
Atom &CIFLocalMolecule::localAtom(int n) { return localAtoms_[n]; }
const Atom &CIFLocalMolecule::localAtom(int n) const { return localAtoms_[n]; }

// Return local atoms
std::vector<Atom> &CIFLocalMolecule::localAtoms() { return localAtoms_; }
const std::vector<Atom> &CIFLocalMolecule::localAtoms() const { return localAtoms_; };

/*
 * CIF Molecular Species
 */

CIFMolecularSpecies::CIFMolecularSpecies() : species_(std::make_shared<Species>()) {}

// Return species parent for molecule instances
std::shared_ptr<Species> &CIFMolecularSpecies::species() { return species_; };
const std::shared_ptr<Species> &CIFMolecularSpecies::species() const { return species_; };

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
