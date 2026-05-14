// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/calculateBonding.h"
#include "data/atomicRadii.h"

CalculateBondingNode::CalculateBondingNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addOutput<Structure>("Structure", "Input structure", inputStructure_);

    // Outputs
    addOutput<Structure>("Structure", "Output structure", outputStructure_);

    // Options
    addOption<Number>("BondingTolerance", "Bonding tolerance, if calculating bonding rather than using CIF definitions",
                      bondingTolerance_);
    addOption<bool>("PreventMetallicBonds", "Whether to prevent metallic bonding", preventMetallicBonds_);
    addOption<bool>("Clear", "Whether to clear bonds before recalculating", clear_);
}

std::string_view CalculateBondingNode::type() const { return "CalculateBonding"; }

std::string_view CalculateBondingNode::summary() const { return "Calculate bonding information for a structure"; }

// Run main processing
NodeConstants::ProcessResult CalculateBondingNode::process()
{
    outputStructure_.clear();
    outputStructure_ = inputStructure_;

    if (clear_)
        outputStructure_.clearBonds();

    double r, radiusI;
    auto box = outputStructure_.box();
    auto nAtoms = outputStructure_.nAtoms();
    for (auto indexI = 0; indexI < nAtoms - 1; ++indexI)
    {
        // Get StructureAtom 'i' and its radius
        auto i = outputStructure_.atomAt(indexI);
        radiusI = AtomicRadii::radius(i->Z());
        for (auto indexJ = indexI + 1; indexJ < nAtoms; ++indexJ)
        {
            // Get StructureAtom 'j'
            auto j = outputStructure_.atomAt(indexJ);

            // If the two atoms are both metal ions and prevent metallic bonds = true, continue
            if (preventMetallicBonds_ && Elements::isMetallic(i->Z()) && Elements::isMetallic(j->Z()))
                continue;

            // If the two atoms are already bound, continue
            if (outputStructure_.getBond(i, j))
                continue;

            // Calculate distance between atoms
            r = box ? box->minimumDistance(j->r(), i->r()) : (j->r() - i->r()).magnitude();

            // Compare distance to sum of atomic radii (multiplied by tolerance factor)
            if (r <= (radiusI + AtomicRadii::radius(j->Z())) * bondingTolerance_.asDouble())
                outputStructure_.addBond(i, j);
        }
    }

    return NodeConstants::ProcessResult::Success;
}