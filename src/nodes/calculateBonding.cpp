// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/calculateBonding.h"
#include "data/atomicRadii.h"

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
    auto nAtoms = structure.nAtoms();
    for (auto indexI = 0; indexI < nAtoms - 1; ++indexI)
    {
        // Get StructureAtom 'i' and its radius
        auto i = structure.atom(indexI);
        auto radiusI = AtomicRadii::radius(i->Z());
        for (auto indexJ = indexI + 1; indexJ < nAtoms; ++indexJ)
        {
            // Get StructureAtom 'j'
            auto j = structure.atom(indexJ);

            // If the two atoms are both metal ions and prevent metallic bonds = true, continue
            if (preventMetallic && Elements::isMetallic(i->Z()) && Elements::isMetallic(j->Z()))
                continue;

            // If the two atoms are already bound, continue
            if (structure.getBond(i, j))
                continue;

            // Calculate distance between atoms
            auto r = box.minimumDistance(j->r(), i->r());

            // Compare distance to sum of atomic radii (multiplied by tolerance factor)
            if (r <= (radiusI + AtomicRadii::radius(j->Z())) * tolerance)
                structure.addBond(i, j);
        }
    }
}
