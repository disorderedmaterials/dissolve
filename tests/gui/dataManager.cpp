// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/dataManagerSimulationModel.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{

class DataManagerTest : public ::testing::Test
{
    public:
    DataManagerTest() = default;

    protected:
    void SetUp() override {}
};

TEST_F(DataManagerTest, DataManager)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    dissolve.clear();
    ASSERT_TRUE(dissolve.loadInput("dissolve/input/full-benzene.txt"));

    DataManagerSimulationModel sim(dissolve, dissolve.processingModuleData());

    // Expect this file to load properly
    ASSERT_TRUE(dissolve.loadRestart("dissolve/restart/full-benzene.txt.restart.test"));

    EXPECT_EQ(sim.columnCount(), 3);
    // There are 38 processing data items in the restart file
    EXPECT_EQ(sim.rowCount(), 38);
    EXPECT_EQ(sim.data(sim.index(0, 0)).toString().toStdString(), "5050//FullWeights");
    EXPECT_EQ(sim.data(sim.index(0, 1)).toString().toStdString(), "NeutronWeights");
    EXPECT_EQ(sim.data(sim.index(0, 2)).toInt(), 1);
    EXPECT_EQ(sim.data(sim.index(1, 0)).toString().toStdString(), "5050//RepresentativeTotalGR");
}

} // namespace UnitTest
