// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/structure.h"
#include "data/spaceGroups.h"
#include "math/matrix4.h"
#include "neta/neta.h"
#include "nodes/cif/io/CIFImportVisitor.h"
#include "nodes/cif/io/cifClasses.h"
#include "nodes/node.h"
#include <map>

// ImportCIFStructure Node
class ImportCIFStructureNode : public Node
{
    public:
    ImportCIFStructureNode(Graph *parentGraph);
    ~ImportCIFStructureNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // CIF strucutre
    Structure structure_;
    // Space group ID
    SpaceGroups::SpaceGroupId spaceGroup_{SpaceGroups::SpaceGroupId::NoSpaceGroup};
    // CIF filepath
    std::string filePath_;
    // Whether to prevent metallic bonding
    bool preventMetallicBonds_{true};

    /*
     * CIF I/O
     */

    /*
     * Basic CIF Data
     */
    private:
    // Vector of enumerated data items
    CIFImportVisitor::CIFTags tags_;

    private:
    // Parse supplied file into the destination objects
    bool parse(std::string_view filename, CIFImportVisitor::CIFTags &tags) const;

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
    std::optional<Vector3> getCellLengths() const;
    // Return cell angles
    std::optional<Vector3> getCellAngles() const;
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
    private:
    // Temporary atom types used for unique atom labels
    std::vector<std::shared_ptr<AtomType>> atomLabelTypes_;
    // Tolerance for removal of overlapping atoms
    double overlapTolerance_{0.1};

    private:
    // Create structure from basic unit cell atoms and connectivity
    bool createStructure(SpaceGroups::SpaceGroupId sgid, double overlapTolerance);

    public:
    // Set overlap tolerance
    void setOverlapTolerance(double tol);

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;

    /*
     * Getters
     */
    public:
    // Return basic crystal structure
    const Structure &structure() const;
};
