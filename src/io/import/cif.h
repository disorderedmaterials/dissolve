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
    bool parse(std::string_view filename, CIFTags &tags) const;

    public:
    // Return whether the specified file parses correctly
    bool validFile(std::string_view filename) const;
    // Read CIF data from specified file
    bool read(std::string_view filename);
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

    private:
    // Flags controlling behaviour
    Flags<UpdateFlags> flags_;
    // Bonding tolerance, if calculating bonding rather than using CIF definitions
    double bondingTolerance_{0.1};
    // NETA for moiety removal, if specified
    NETADefinition moietyRemovalNETA_;
    // Supercell repeat
    Vec3<int> supercellRepeat_{1, 1, 1};
    // Basic unit cell
    Species *unitCellSpecies_;
    Configuration *unitCellConfiguration_;
    // Cleaned unit cell
    Species *cleanedUnitCellSpecies_;
    Configuration *cleanedUnitCellConfiguration_;
    // Molecular definition of unit cell (if possible)
    std::vector<CIFMolecularSpecies> molecularUnitCellSpecies_;
    Configuration *molecularUnitCellConfiguration_;
    // Supercell
    Species *supercellSpecies_;
    Configuration *supercellConfiguration_;
    // Final supercell, partitioned accordingly
    Species *partitionedSpecies_;
    Configuration *partitionedConfiguration_;

    private:
    // Create basic unit cell
    bool createBasicUnitCel();
    // Create the cleaned unit cell
    bool createCleanedUnitCell();
    // Try to detect molecules in the cell contents
    bool detectMolecules();
    // Create supercell species
    bool createSupercell();
    // Create partitioned cell
    bool createPartitionedCell();

    public:
    // Reset all objects
    void resetSpeciesAndConfigurations();
    // Return current flags (for editing)
    Flags<UpdateFlags> &flags();
    // Set bonding tolerance
    void setBondingTolerance(double tol);
    // Set NETA for moiety removal
    bool setMoietyRemovalNETA(std::string_view netaDefinition);
    // Set supercell repeat
    void setSupercellRepeat(const Vec3<int> &repeat);
    // Recreate the data
    bool generate(std::optional<Flags<UpdateFlags>> newFlags = {});
    // Finalise, returning the required species and resulting configuration
    std::pair<std::vector<Species *>, Configuration *> finalise(CoreData &coreData) const;
    // Return whether the generated data is valid
    bool isValid() const;
    // Structural
    Species *structuralUnitCellSpecies();
    Configuration *structuralUnitCellConfiguration();
    // Cleaned
    Species *cleanedUnitCellSpecies();
    Configuration *cleanedUnitCellConfiguration();
    // Molecular
    const std::vector<CIFMolecularSpecies> &molecularSpecies() const;
    Configuration *molecularConfiguration();
    // Supercell
    Species *supercellSpecies();
    Configuration *supercellConfiguration();
    // Partitioned
    Species *partitionedSpecies();
    Configuration *partitionedConfiguration();

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
};
