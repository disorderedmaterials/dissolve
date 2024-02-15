// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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
    // Temporary CoreData
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
    // CIF Species Output Flags
    enum OutputFlags
    {
        OutputConfiguration,    /* Output a Configuration */
        OutputMolecularSpecies, /* Partitioning - output molecular species */
        OutputFramework,        /* Partitioning - output a framework species */
        OutputSupermolecule     /* Partitioning - output a supermolecule */
    };

    private:
    // Tolerance for removal of overlapping atoms
    double overlapTolerance_{0.1};
    // Whether to use CIF bonding definitions
    bool useCIFBondingDefinitions_{false};
    // Bonding tolerance, if calculating bonding rather than using CIF definitions
    double bondingTolerance_{1.1};
    // Whether to prevent metallic bonding
    bool preventMetallicBonds_{false};
    // Whether to remove free atomic moieties in clean-up
    bool removeAtomics_{false};
    // Whether to remove water and coordinated oxygen atoms in clean-up
    bool removeWaterAndCoordinateOxygens_{false};
    // Whether to remove by NETA definition in clean-up
    bool removeNETA_{false};
    // Whether to expand NETA matches to fragments when removing in clean-up
    bool removeNETAByFragment_{false};
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

    private:
    // Create basic unit cell
    bool createBasicUnitCell();
    // Create the cleaned unit cell
    bool createCleanedUnitCell();
    // Try to detect molecules in the cell contents
    bool detectMolecules();
    // Create supercell species
    bool createSupercell();

    public:
    // Reset all objects
    void resetSpeciesAndConfigurations();
    // Set overlap tolerance
    void setOverlapTolerance(double tol);
    // Set whether to use CIF bonding definitions
    void setUseCIFBondingDefinitions(bool b);
    // Set bonding tolerance
    void setBondingTolerance(double tol);
    // Set whether to prevent metallic bonding
    void setPreventMetallicBonds(bool b);
    // Set whether to remove free atomic moieties in clean-up
    void setRemoveAtomics(bool b);
    // Set whether to remove water and coordinated oxygen atoms in clean-up
    void setRemoveWaterAndCoordinateOxygens(bool b);
    // Set whether to remove by NETA definition in clean-up
    void setRemoveNETA(bool b, bool byFragment);
    // Set NETA for moiety removal
    bool setMoietyRemovalNETA(std::string_view netaDefinition);
    // Set supercell repeat
    void setSupercellRepeat(const Vec3<int> &repeat);
    // Recreate the data
    bool generate();
    // Finalise, returning the required species and resulting configuration
    std::pair<std::vector<const Species *>, Configuration *> finalise(CoreData &coreData,
                                                                      const Flags<OutputFlags> &flags = {}) const;
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

    /*
     * Helpers
     */
    private:
    // Apply CIF bonding to a given species
    void applyCIFBonding(Species *sp, bool preventMetallicBonding);
    // Determine a unique NETA definition corresponding to a given species
    std::optional<NETADefinition> uniqueNETADefinition(Species *sp);
    // Get instances of species molecules from the supplied NETA definition
    std::vector<CIFLocalMolecule> getSpeciesInstances(Species *moleculeSpecies, const NETADefinition &neta);
    // 'Fix' the geometry of a given species
    void fixGeometry(Species *sp, const Box *box);
};
