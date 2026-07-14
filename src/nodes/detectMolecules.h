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
    using FragmentIndices = std::vector<int>;
    using NETAFragmentVector = std::vector<std::pair<std::optional<NETADefinition>, FragmentIndices>>;
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
    // Lambda to determine if fragment is large enough to skip NETA creation
    std::function<bool(int)> largeFragment_{[this](int size) { return size * 2 > inputStructure_.nAtoms(); }};

    /*
     * Helpers
     */
    private:
    // Copy atom and bond information from one structure to another
    Structure &copyStructureAtomsAndBonds(Structure &target, const std::vector<int> fragmentIndices) const;
    // Get fragment atoms from either a single set of fragment indices, or in its overloaded form, a vector of fragments
    std::vector<const StructureAtom *> getFragmentAtoms(const std::vector<int> &fragmentIndices) const;
    std::vector<const StructureAtom *> getFragmentAtoms(const NETAFragmentVector &fragmentIndices) const;
    // Find all molecular fragments
    std::map<int, NETAFragmentVector> findMolecularFragments() const;
    // Determine best NETA definition for index atoms within a fragment
    NETADefinition bestNETADefintion(const std::vector<int> &fragmentAtoms) const;

    /*
     * Processing
     */
    protected:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
