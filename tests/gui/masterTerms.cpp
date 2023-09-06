// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/masterAngleModel.h"
#include "gui/models/masterBondModel.h"
#include "gui/models/masterTorsionModel.h"
#include "main/dissolve.h"
#include <QTableView>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

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
    std::vector<std::shared_ptr<MasterBond>> masterBonds;
    auto &b1 = masterBonds.emplace_back(std::make_shared<MasterBond>("CA-CA"));
    b1->setInteractionFormAndParameters(BondFunctions::Form::Harmonic, "k=3924.590   eq=1.400");
    auto &b2 = masterBonds.emplace_back(std::make_shared<MasterBond>("CA-HA"));
    b2->setInteractionFormAndParameters(BondFunctions::Form::Harmonic, "k=3071.060   eq=1.080");

    MasterBondModel model;
    model.setSourceData(masterBonds);

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
        EXPECT_EQ(model.data(model.index(0, 2), role).toString().toStdString(), "k=3924.59 eq=1.4");
        EXPECT_EQ(model.data(model.index(1, 2), role).toString().toStdString(), "k=3071.06 eq=1.08");
    }

    // Mutate table contents
    EXPECT_TRUE(model.setData(model.index(0, 0), "TEST"));
    EXPECT_EQ(model.data(model.index(0, 0)).toString().toStdString(), "TEST");

    EXPECT_TRUE(model.setData(model.index(0, 1), "EPSR"));
    EXPECT_EQ(model.data(model.index(0, 1)).toString().toStdString(), "EPSR");

    EXPECT_FALSE(model.setData(model.index(0, 1), "NON_EXISTING_ENUM"));
    EXPECT_EQ(model.data(model.index(0, 1)).toString().toStdString(), "EPSR");

    EXPECT_FALSE(model.setData(model.index(0, 2), "3 4 1"));
    EXPECT_FALSE(model.setData(model.index(0, 2), "k=3 eq=4"));
    EXPECT_TRUE(model.setData(model.index(0, 2), "3 4"));
    EXPECT_THAT(model.data(model.index(0, 2)).toString().toStdString(),
                testing::AnyOf(testing::Eq("C/2=3.0 eq=4.0"), testing::Eq("C/2=3 eq=4")));
}

TEST_F(MasterTermsTableModelTest, MasterAngles)
{
    std::vector<std::shared_ptr<MasterAngle>> masterAngles;
    auto &a1 = masterAngles.emplace_back(std::make_shared<MasterAngle>("CA-CA-CA"));
    a1->setInteractionFormAndParameters(AngleFunctions::Form::Harmonic, "k=527.184   eq=120.000");
    auto &a2 = masterAngles.emplace_back(std::make_shared<MasterAngle>("CA-CA-HA"));
    a2->setInteractionFormAndParameters(AngleFunctions::Form::Harmonic, "k=292.880   eq=120.000");

    MasterAngleModel model;
    model.setSourceData(masterAngles);

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
        EXPECT_THAT(model.data(model.index(0, 2), role).toString().toStdString(),
                    testing::AnyOf(testing::Eq("k=527.184 eq=120.0"), testing::Eq("k=527.184 eq=120")));
        EXPECT_THAT(model.data(model.index(1, 2), role).toString().toStdString(),
                    testing::AnyOf(testing::Eq("k=292.88 eq=120.0"), testing::Eq("k=292.88 eq=120")));
    }

    // Mutate table contents
    EXPECT_TRUE(model.setData(model.index(0, 0), "TEST"));
    EXPECT_EQ(model.data(model.index(0, 0)).toString().toStdString(), "TEST");

    EXPECT_TRUE(model.setData(model.index(0, 1), "Cos"));
    EXPECT_EQ(model.data(model.index(0, 1)).toString().toStdString(), "Cos");

    EXPECT_FALSE(model.setData(model.index(0, 1), "NON_EXISTING_ENUM"));
    EXPECT_EQ(model.data(model.index(0, 1)).toString().toStdString(), "Cos");

    EXPECT_FALSE(model.setData(model.index(0, 2), "3 4"));
    EXPECT_TRUE(model.setData(model.index(0, 2), "3 4 1 1"));
    EXPECT_THAT(model.data(model.index(0, 2)).toString().toStdString(),
                testing::AnyOf(testing::Eq("k=3.0 n=4.0 eq=1.0 s=1.0"), testing::Eq("k=3 n=4 eq=1 s=1")));
}

TEST_F(MasterTermsTableModelTest, MasterTorsions)
{
    std::vector<std::shared_ptr<MasterTorsion>> masterTorsions;
    auto &t1 = masterTorsions.emplace_back(std::make_shared<MasterTorsion>("CA-CA-CA-CA"));
    t1->setInteractionFormAndParameters(TorsionFunctions::Form::Cos3, "0.000    30.334     0.000");
    auto &t2 = masterTorsions.emplace_back(std::make_shared<MasterTorsion>("CA-CA-CA-HA"));
    t2->setInteractionFormAndParameters(TorsionFunctions::Form::Cos3, "0.000    30.334     1.000");
    auto &t3 = masterTorsions.emplace_back(std::make_shared<MasterTorsion>("HA-CA-CA-HA"));
    t3->setInteractionFormAndParameters(TorsionFunctions::Form::Cos3, "0.000    30.334     2.000");

    MasterTorsionModel model;
    model.setSourceData(masterTorsions);

    // test table structure
    EXPECT_EQ(model.columnCount(), 5);
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
        EXPECT_THAT(model.data(model.index(0, 2), role).toString().toStdString(),
                    testing::AnyOf(testing::Eq("k1=0.0 k2=30.334 k3=0.0"), testing::Eq("k1=0 k2=30.334 k3=0")));
        EXPECT_THAT(model.data(model.index(1, 2), role).toString().toStdString(),
                    testing::AnyOf(testing::Eq("k1=0.0 k2=30.334 k3=1.0"), testing::Eq("k1=0 k2=30.334 k3=1")));
        EXPECT_THAT(model.data(model.index(2, 2), role).toString().toStdString(),
                    testing::AnyOf(testing::Eq("k1=0.0 k2=30.334 k3=2.0"), testing::Eq("k1=0 k2=30.334 k3=2")));
        EXPECT_DOUBLE_EQ(model.data(model.index(0, 3), role).toDouble(), 0.5);
        EXPECT_DOUBLE_EQ(model.data(model.index(0, 4), role).toDouble(), 0.5);
    }

    // Mutate table contents
    EXPECT_TRUE(model.setData(model.index(0, 0), "TEST"));
    EXPECT_EQ(model.data(model.index(0, 0)).toString().toStdString(), "TEST");

    EXPECT_TRUE(model.setData(model.index(0, 1), "Cos3C"));
    EXPECT_EQ(model.data(model.index(0, 1)).toString().toStdString(), "Cos3C");

    EXPECT_FALSE(model.setData(model.index(0, 1), "NON_EXISTING_ENUM"));
    EXPECT_EQ(model.data(model.index(0, 1)).toString().toStdString(), "Cos3C");

    EXPECT_FALSE(model.setData(model.index(0, 2), "3 4 5"));
    EXPECT_TRUE(model.setData(model.index(0, 2), "1.0 3 4 5"));
    EXPECT_THAT(model.data(model.index(0, 2)).toString().toStdString(),
                testing::AnyOf(testing::Eq("k0=1.0 k1=3.0 k2=4.0 k3=5.0"), testing::Eq("k0=1 k1=3 k2=4 k3=5")));

    EXPECT_TRUE(model.setData(model.index(0, 3), 0.83));
    EXPECT_DOUBLE_EQ(model.data(model.index(0, 3)).toDouble(), 0.83);
    EXPECT_TRUE(model.setData(model.index(0, 4), 0.0));
    EXPECT_DOUBLE_EQ(model.data(model.index(0, 4)).toDouble(), 0.0);
}

} // namespace UnitTest
