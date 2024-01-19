// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/species.h"
#include "gui/models/pairPotentialModel.h"
#include "main/dissolve.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>

Q_DECLARE_METATYPE(Sears91::Isotope);

namespace UnitTest
{

class ForcefieldTabTest : public ::testing::Test
{
    public:
    ForcefieldTabTest() = default;

    protected:
    void SetUp() override {}
};

std::vector<Qt::ItemDataRole> roles = {Qt::DisplayRole, Qt::EditRole};

TEST_F(ForcefieldTabTest, PairPotentials)
{

    CoreData coreData;
    Dissolve dissolve(coreData);

    dissolve.clear();
    ASSERT_TRUE(dissolve.loadInput("dissolve/input/full-benzene.txt"));
    ASSERT_TRUE(dissolve.regeneratePairPotentials());

    PairPotentialModel pairs(dissolve.pairPotentials());
    ASSERT_EQ(dissolve.nPairPotentials(), 3);

    // Test Pairss
    EXPECT_EQ(pairs.columnCount(), 6);
    EXPECT_EQ(pairs.rowCount(), 3);

    EXPECT_EQ(pairs.data(pairs.index(0, 0)).toString().toStdString(), "CA");
    EXPECT_EQ(pairs.data(pairs.index(0, 1)).toString().toStdString(), "CA");
    EXPECT_EQ(pairs.data(pairs.index(0, 2)).toString().toStdString(), "LJGeometric");
    EXPECT_DOUBLE_EQ(pairs.data(pairs.index(0, 3)).toDouble(), -0.115);
    EXPECT_DOUBLE_EQ(pairs.data(pairs.index(0, 4)).toDouble(), -0.115);
    EXPECT_EQ(pairs.data(pairs.index(0, 5)).toString().toStdString(), "epsilon=0.29288 sigma=3.55");

    EXPECT_EQ(pairs.data(pairs.index(2, 0)).toString().toStdString(), "HA");
    EXPECT_EQ(pairs.data(pairs.index(2, 1)).toString().toStdString(), "HA");
    EXPECT_EQ(pairs.data(pairs.index(2, 2)).toString().toStdString(), "LJGeometric");
    EXPECT_DOUBLE_EQ(pairs.data(pairs.index(2, 3)).toDouble(), 0.115);
    EXPECT_DOUBLE_EQ(pairs.data(pairs.index(2, 4)).toDouble(), 0.115);
    EXPECT_EQ(pairs.data(pairs.index(2, 5)).toString().toStdString(), "epsilon=0.12552 sigma=2.42");

    // Currently, we do not support the user changing the values, but
    // I've added the option for the future
    EXPECT_FALSE(pairs.setData(pairs.index(0, 0), "HA"));
    EXPECT_FALSE(pairs.setData(pairs.index(0, 1), "HA"));
    EXPECT_FALSE(pairs.setData(pairs.index(0, 2), "LJ"));
    EXPECT_TRUE(pairs.setData(pairs.index(0, 3), 2));
    EXPECT_DOUBLE_EQ(pairs.data(pairs.index(0, 3)).toDouble(), 2);
    EXPECT_TRUE(pairs.setData(pairs.index(0, 4), -3));
    EXPECT_DOUBLE_EQ(pairs.data(pairs.index(0, 4)).toDouble(), -3);
    EXPECT_TRUE(pairs.setData(pairs.index(0, 5), "4.0, -5.0"));
    EXPECT_THAT(pairs.data(pairs.index(0, 5)).toString().toStdString(),
                testing::AnyOf(testing::Eq("epsilon=4.0 sigma=-5.0"), testing::Eq("epsilon=4 sigma=-5")));
}
} // namespace UnitTest
