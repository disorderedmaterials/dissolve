// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "data/spaceGroups.h"
#include "io/import/cifClasses.h"
#include "math/matrix4.h"
#include <map>

// CIF Handler
class CIFHandler
{
    public:
    CIFHandler() = default;
    ~CIFHandler() = default;

    /*
     * Raw Data
     */
    public:
    // Data Types
    using CIFTags = std::map<std::string, std::vector<std::string>>;

    private:
    // Vector of enumerated data items
    CIFTags tags_;

    private:
    // Parse supplied file into the destination objects
    bool parse(std::string filename, CIFTags &tags) const;

    public:
    // Return whether the specified file parses correctly
    bool validFile(std::string filename) const;
    // Read CIF data from specified file
    bool read(std::string filename);
    // Return if the specified tag exists
    bool hasTag(std::string tag) const;
    // Return tag data string (if it exists) assuming a single datum (first in the vector)
    std::optional<std::string> getTagString(std::string tag) const;
    // Return tag data strings (if it exists)
    std::vector<std::string> getTagStrings(std::string tag) const;
    // Return tag data as double (if it exists) assuming a single datum (first in the vector)
    std::optional<double> getTagDouble(std::string tag) const;
    // Return tag data doubles (if it exists)
    std::vector<double> getTagDoubles(std::string tag) const;
    // Return tag data as integer (if it exists) assuming a single datum (first in the vector)
    std::optional<int> getTagInt(std::string tag) const;

    /*
     * Processed Data
     */
    private:
    // Space group
    SpaceGroups::SpaceGroupId spaceGroup_{SpaceGroups::NoSpaceGroup};
    // Atom assemblies
    std::vector<CIFAssembly> assemblies_;
    // Bond information
    std::vector<CIFBondingPair> bondingPairs_;

    public:
    // Set space group from index
    void setSpaceGroup(SpaceGroups::SpaceGroupId sgid);
    // Return space group
    SpaceGroups::SpaceGroupId spaceGroup() const;
    // Return cell lengths
    std::optional<Vec3<double>> getCellLengths() const;
    // Return cell angles
    std::optional<Vec3<double>> getCellAngles() const;
    // Return chemical formula
    std::string chemicalFormula() const;
    // Get (add or retrieve) named assembly
    CIFAssembly &getAssembly(std::string_view name);
    // Return atom assemblies
    std::vector<CIFAssembly> &assemblies();
    const std::vector<CIFAssembly> &assemblies() const;
    // Return whether any bond distances are defined
    bool hasBondDistances() const;
    // Return whether a bond distance is defined for the specified label pair
    std::optional<double> bondDistance(std::string_view labelI, std::string_view labelJ) const;

    /*
     * Species & Configurations
     */

    typedef struct CIFMolecularSpecies
    {
        Species *species;
        std::string netaString;
        std::vector<std::vector<int>> instances;
        std::vector<std::vector<Vec3<double>>> coordinates;
    } CIFMolecularSpecies;

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
    // CIF Species Cleaning Flags
    enum CleaningFlags
    {
        MoietyRemoveAtomics,  /* Remove atoms of single moiety */
        MoietyRemoveWater,    /* Remove water molecules of single moiety */
        MoietyRemoveNETA,     /* Remove single atoms by NETA definition */
        RemoveBoundFragments, /* Remove entire fragments when using NETA definition */
    };
    // CIF Species Bonding Flags
    enum BondingFlags
    {
        CalculateBonding,       /* Calculate bonding */
        PreventMetallicBonding, /* Prevent metallic bonding */
    };

    public:
    // Create a structural species
    bool createStructuralSpecies(CoreData &coreData, double tolerance, Flags<BondingFlags> bondingFlags = {});
    // Create a cleaned structural species
    bool createCleanedSpecies(CoreData &coreData, Flags<CleaningFlags> cleaningFlags = {},
                              std::optional<NETADefinition> moietyNETA = std::nullopt);
    // Create molecular species
    bool createMolecularSpecies(CoreData &coreData);
    // Create configuration that composes molecular species
    bool createMolecularConfiguration(CoreData &coreData);
    // Create supercell species
    bool createSupercellSpecies(CoreData &coreData, Vec3<int> repeat, Flags<BondingFlags> bondingFlags = {});

    /*
     * Helpers
     */
    private:
    // Apply CIF bonding to a given species
    void applyCIFBonding(Species *sp, bool preventMetallicBonding);
    // Determine a unique NETA definition corresponding to a given species
    std::optional<NETADefinition> uniqueNETADefinition(Species *sp);
    // Determine instances of a NETA definition in a given species
    std::vector<std::vector<int>> speciesCopies(Species *sp, NETADefinition neta);
    // Determine coordinates of instances in a given species
    std::vector<std::vector<Vec3<double>>> speciesCopiesCoordinates(Species *sp, std::vector<std::vector<int>> copies);
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
