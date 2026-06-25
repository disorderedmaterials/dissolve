// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/localMolecule.h"
#include "classes/molecule.h"
#include "classes/species.h"
#include "classes/structure.h"
#include "data/elements.h"
#include "math/vector3.h"
#include "nodes/node.h"
#include <algorithm>
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
     * Helpers
     */
    private:
    // Copy atom and bond information from one structure to another
    static Structure &copyStructureAtomsAndBonds(const Structure &source, Structure &target,
                                                 const std::vector<int> fragmentIndices);
    // Find molecular fragments
    std::vector<std::vector<int>> findMolecularFragments(const Structure &structure) const;

    /*
     * Processing
     */
    private:
    void registerDynamicOutputs() override;

    protected:
    // Run main processing
    NodeConstants::ProcessResult process() override;

    /*
     * Getters
     */
    public:
    // Output structures
    const std::vector<Structure> &detectedStructures() const;
};
