// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui/models/isotopologueSetModel.h"
#include "classes/isotopologueSet.h"
#include "tests/speciesData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(IsotopologueSetModelTest, Basic)
{
    auto waterNode = loadTOMLSpecies("species/water.toml");
    ASSERT_TRUE(waterNode);
    auto &water = waterNode->species();

    auto methanolNode = loadTOMLSpecies("species/methanol.toml");
    ASSERT_TRUE(methanolNode);
    auto &methanol = methanolNode->species();

    // Create an isotopologue set
    auto h5hSet = IsotopologueSet({{water.naturalIsotopologue(), 1.0},
                                   {methanol.naturalIsotopologue(), 0.5},
                                   {methanol.findIsotopologue("OD-MethylH"), 0.5}});

    // Set up the model
    IsotopologueSetModel model;
    model.setSourceData(h5hSet);

    // Basic Data
    EXPECT_EQ(model.columnCount(), 3);
    EXPECT_EQ(model.rowCount(), 2);

    // Sub-data
    EXPECT_EQ(model.data(model.index(0, 0)).toString().toStdString(), "Water");
    EXPECT_EQ(model.rowCount(model.index(0, 0)), 1);
    EXPECT_EQ(model.data(model.index(0, 1, model.index(0, 0))).toString().toStdString(), "Natural");
    EXPECT_EQ(model.rowCount(model.index(1, 0)), 2);
    EXPECT_EQ(model.data(model.index(0, 1, model.index(1, 0))).toString().toStdString(), "Natural");
    EXPECT_EQ(model.data(model.index(1, 1, model.index(1, 0))).toString().toStdString(), "OD-MethylH");
    EXPECT_DOUBLE_EQ(model.data(model.index(0, 2, model.index(1, 0))).toDouble(), 0.5);
    EXPECT_DOUBLE_EQ(model.data(model.index(1, 2, model.index(1, 0))).toDouble(), 0.5);

    // Set
    EXPECT_TRUE(model.setData(model.index(0, 2, model.index(0, 0)), 10.0));
    EXPECT_FALSE(model.setData(model.index(0, 2, model.index(0, 0)), -1.0));
    EXPECT_TRUE(model.setData(model.index(1, 1, model.index(1, 0)), "Deuteriated"));
    EXPECT_FALSE(model.setData(model.index(1, 1, model.index(1, 0)), "Not A Real Isotopologue"));
}

} // namespace UnitTest
