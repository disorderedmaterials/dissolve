// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "data/spacegroups.h"
#include "io/import/cifclasses.h"
#include "math/matrix4.h"
#include <map>

// CIF Importer
class CIFImport
{
    public:
    CIFImport() = default;
    ~CIFImport() = default;

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
    // Space group information
    SpaceGroup spaceGroup_;
    // Atom assemblies
    std::vector<CIFAssembly> assemblies_;
    // Bond information
    std::vector<CIFBondingPair> bondingPairs_;

    public:
    // Set space group from index
    void setSpaceGroupFromIndex(int index);
    // Return space group information
    const SpaceGroup &spaceGroup() const;
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
};
