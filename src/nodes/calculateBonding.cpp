// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/calculateBonding.h"
#include "classes/cellArray.h"
#include "data/atomicRadii.h"
#include "main/dissolve.h"
#include "templates/parallelDefs.h"

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

    CellArray cells;
    cells.generate(box, 6.0);

    // Populate cells
    for (auto &atom : structure.atoms())
        cells.cell(atom->r())->addAtom(atom.get());

    // Look at two indices and check to see if it would be a vaild
    // bond.  If so, return a list with that bond, otherwise an empty
    // list.  From an information theory standpoint, a list of at most
    // one element is identical to a std::optional, but the lists can
    // be trivially combined during the reduce part of
    // transform_reduce
    auto validBond = [&structure, &box, tolerance, preventMetallic,
                      clearBefore](const CellNeighbourPair idx) -> std::set<std::tuple<StructureAtom *, StructureAtom *>>
    {
        auto cellI = idx.cell;
        auto cellJ = idx.neighbour;

        std::set<std::tuple<StructureAtom *, StructureAtom *>> result;

        bool sameCell = cellI.index() == cellJ.index();

        for (auto i : cellI.atoms() | castView<StructureAtom *>())
            for (auto j : cellJ.atoms() | castView<StructureAtom *>())
            {
                // Don't bond atoms to themselves
                if (sameCell && i->index() == j->index())
                    continue;

                // Get StructureAtom 'i' and its radius
                auto radiusI = AtomicRadii::radius(i->Z());
                // Get StructureAtom 'j'

                // If the two atoms are both metal ions and prevent metallic bonds = true, continue
                if (preventMetallic && Elements::isMetallic(i->Z()) && Elements::isMetallic(j->Z()))
                    continue;

                // Calculate distance between atoms
                auto r = box.minimumDistance(j->r(), i->r());

                // Compare distance to sum of atomic radii (multiplied by tolerance factor)
                if (r > (radiusI + AtomicRadii::radius(j->Z())) * tolerance)
                    continue;

                result.insert({i, j});
            }

        return result;
    };

    // Combine two lists of bonds into a single list
    auto joinBonds = [](auto a, auto b)
    {
        auto ab = a;
        ab.merge(b);
        return ab;
    };

    // Create an empty vector of the correct shape
    std::set<std::tuple<StructureAtom *, StructureAtom *>> empty;
    // In parallel, construct the list of the bonds that need to be added
    auto pairs = cells.getCellNeighbourPairsWithSelf();
    auto results = std::transform_reduce(ParallelPolicies::par_unseq, pairs.begin(), pairs.end(), empty, joinBonds, validBond);

    // Add the bonds serially
    for (auto [i, j] : results)
        structure.addBond(i, j);
}
