// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "io/import/cifClasses.h"
#include "classes/empiricalFormula.h"
#include "classes/molecule.h"

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
 * CIF Molecular Species
 */

CIFMolecularSpecies::CIFMolecularSpecies(const Species *targetSpecies, std::string_view netaString,
                                         std::vector<std::vector<Vec3<double>>> coordinates)
    : species_(targetSpecies), netaString_(netaString), coordinates_(coordinates)
{
}

// Return species to which the definitions relate
const Species *CIFMolecularSpecies::species() const { return species_; };

// Return NETA string for the species
std::string_view CIFMolecularSpecies::netaString() const { return netaString_; };

// Return coordinates of instances
const std::vector<std::vector<Vec3<double>>> &CIFMolecularSpecies::coordinates() const { return coordinates_; }

std::vector<std::vector<Vec3<double>>> &CIFMolecularSpecies::coordinates() { return coordinates_; }
