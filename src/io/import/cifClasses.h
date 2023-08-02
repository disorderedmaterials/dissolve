// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "data/elements.h"
#include "neta/neta.h"
#include "templates/optionalRef.h"
#include "templates/vector3.h"
#include <algorithm>
#include <vector>

// Forward declarations
class Box;
class CIFImport;
class CoreData;
class Configuration;
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

class CIFSpecies
{

    public:
    typedef struct CIFMolecularSpecies
    {
        Species *species;
        std::string netaString;
        std::vector<std::vector<int>> instances;
        std::vector<std::vector<Vec3<double>>> coordinates;
    } CIFMolecularSpecies;

    public:
    CIFSpecies(CIFImport &cifImporter, CoreData &coreData);

    private:
    CIFImport &cifImporter_;
    CoreData &coreData_;
    Species *structuralSpecies_;
    Configuration *structuralConfiguration_;
    Species *cleanedSpecies_;
    Configuration *cleanedConfiguration_;
    std::vector<CIFMolecularSpecies *> molecularSpecies_;
    Configuration *molecularConfiguration_;
    Species *supercellSpecies_;
    Configuration *supercellConfiguration_;

    /*
     * Creation
     */
    public:
    // Create a structural species
    bool createStructuralSpecies(double tolerance, bool calculateBonding, bool preventMetallicBonding);
    // Create a cleaned structural species
    bool createCleanedSpecies(bool removeAtomsOfSingleMoiety, bool removeWaterMoleculesOfSingleMoiety,
                              std::optional<NETADefinition> moietyNETA = std::nullopt,
                              std::optional<bool> removeEntireFragment = std::nullopt);
    // Create molecular species
    bool createMolecularSpecies();
    // Create configuration that composes molecular species
    bool createMolecularConfiguration(OptionalReferenceWrapper<CoreData> coreData);
    // Create supercell species
    bool createSupercellSpecies(Vec3<int> repeat, bool calculateBonding, bool preventMetallicBonding);

    /*
     * Helpers
     */
    private:
    // Apply CIF bonding to a given species
    void applyCIFBonding(Species *sp, bool preventMetallicBonding);
    // Determine a unique NETA definition corresponding to a given species
    std::optional<NETADefinition> uniqueNETADefinition(Species *sp);
    // Determine instances of a NETA definition in a given species
    std::vector<std::vector<int>> instances(Species *sp, NETADefinition neta);
    // Determine coordinates of instances in a given species
    std::vector<std::vector<Vec3<double>>> coordinates(Species *sp, std::vector<std::vector<int>> instances);
    // 'Fix' the geometry of a given species
    void fixGeometry(Species *sp, const Box *box);

    /*
     * Retrieval
     */
    public:
    // Structural
    Species *structuralSpecies();
    Configuration *structuralConfiguration();
    // Cleaned
    Species *cleanedSpecies();
    Configuration *cleanedConfiguration();
    // Molecular
    std::vector<Species *> molecularSpecies();
    Configuration *molecularConfiguration();
    // Supercell
    Species *supercellSpecies();
    Configuration *supercellConfiguration();
};
