// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/calculateBonding.h"
#include "data/atomicRadii.h"
#include "main/dissolve.h"
#include "templates/algorithms.h"
#include "templates/parallelDefs.h"
#include <mutex>

CalculateBondingNode::CalculateBondingNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Structure", "Input structure", inputStructure_);

    // Outputs
    addOutput("Structure", "Output structure", outputStructure_);

    // Options
    addOption("Tolerance", "Bonding tolerance factor", tolerance_);
    addOption("PreventMetallicBonds", "Whether to prevent metallic bonding", preventMetallicBonds_);
    addOption("Clear", "Whether to clear bonds before recalculating", clear_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view CalculateBondingNode::type() const { return "CalculateBonding"; }

// Return short summary of the node's purpose
std::string_view CalculateBondingNode::summary() const { return "Calculate bonding within a structure"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult CalculateBondingNode::process()
{
    outputStructure_.clear();
    outputStructure_ = inputStructure_;

    calculate(outputStructure_, tolerance_.asDouble(), clear_, preventMetallicBonds_);

    return NodeConstants::ProcessResult::Success;
}

// Calculate bonding in the specified structure
void CalculateBondingNode::calculate(Structure &structure, double tolerance, bool clearBefore, bool preventMetallic)
{
    if (clearBefore)
        structure.clearBonds();

    auto box = structure.box();
    auto nAtoms = PairIterator(structure.nAtoms());
    std::mutex structureMutex;
    dissolve::for_each_pair(
        ParallelPolicies::par_unseq, structure.nAtoms(),
        [&structureMutex, &structure, &box, tolerance, preventMetallic, clearBefore](auto indexI, auto indexJ)
        {
            if (indexI == indexJ)
                return;
            auto i = structure.atom(indexI);
            // Get StructureAtom 'i' and its radius
            auto radiusI = AtomicRadii::radius(i->Z());
            // Get StructureAtom 'j'
            auto j = structure.atom(indexJ);

            // If the two atoms are both metal ions and prevent metallic bonds = true, continue
            if (preventMetallic && Elements::isMetallic(i->Z()) && Elements::isMetallic(j->Z()))
                return;

            // Calculate distance between atoms
            auto r = box.minimumDistance(j->r(), i->r());

            // Compare distance to sum of atomic radii (multiplied by tolerance factor)
            if (r > (radiusI + AtomicRadii::radius(j->Z())) * tolerance)
                return;

            std::lock_guard<std::mutex> guard(structureMutex);
            // If the two atoms are already bound, continue
            if (structure.getBond(i, j))
                return;

            structure.addBond(i, j);
        });
}
