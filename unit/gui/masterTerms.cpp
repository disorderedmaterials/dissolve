// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors
#include "gui/models/masterTermModel.h"
#include "main/dissolve.h"
#include <QTableView>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>

namespace UnitTest
{

class MasterTermsTableModelTest : public ::testing::Test
{
    public:
    MasterTermsTableModelTest() = default;

    protected:
    void SetUp() override {}
};

std::vector<Qt::ItemDataRole> roles = {Qt::DisplayRole, Qt::EditRole};

TEST_F(MasterTermsTableModelTest, MasterBonds)
{

    CoreData coreData;
    Dissolve dissolve(coreData);

    dissolve.clear();
    dissolve.loadInput("restart/benzene.txt");

    auto &masterBonds = dissolve.coreData().masterBonds();

    MasterTermBondModel model(masterBonds);

    // test table structure
    EXPECT_EQ(model.columnCount(), 3);
    EXPECT_EQ(model.rowCount(), 2);

    // test table contents
    for (auto role : roles)
    {
        EXPECT_EQ(model.data(model.index(0, 0), role).toString().toStdString(), "CA-CA");
        EXPECT_EQ(model.data(model.index(1, 0), role).toString().toStdString(), "CA-HA");
        EXPECT_EQ(model.data(model.index(0, 1), role).toString().toStdString(), "Harmonic");
        EXPECT_EQ(model.data(model.index(1, 1), role).toString().toStdString(), "Harmonic");
        EXPECT_EQ(model.data(model.index(0, 2), role).toString().toStdString(), "3924.59, 1.4");
        EXPECT_EQ(model.data(model.index(1, 2), role).toString().toStdString(), "3071.06, 1.08");
    }

    // Mutate table contents
    EXPECT_TRUE(model.setData(model.index(0, 0), "TEST"));
    EXPECT_EQ(model.data(model.index(0, 0)).toString().toStdString(), "TEST");

    EXPECT_TRUE(model.setData(model.index(0, 1), "EPSR"));
    EXPECT_EQ(model.data(model.index(0, 1)).toString().toStdString(), "EPSR");

    EXPECT_FALSE(model.setData(model.index(0, 1), "NON_EXISTING_ENUM"));
    EXPECT_EQ(model.data(model.index(0, 1)).toString().toStdString(), "EPSR");

    EXPECT_TRUE(model.setData(model.index(0, 2), "3, 4"));
    EXPECT_THAT(model.data(model.index(0, 2)).toString().toStdString(), testing::AnyOf(testing::Eq("3.0, 4.0"), testing::Eq("3, 4")));
}
TEST_F(MasterTermsTableModelTest, MasterAngles)
{

    CoreData coreData;
    Dissolve dissolve(coreData);

    dissolve.clear();
    dissolve.loadInput("restart/benzene.txt");

    auto &masterAngles = dissolve.coreData().masterAngles();

    MasterTermAngleModel model(masterAngles);

    // test table structure
    EXPECT_EQ(model.columnCount(), 3);
    EXPECT_EQ(model.rowCount(), 2);

    // test table contents
    for (auto role : roles)
    {
        EXPECT_EQ(model.data(model.index(0, 0), role).toString().toStdString(), "CA-CA-CA");
        EXPECT_EQ(model.data(model.index(1, 0), role).toString().toStdString(), "CA-CA-HA");
        EXPECT_EQ(model.data(model.index(0, 1), role).toString().toStdString(), "Harmonic");
        EXPECT_EQ(model.data(model.index(1, 1), role).toString().toStdString(), "Harmonic");
        EXPECT_THAT(model.data(model.index(0, 2), role).toString().toStdString(), testing::AnyOf(testing::Eq("527.184, 120.0"), testing::Eq("527.184, 120")));
        EXPECT_THAT(model.data(model.index(1, 2), role).toString().toStdString(), testing::AnyOf(testing::Eq("292.88, 120.0"), testing::Eq("292.88, 120")));
    }

    // Mutate table contents
    EXPECT_TRUE(model.setData(model.index(0, 0), "TEST"));
    EXPECT_EQ(model.data(model.index(0, 0)).toString().toStdString(), "TEST");

    EXPECT_TRUE(model.setData(model.index(0, 1), "Cos"));
    EXPECT_EQ(model.data(model.index(0, 1)).toString().toStdString(), "Cos");

    EXPECT_FALSE(model.setData(model.index(0, 1), "NON_EXISTING_ENUM"));
    EXPECT_EQ(model.data(model.index(0, 1)).toString().toStdString(), "Cos");

    EXPECT_TRUE(model.setData(model.index(0, 2), "3, 4"));
    EXPECT_THAT(model.data(model.index(0, 2)).toString().toStdString(), testing::AnyOf(testing::Eq("3.0, 4.0"), testing::Eq("3, 4")));
}
TEST_F(MasterTermsTableModelTest, MasterTorsions)
{

    CoreData coreData;
    Dissolve dissolve(coreData);

    dissolve.clear();
    dissolve.loadInput("restart/benzene.txt");

    auto &masterTorsions = dissolve.coreData().masterTorsions();

    MasterTermTorsionModel model(masterTorsions);

    // test table structure
    EXPECT_EQ(model.columnCount(), 3);
    EXPECT_EQ(model.rowCount(), 3);

    // test table contents
    for (auto role : roles)
    {
        EXPECT_EQ(model.data(model.index(0, 0), role).toString().toStdString(), "CA-CA-CA-CA");
        EXPECT_EQ(model.data(model.index(1, 0), role).toString().toStdString(), "CA-CA-CA-HA");
        EXPECT_EQ(model.data(model.index(2, 0), role).toString().toStdString(), "HA-CA-CA-HA");
        EXPECT_EQ(model.data(model.index(0, 1), role).toString().toStdString(), "Cos3");
        EXPECT_EQ(model.data(model.index(1, 1), role).toString().toStdString(), "Cos3");
        EXPECT_EQ(model.data(model.index(2, 1), role).toString().toStdString(), "Cos3");
        EXPECT_THAT(model.data(model.index(0, 2), role).toString().toStdString(), testing::AnyOf(testing::Eq("0.0, 30.334, 0.0"), testing::Eq("0, 30.334, 0")));
        EXPECT_THAT(model.data(model.index(1, 2), role).toString().toStdString(), testing::AnyOf(testing::Eq("0.0, 30.334, 0.0"), testing::Eq("0, 30.334, 0")));
        EXPECT_THAT(model.data(model.index(2, 2), role).toString().toStdString(), testing::AnyOf(testing::Eq("0.0, 30.334, 0.0"), testing::Eq("0, 30.334, 0")));
    }

    // Mutate table contents
    EXPECT_TRUE(model.setData(model.index(0, 0), "TEST"));
    EXPECT_EQ(model.data(model.index(0, 0)).toString().toStdString(), "TEST");

    EXPECT_TRUE(model.setData(model.index(0, 1), "Cos3C"));
    EXPECT_EQ(model.data(model.index(0, 1)).toString().toStdString(), "Cos3C");

    EXPECT_FALSE(model.setData(model.index(0, 1), "NON_EXISTING_ENUM"));
    EXPECT_EQ(model.data(model.index(0, 1)).toString().toStdString(), "Cos3C");

    EXPECT_TRUE(model.setData(model.index(0, 2), "3, 4, 5"));
    EXPECT_THAT(model.data(model.index(0, 2)).toString().toStdString(), testing::AnyOf(testing::Eq("3.0, 4.0, 5.0"), testing::Eq("3, 4, 5")));
}

} // namespace UnitTest
