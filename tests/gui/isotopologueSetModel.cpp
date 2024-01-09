// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/isotopologueSetModel.h"
#include "keywords/isotopologueSet.h"
#include "main/dissolve.h"
#include "modules/neutronSQ/neutronSQ.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(IsotopologueSetModelTest, Basic)
{
    CoreData coreData;
    Dissolve dissolve(coreData);
    ASSERT_TRUE(dissolve.loadInput("dissolve/input/correlations-waterMethanol.txt"));

    // Find the 'H5H' NeutronSQ module and grab the relevant IsotopologueSet
    auto *h5h = dynamic_cast<NeutronSQModule *>(Module::find("H5H"));
    assert(h5h);
    auto optH5hSet = h5h->keywords().get<IsotopologueSet, IsotopologueSetKeyword>("Isotopologue");
    ASSERT_TRUE(optH5hSet.has_value());
    auto &h5hSet = optH5hSet.value();

    // Set up the model
    IsotopologueSetModel model;
    model.setSourceData(h5hSet);

    // Basic Data
    EXPECT_EQ(model.columnCount(), 3);
    EXPECT_EQ(model.rowCount(), 2);

    // Sub-data
    EXPECT_EQ(model.data(model.index(0, 0)).toString().toStdString(), "Water");
    EXPECT_EQ(model.rowCount(model.index(0, 0)), 1);
    EXPECT_EQ(model.data(model.index(0, 1, model.index(0, 0))).toString().toStdString(), "Protiated");
    EXPECT_EQ(model.rowCount(model.index(1, 0)), 2);
    EXPECT_EQ(model.data(model.index(0, 1, model.index(1, 0))).toString().toStdString(), "Protiated");
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
