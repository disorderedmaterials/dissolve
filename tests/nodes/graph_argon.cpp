// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/atomicMC/atomicMC.h"
#include "nodes/atomicSpecies.h"
#include "nodes/configuration.h"
#include "nodes/dissolve.h"
#include "nodes/energy/energy.h"
#include "nodes/gr/gr.h"
#include "nodes/insert.h"
#include "nodes/md/md.h"
#include "nodes/neutronSQ/neutronSQ.h"
#include "nodes/number.h"
#include "nodes/sq/sq.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class GraphArgonTest : public ::testing::Test
{
    public:
    GraphArgonTest() : dissolve_(coreData_), root_(dissolve_) { Node::echo_ = true; }

    // Create a graph for testing
    void createGraph(bool advanced = false)
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

        if (advanced)
        {
            atomicMCNode_ = dynamic_cast<AtomicMCNode *>(root_.createNode("AtomicMC", "AtomicMC"));
            mdNode_ = dynamic_cast<MDNode *>(root_.createNode("MD", "MD"));
            energyNode_ = dynamic_cast<EnergyNode *>(root_.createNode("Energy", "Energy"));
            grNode_ = dynamic_cast<GRNode *>(root_.createNode("GR", "GR"));
            sqNode_ = dynamic_cast<SQNode *>(root_.createNode("SQ", "SQ"));
            neutronSQNode_ = dynamic_cast<NeutronSQNode *>(root_.createNode("NeutronSQ", "NeutronSQ"));

            ASSERT_TRUE(atomicMCNode_);
            ASSERT_TRUE(mdNode_);
            ASSERT_TRUE(energyNode_);
            ASSERT_TRUE(grNode_);
            ASSERT_TRUE(sqNode_);
            ASSERT_TRUE(neutronSQNode_);

            ASSERT_TRUE(root_.addEdge({"Insert", "Configuration", "AtomicMC", "Configuration"}));
            ASSERT_TRUE(root_.addEdge({"AtomicMC", "Configuration", "MD", "Configuration"}));
            ASSERT_TRUE(root_.addEdge({"MD", "Configuration", "Energy", "Configuration"}));
            ASSERT_TRUE(root_.addEdge({"Energy", "Configuration", "GR", "Configuration"}));
            ASSERT_TRUE(root_.addEdge({"GR", "UnweightedGR", "SQ", "UnweightedGR"}));
            ASSERT_TRUE(root_.addEdge({"SQ", "UnweightedGR", "NeutronSQ", "UnweightedGR"}));
            ASSERT_TRUE(root_.addEdge({"SQ", "UnweightedSQ", "NeutronSQ", "UnweightedSQ"}));
        }
    }

    protected:
    // We need a CoreData and Dissolve definition to properly instantiate DissolveGraph at present.
    CoreData coreData_;
    Dissolve dissolve_;
    DissolveGraph root_;
    AtomicSpeciesNode *arNode_{nullptr};
    ConfigurationNode *configurationNode_{nullptr};
    InsertNode *insertNode_{nullptr};
    AtomicMCNode *atomicMCNode_{nullptr};
    MDNode *mdNode_{nullptr};
    EnergyNode *energyNode_{nullptr};
    GRNode *grNode_{nullptr};
    SQNode *sqNode_{nullptr};
    NeutronSQNode *neutronSQNode_{nullptr};
};

TEST_F(GraphArgonTest, InitSimulation)
{
    createGraph();

    ASSERT_EQ(insertNode_->run(), NodeConstants::ProcessResult::Success);
    auto *cfg = insertNode_->getOutputValue<Configuration *>("Configuration");

    // Check Configuration contents
    ASSERT_TRUE(cfg);
    EXPECT_EQ(cfg->nMolecules(), insertNode_->getInputValue<Number>("Population").asInteger());
};

TEST_F(GraphArgonTest, AdvancedSimulation)
{
    createGraph(true);
    ASSERT_EQ(neutronSQNode_->run(), NodeConstants::ProcessResult::Success);
}

} // namespace UnitTest
