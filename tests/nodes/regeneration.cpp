// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/configuration.h"
#include "nodes/dissolve.h"
#include "nodes/insert.h"
#include "nodes/species.h"
#include "nodes/temperature.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{

class RegenerationTest : public ::testing::Test
{
    public:
    RegenerationTest() : dissolve_(coreData_), root_(dissolve_) {}

    void createGraph()
    {
        // Configuration
        configurationNode_ = dynamic_cast<ConfigurationNode *>(root_.createNode("Configuration", "Configuration"));
        ASSERT_TRUE(configurationNode_->setOption<Number>("Temperature", 300));

        // Water species
        createWater(&root_);
        speciesNode_ = dynamic_cast<SpeciesNode *>(root_.node("Water"));

        // Insert
        insertNode_ = dynamic_cast<InsertNode *>(root_.createNode("Insert", "Insert"));
        ASSERT_TRUE(insertNode_->setOption<Number>("Population", 100));
        ASSERT_TRUE(insertNode_->setOption<Number>("Density", 0.1));
        ASSERT_TRUE(insertNode_->setOption<Units::DensityUnits>("DensityUnits", Units::DensityUnits::AtomsPerAngstromUnits));

        // Temperature
        temperatureNode_ = dynamic_cast<TemperatureNode *>(root_.createNode("Temperature", "Temperature"));
        ASSERT_TRUE(temperatureNode_->setOption<Number>("Temperature", 0));

        // Edges
        root_.addEdge({"Configuration", "Configuration", "Insert", "Configuration"});
        root_.addEdge({"Water", "Species", "Insert", "Species"});
        root_.addEdge({"Insert", "Configuration", "Temperature", "Configuration"});
    }

    protected:
    // We need a CoreData and Dissolve definition to properly instantiate DissolveGraph at present.
    CoreData coreData_;
    Dissolve dissolve_;
    DissolveGraph root_;

    // Graph nodes
    ConfigurationNode *configurationNode_{nullptr};
    SpeciesNode *speciesNode_{nullptr};
    InsertNode *insertNode_{nullptr};
    TemperatureNode *temperatureNode_{nullptr};

    static const Configuration *configurationPtr(Node *node) { return node->getOutputValue<Configuration *>("Configuration"); }
};

TEST_F(RegenerationTest, ModifyConfigurationTemperature)
{
    createGraph();

    // Configuration initialised at 300K, then modified to 0K
    EXPECT_EQ(configurationPtr(static_cast<Node *>(configurationNode_))->temperature(), 300);
    EXPECT_EQ(temperatureNode_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(configurationPtr(static_cast<Node *>(insertNode_))->temperature(), 0);

    // Check versioning
    EXPECT_EQ(configurationNode_->versionIndex(), 0);
    EXPECT_EQ(insertNode_->versionIndex(), 0);
    EXPECT_EQ(temperatureNode_->versionIndex(), 0);

    // Set temperature at source configuration to 100K and re-run graph
    configurationNode_->setOption<Number>("Temperature", 100);
    EXPECT_EQ(configurationNode_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(configurationPtr(static_cast<Node *>(configurationNode_))->temperature(), 100);
    EXPECT_EQ(temperatureNode_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(configurationPtr(static_cast<Node *>(insertNode_))->temperature(), 0);

    // Check versioning - everything runs again except for the generator
    EXPECT_EQ(configurationNode_->versionIndex(), 1);
    EXPECT_EQ(insertNode_->versionIndex(), 0);
    EXPECT_EQ(temperatureNode_->versionIndex(), 1);
}

TEST_F(RegenerationTest, ModifySpeciesPopulation)
{
    createGraph();

    // Configuration initialised at 300K, then modified to 0K
    EXPECT_EQ(configurationPtr(static_cast<Node *>(configurationNode_))->temperature(), 300);
    EXPECT_EQ(temperatureNode_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(configurationPtr(static_cast<Node *>(insertNode_))->temperature(), 0);

    // Check versioning
    EXPECT_EQ(configurationNode_->versionIndex(), 0);
    EXPECT_EQ(insertNode_->versionIndex(), 0);
    EXPECT_EQ(temperatureNode_->versionIndex(), 0);

    // Set temperature at source configuration to 100K and re-run graph
    insertNode_->setOption<Number>("Population", insertNode_->findOption("Population")->get<Number>().asInteger() + 1);
    EXPECT_EQ(temperatureNode_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(configurationPtr(static_cast<Node *>(insertNode_))->temperature(), 0);

    // Check versioning - everything runs again except for the configuration
    EXPECT_EQ(configurationNode_->versionIndex(), 0);
    EXPECT_EQ(insertNode_->versionIndex(), 1);
    EXPECT_EQ(temperatureNode_->versionIndex(), 1);
}

TEST_F(RegenerationTest, ChangeConfiguration)
{
    createGraph();

    // Configuration initialised at 300K, then modified to 0K
    EXPECT_EQ(configurationPtr(static_cast<Node *>(configurationNode_))->temperature(), 300);
    EXPECT_EQ(temperatureNode_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(configurationPtr(static_cast<Node *>(insertNode_))->temperature(), 0);

    // Check versioning
    EXPECT_EQ(configurationNode_->versionIndex(), 0);
    EXPECT_EQ(insertNode_->versionIndex(), 0);
    EXPECT_EQ(temperatureNode_->versionIndex(), 0);

    // Remove source configuration and re-run graph - generator should run as usual
    root_.removeEdge(root_.findEdge({"Configuration", "Configuration", "Insert", "Configuration"}));
    auto newConfigurationNode = dynamic_cast<ConfigurationNode *>(root_.createNode("Configuration", "Configuration'"));
    root_.addEdge({"Configuration'", "Configuration", "Insert", "Configuration"});
    ASSERT_TRUE(newConfigurationNode->setOption<Number>("Temperature", 250));
    EXPECT_TRUE(insertNode_->versionIndex(), -1);
    EXPECT_EQ(temperatureNode_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(configurationPtr(static_cast<Node *>(insertNode_))->temperature(), 0);

    // Check versioning - everything runs except for the old configuration (which is now disconnected)
    EXPECT_EQ(configurationNode_->versionIndex(), 0);
    EXPECT_EQ(newConfigurationNode->versionIndex(), 0);
    EXPECT_EQ(insertNode_->versionIndex(), 0);
    EXPECT_EQ(temperatureNode_->versionIndex(), 1);
}

} // namespace UnitTest