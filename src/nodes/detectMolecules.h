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
    using FragmentVector = std::vector<std::vector<int>>;
    using AtomCollection = std::variant<std::set<const AtomBase *>, std::vector<const StructureAtom *>>;

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
    // Add fragment molecular instance
    static void addInstance(std::vector<Vector3> &targetInstance, const AtomCollection &instanceFragmentAtoms);
    // Get fragment atoms from either a single set of fragment indices, or in its overloaded form, a vector of fragments
    static std::vector<const StructureAtom *> getFragmentAtoms(const Structure &structure,
                                                               const std::vector<int> &fragmentIndices);
    static std::vector<const StructureAtom *> getFragmentAtoms(const Structure &structure,
                                                               const FragmentVector &fragmentIndices);
    // Find all molecular fragments
    static std::map<int, FragmentVector> findMolecularFragments(const Structure &structure);

    /*
     * Processing
     */
    protected:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
