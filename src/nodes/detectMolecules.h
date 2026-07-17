// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "neta/neta.h"
#include "classes/structure.h"
#include "nodes/node.h"
#include <map>
#include <vector>

class Species;

// DetectMolecules Node
class DetectMoleculesNode : public Node
{
    public:
    DetectMoleculesNode(Graph *parentGraph);
    ~DetectMoleculesNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // Input structure
    Structure inputStructure_;
    // Output structures
    std::vector<Structure> detectedStructures_;

    /*
     * Processing
     */
    private:
    // Duplicate specified input structure atoms and their bonds, returning a new structure (including the unit cell)
    Structure duplicateAtomsAndBonds(const std::vector<int> &inputStructureAtomIndices) const;
    // Get all fragments in the structure
    std::map<int, std::vector<std::vector<int>>> getFragments() const;
    // Get coordinates of specified atoms of the input structure
    std::vector<Vector3> getAtomCoordinates(const std::vector<int> &inputStructureAtomIndices) const;
    // Determine best NETA definition for supplied fragment atoms
    NETADefinition bestNETADefinition(const std::vector<int> &fragmentAtoms) const;
    // Use the supplied NETA definition on the provided fragment, returning the first match
    NETAMatchedGroup matchFragment(const NETADefinition &neta, const std::vector<int> &fragmentAtoms) const;

    protected:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
