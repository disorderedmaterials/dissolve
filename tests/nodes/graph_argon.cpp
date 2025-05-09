// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/atomicSpecies.h"
#include "nodes/configuration.h"
#include "nodes/dissolve.h"
#include "nodes/insert.h"
#include "nodes/number.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class GraphArgonTest : public ::testing::Test
{
    public:
    GraphArgonTest() : dissolve_(coreData_), root_(dissolve_) {}

    // Create a graph for testing
    void createGraph()
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
        arNode_ =
            dynamic_cast<AtomicSpeciesNode *>(root_.addNode(std::make_unique<AtomicSpeciesNode>(&root_, Elements::Ar), "Ar"));
        configurationNode_ = dynamic_cast<ConfigurationNode *>(root_.createNode("Configuration", "Bulk"));
        insertNode_ = dynamic_cast<InsertNode *>(root_.createNode("Insert", "Insert"));

        ASSERT_TRUE(arNode_);
        ASSERT_TRUE(configurationNode_);
        ASSERT_TRUE(insertNode_);

        ASSERT_TRUE(root_.addEdge({"Ar", "Species", "Insert", "Species"}));
        ASSERT_TRUE(root_.addEdge({"Bulk", "Configuration", "Insert", "Configuration"}));
    }

    protected:
    // We need a CoreData and Dissolve definition to properly instantiate DissolveGraph at present.
    CoreData coreData_;
    Dissolve dissolve_;
    DissolveGraph root_;
    AtomicSpeciesNode *arNode_{nullptr};
    ConfigurationNode *configurationNode_{nullptr};
    InsertNode *insertNode_{nullptr};
};

TEST_F(GraphArgonTest, Simulation)
{
    createGraph();

    ASSERT_EQ(insertNode_->run(), NodeConstants::ProcessResult::Success);
    auto *cfg = insertNode_->getOutputValue<Configuration *>("Configuration");

    // Check Configuration contents
    ASSERT_TRUE(cfg);
    EXPECT_EQ(cfg->nMolecules(), insertNode_->getInputValue<Number>("Population").asInteger());
};

} // namespace UnitTest
