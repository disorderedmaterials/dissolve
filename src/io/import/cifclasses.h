// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "data/elements.h"
#include "templates/optionalref.h"
#include "templates/vector3.h"

// CIF Symmetry-Unique Atom
class CIFSymmetryAtom
{
    public:
    CIFSymmetryAtom(std::string_view label, Elements::Element Z, Vec3<double> rFrac, double occ);
    ~CIFSymmetryAtom() = default;

    private:
    // Label (from _atom_site_label)
    std::string label_;
    // Element (from  _atom_site_type_symbol)
    Elements::Element Z_;
    // Fractional coordinate of atom (from _atom_site_fract_[xyz])
    Vec3<double> rFrac_;
    // Site occupancy (from _atom_site_occupancy)
    double occupancy_;

    public:
    // Return label (from _atom_site_label)
    std::string_view label() const;
    // Return element (from  _atom_site_type_symbol)
    Elements::Element Z() const;
    // Return fractional coordinate of atom (from _atom_site_fract_[xyz])
    Vec3<double> rFrac() const;
    // Return site occupancy (from _atom_site_occupancy)
    double occupancy() const;
};

// CIF Bonding Pair
class CIFBondingPair
{
    public:
    CIFBondingPair(std::string_view labelI, std::string_view labelJ, double r);
    ~CIFBondingPair() = default;

    private:
    // Labels of involved atoms (from _geom_bond_atom_site_label_[12])
    std::string labelI_, labelJ_;
    // Distance between bonded pair (from _geom_bond_distance)
    double r_;

    public:
    // Return labels of involved atom i (from _geom_bond_atom_site_label_1)
    std::string_view labelI() const;
    // Return labels of involved atom i (from _geom_bond_atom_site_label_2)
    std::string_view labelJ() const;
    // Return distance between bonded pair (from _geom_bond_distance)
    double r() const;
};

// CIF Atom Group
class CIFAtomGroup
{
    public:
    CIFAtomGroup(std::string_view name);
    ~CIFAtomGroup() = default;

    private:
    // Group name (from _atom_site_disorder_group)
    std::string name_;
    // Atoms in the group
    std::vector<CIFSymmetryAtom> atoms_;

    public:
    // Group name (from _atom_site_disorder_group)
    std::string_view name() const;
    // Add atom to the group
    void addAtom(const CIFSymmetryAtom &i);
    // Return atoms in the group
    const std::vector<CIFSymmetryAtom> &atoms() const;
};

// CIF Assembly
class CIFAssembly
{
    public:
    CIFAssembly(std::string_view name);
    ~CIFAssembly() = default;

    private:
    // Name of the assembly (from _atom_site_disorder_assembly or 'Global')
    std::string name_;
    // Available atom groups
    std::vector<CIFAtomGroup> groups_;
    // Active group
    OptionalReferenceWrapper<const CIFAtomGroup> activeGroup_;

    public:
    // Return name of the assembly (from _atom_site_disorder_assembly or 'Global')
    std::string_view name() const;
    // Return all groups
    const std::vector<CIFAtomGroup> &groups() const;
    // Get (add or retrieve) named group
    CIFAtomGroup &getGroup(std::string_view groupName);
    // Return the number of defined groups
    int nGroups() const;
    // Set the active group
    void setActiveGroup(const CIFAtomGroup &group);
    // Return the active group
    const CIFAtomGroup &activeGroup() const;
    // Return whether the specified group is the active one
    bool isActiveGroup(const CIFAtomGroup &group) const;
};
