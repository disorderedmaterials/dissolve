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
    // Best NETA definitions
    std::map<std::unique_ptr<NETADefinition>, std::vector<StructureAtom *>> bestNETADefinitions_;

    /*
     * Helpers
     */
    private:
    // Determine the best NETA definition for the supplied species
    std::tuple<NETADefinition, std::vector<StructureAtom *>> bestNETADefinition(const Structure &structure);
    // Get instances for the supplied species from the cleaned unit cell
    std::vector<std::vector<Vector3>> getInstances(const Structure &referenceStructure, std::vector<bool> &atomMask,
                                                   const NETADefinition &neta,
                                                   const std::vector<StructureAtom *> &referenceRootAtoms);
    // Recursively check NETA description matches between the supplied atoms
    std::map<const StructureAtom *, const StructureAtom *>
    matchAtom(const StructureAtom *referenceAtom, const StructureAtom *instanceAtom,
              const std::map<const StructureAtom *, NETADefinition> &refNETA,
              const std::map<const StructureAtom *, const StructureAtom *> &map);

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
    // Output structures
    const std::vector<Structure> &detectedStructures() const;
};
