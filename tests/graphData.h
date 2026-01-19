// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "main/dissolve.h"
#include "nodes/atomicSpecies.h"
#include "nodes/configuration.h"
#include "nodes/dissolve.h"
#include "nodes/insert.h"
#include <gtest/gtest.h>

namespace UnitTest
{
// Basic object setup for any Graph-based test
class GraphTestData
{
    public:
    GraphTestData() : dissolve(coreData), graphRoot(dissolve) { Node::echo_ = true; }
    CoreData coreData;
    Dissolve dissolve;
    DissolveGraph graphRoot;
};

// Create an Argon graph in the supplied root node
inline void createArgonGraph(Graph *root, int population = 1000)
{
    /*
     *    Configuration (Bulk)
     *    ------------------
     *    -   Configuration-o ---+
     *    -                |      \        Insert (Insert)
     *    -----------------/       \       ------------------
     *                              +---- o-Configuration   |
     *    AtomicSpecies (Ar)      +------ o-Species         |
     *    ------------------     /         -----------------/
     *    -         Species-o --+
     *    -                |
     *    -----------------/
     */

    // Create nodes
    auto arNode = root->addNode(std::make_unique<AtomicSpeciesNode>(root, Elements::Ar), "Ar");
    ASSERT_TRUE(arNode);
    auto configurationNode = root->createNode("Configuration", "Bulk");
    ASSERT_TRUE(configurationNode);
    auto insertNode = root->createNode("Insert", "Insert");
    ASSERT_TRUE(insertNode);

    // Create edges
    ASSERT_TRUE(root->addEdge({"Ar", "Species", "Insert", "Species"}));
    ASSERT_TRUE(root->addEdge({"Bulk", "Configuration", "Insert", "Configuration"}));

    // Set configuration contents
    ASSERT_TRUE(insertNode->setInput<Number>("Population", population));
    ASSERT_TRUE(insertNode->setInput<Number>("Density", 0.0213));
    ASSERT_TRUE(insertNode->setOption<Units::DensityUnits>("DensityUnits", Units::DensityUnits::AtomsPerAngstromUnits));
}

} // namespace UnitTest
