// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "io/import/coordinates.h"
#include "io/import/data1D.h"
#include "nodes/atomicMC/atomicMC.h"
#include "nodes/configuration.h"
#include "nodes/data1DImport.h"
#include "nodes/dissolve.h"
#include "nodes/importConfigurationCoordinates.h"
#include "nodes/insert.h"
#include "nodes/iterableGraph.h"
#include "nodes/species.h"
#include "nodes/numberNode.h"
#include "tests/testData.h"
#include <array>
#include <gtest/gtest.h>
#include <map>
#include <math/vector3.h>
#include <tuple>
#include <variant>
#include <vector>

namespace UnitTest
{

TEST(AtomShakeTest, Water)
{
    // We need a CoreData and Dissolve definition to properly instantiate DissolveGraph at present.
    CoreData coreData;
    Dissolve dissolve{coreData};
    DissolveGraph root{dissolve};

    coreData.clear();

    // Create iterable graph containing an AtomicMCNode
    auto iterator = dynamic_cast<IterableGraph *>(root.createNode("Iterator", "Iterator"));

    auto atomicMCNode = dynamic_cast<AtomicMCNode *>(iterator->createNode("AtomicMC", "AtomicMC"));
    ASSERT_TRUE(atomicMCNode->setOption<Number>("ShakesPerAtom", 10));

    // Water species
    auto species = createWater(&root);

    // Water configuration
    auto configurationNode = dynamic_cast<ConfigurationNode *>(root.createNode("Configuration", "Bulk"));
    ASSERT_TRUE(configurationNode);

    auto insertNode = dynamic_cast<InsertNode *>(root.createNode("Insert", "Insert"));
    ASSERT_TRUE(insertNode);
    ASSERT_TRUE(insertNode->setInput<Number>("Population", 1));
    ASSERT_TRUE(insertNode->setInput<Number>("Density", 0.1));

    // Create number node to modify temperature
    auto temperatureValueNode = dynamic_cast<NumberNode *>(root.createNode("Number", "Temperature"));
    ASSERT_TRUE(temperatureValueNode);
    ASSERT_TRUE(temperatureValueNode->setOption<Number>("A", 0));

    // Set density units
    ASSERT_TRUE(insertNode->setOption<Units::DensityUnits>("DensityUnits", Units::DensityUnits::AtomsPerAngstromUnits));

    // Set connections
    EXPECT_TRUE(root.addEdge({"Water", "Species", "Insert", "Species"}));
    EXPECT_TRUE(root.addEdge({"Bulk", "Configuration", "Insert", "Configuration"}));
    EXPECT_TRUE(root.addEdge({"Insert", "Configuration", "Iterator", "Configuration"}));
    EXPECT_TRUE(iterator->addEdge({"Inputs", "Configuration", "AtomicMC", "Configuration"}));
    // TODO: Output parameter name cannot be the same as loopback (although loopback param CAN match input).
    // May need to enforce this.
    EXPECT_TRUE(iterator->addEdge({"AtomicMC", "Configuration", "Outputs", "OutputConfiguration"}));
    EXPECT_TRUE(iterator->addEdge({"AtomicMC", "Configuration", "LoopBacks", "Configuration"}));

    // Run a single iteration at 300K
    ASSERT_TRUE(iterator->setOption<Number>("N", 1));
    ASSERT_TRUE(iterator->run() == NodeConstants::ProcessResult::Success);

    // Zero K optimization for 100 iterations
    EXPECT_TRUE(root.addEdge({"Temperature", "A", "Iterator", "A"}));
    EXPECT_TRUE(iterator->addEdge({"Inputs", "A", "AtomicMC", "Temperature"}));
    ASSERT_TRUE(iterator->setOption<Number>("N", 100));
    ASSERT_TRUE(iterator->run() == NodeConstants::ProcessResult::Success);

    // Check the geometry of the molecule
    auto cfg = configurationNode->getOutputValue<Configuration *>("Configuration");
    auto *box = cfg->box();
    auto &atoms = cfg->atoms();
    auto rMin01 = box->minimumDistance(atoms[0].r(), atoms[1].r());
    auto rMin02 = box->minimumDistance(atoms[0].r(), atoms[2].r());
    auto angle102 = box->angleInDegrees(atoms[1].r(), atoms[0].r(), atoms[2].r());
    EXPECT_NEAR(rMin01, 1.0, 1.0e-4);
    EXPECT_NEAR(rMin02, 1.0, 1.0e-4);
    EXPECT_NEAR(angle102, 113.24, 1.7e-3);
}

} // namespace UnitTest