// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "classes/coreData.h"
#include "data/spaceGroups.h"
#include "io/import/cifClasses.h"
#include "math/matrix4.h"
#include "neta/neta.h"
#include "templates/flags.h"
#include <map>

// Forward Declarations
class Box;
class Configuration;
class Species;

// CIF Handler
class CIFHandler
{
    public:
    CIFHandler();
    ~CIFHandler() = default;

    private:
    // Reference to CoreData
    CoreData coreData_;

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
    private:
    Species *structuralSpecies_;
    Configuration *structuralConfiguration_;
    Species *cleanedSpecies_;
    Configuration *cleanedConfiguration_;
    std::vector<CIFMolecularSpecies> molecularSpecies_;
    Configuration *molecularConfiguration_;
    Species *supercellSpecies_;
    Configuration *supercellConfiguration_;
    Species *partitionedSpecies_;
    Configuration *partitionedConfiguration_;

    /*
     * Creation
     */

    public:
    // CIF Species Update Flags
    enum UpdateFlags
    {
        CleanMoietyRemoveAtomics,  /* Remove atoms of single moiety */
        CleanMoietyRemoveWater,    /* Remove water molecules of single moiety  */
        CleanMoietyRemoveNETA,     /* Remove single atoms by NETA definition  */
        CleanRemoveBoundFragments, /* Remove entire fragments when using NETA definition  */
        CalculateBonding,          /* Calculate bonding */
        PreventMetallicBonding,    /* Prevent metallic bonding */
        CreateSupermolecule
    };

    public:
    bool update(double tolerance = 0.1, Vec3<int> supercellRepeat = {1, 1, 1},
                std::optional<NETADefinition> moietyNETA = std::nullopt, Flags<UpdateFlags> flags = {});
    std::pair<std::vector<Species *>, Configuration *> finalise(CoreData &coreData) const;
    void reset();
    bool isValid() const;

    private:
    // Create a structural species
    bool createStructuralSpecies(double tolerance, Flags<UpdateFlags> flags = {});
    // Create a cleaned structural species
    bool createCleanedSpecies(Flags<UpdateFlags> flags = {}, std::optional<NETADefinition> moietyNETA = std::nullopt);
    // Create molecular species
    bool createMolecularSpecies();
    // Create supercell species
    bool createSupercellSpecies(Vec3<int> repeat, Flags<UpdateFlags> flags = {});
    // Create partitioned species
    bool createPartitionedSpecies(Flags<UpdateFlags> flags = {});

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
    const std::vector<CIFMolecularSpecies> &molecularSpecies() const;
    Configuration *molecularConfiguration();
    // Supercell
    Species *supercellSpecies();
    Configuration *supercellConfiguration();
    // Partitioned
    Species *partitionedSpecies();
    Configuration *partitionedConfiguration();
};
