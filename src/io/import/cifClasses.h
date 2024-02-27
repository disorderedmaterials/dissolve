// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/molecule.h"
#include "classes/species.h"
#include "data/elements.h"
#include "templates/vector3.h"
#include <algorithm>
#include <vector>

// Forward Declarations
class Species;

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
    // Whether the group is active (included in unit cell generation)
    bool active_{false};

    public:
    // Group name (from _atom_site_disorder_group)
    std::string_view name() const;
    // Add atom to the group
    void addAtom(const CIFSymmetryAtom &i);
    // Return atoms in the group
    const std::vector<CIFSymmetryAtom> &atoms() const;
    // Set whether the group is active (included in unit cell generation)
    void setActive(bool b);
    // Return whether the group is active (included in unit cell generation)
    bool active() const;
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

    public:
    // Return name of the assembly (from _atom_site_disorder_assembly or 'Global')
    std::string_view name() const;
    // Return all groups
    std::vector<CIFAtomGroup> &groups();
    const std::vector<CIFAtomGroup> &groups() const;
    // Get (add or retrieve) named group
    CIFAtomGroup &getGroup(std::string_view groupName);
    // Return the number of defined groups
    int nGroups() const;
};

// CIF Local Molecule Definition
class CIFLocalMolecule : public Molecule
{
    public:
    CIFLocalMolecule() = default;
    ~CIFLocalMolecule() = default;
    CIFLocalMolecule(const CIFLocalMolecule &copyFrom);
    CIFLocalMolecule(CIFLocalMolecule &&moveFrom);
    CIFLocalMolecule &operator=(const CIFLocalMolecule &copyFrom);
    CIFLocalMolecule &operator=(CIFLocalMolecule &&moveFrom);

    private:
    // Local vector of Atoms that belong to this Molecule and their original unit cell indices
    AtomVector localAtoms_;
    std::vector<int> unitCellIndices_;

    private:
    // Copy data from specified object
    void copyData(const CIFLocalMolecule &object);
    // Add Atom to Molecule
    void addAtom(Atom i) override;
    // Update local atom pointers from main vector
    void updateAtoms(AtomVector &mainAtoms, int offset) override;

    public:
    // Set Species that this Molecule represents
    void setSpecies(const Species *sp) override;
    // Set coordinates and local unit cell index of the specified atom
    void setAtom(int index, const Vec3<double> &r, int unitCellIndex);
    // Return local atoms
    std::vector<Atom> &localAtoms();
    const std::vector<Atom> &localAtoms() const;
    // Return local unit cell indices for the atoms
    const std::vector<int> &unitCellIndices() const;
};

// CIF Repeated Molecular Species
class CIFMolecularSpecies
{
    public:
    CIFMolecularSpecies();

    private:
    // Species parent for molecule instances
    std::shared_ptr<Species> species_;
    // Molecule instances
    std::vector<CIFLocalMolecule> instances_;

    public:
    // Return species parent for molecule instances
    std::shared_ptr<Species> &species();
    const std::shared_ptr<Species> &species() const;
    // Return molecule instances
    const std::vector<CIFLocalMolecule> &instances() const;
    std::vector<CIFLocalMolecule> &instances();
    // Append supplied instances to our vector
    void appendInstances(const std::vector<CIFLocalMolecule> &newInstances);
    // Return coordinates for all instances as a vector of vectors
    std::vector<std::vector<Vec3<double>>> allInstanceCoordinates() const;
};
