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
    ASSERT_TRUE(dissolve.updatePairPotentials());

    PairPotentialModel pairs(dissolve.pairPotentials());
    ASSERT_EQ(dissolve.nPairPotentials(), 3);

    // Test Pairs
    EXPECT_EQ(pairs.columnCount(), 6);
    EXPECT_EQ(pairs.rowCount(), 3);

    EXPECT_EQ(pairs.data(pairs.index(0, PairPotentialModel::Columns::NameIColumn)).toString().toStdString(), "CA");
    EXPECT_EQ(pairs.data(pairs.index(0, PairPotentialModel::Columns::NameJColumn)).toString().toStdString(), "CA");
    EXPECT_DOUBLE_EQ(pairs.data(pairs.index(0, PairPotentialModel::Columns::ChargeIColumn)).toDouble(), -0.115);
    EXPECT_DOUBLE_EQ(pairs.data(pairs.index(0, PairPotentialModel::Columns::ChargeJColumn)).toDouble(), -0.115);
    EXPECT_EQ(pairs.data(pairs.index(0, PairPotentialModel::Columns::ShortRangeFormColumn)).toString().toStdString(),
              "LennardJones126");
    EXPECT_EQ(pairs.data(pairs.index(0, PairPotentialModel::Columns::ShortRangeParametersColumn)).toString().toStdString(),
              "epsilon=0.29288 sigma=3.55");

    EXPECT_EQ(pairs.data(pairs.index(1, PairPotentialModel::Columns::NameIColumn)).toString().toStdString(), "CA");
    EXPECT_EQ(pairs.data(pairs.index(1, PairPotentialModel::Columns::NameJColumn)).toString().toStdString(), "HA");
    EXPECT_DOUBLE_EQ(pairs.data(pairs.index(1, PairPotentialModel::Columns::ChargeIColumn)).toDouble(), -0.115);
    EXPECT_DOUBLE_EQ(pairs.data(pairs.index(1, PairPotentialModel::Columns::ChargeJColumn)).toDouble(), 0.115);
    EXPECT_EQ(pairs.data(pairs.index(1, PairPotentialModel::Columns::ShortRangeFormColumn)).toString().toStdString(),
              "LennardJones126");

    EXPECT_EQ(pairs.data(pairs.index(2, PairPotentialModel::Columns::NameIColumn)).toString().toStdString(), "HA");
    EXPECT_EQ(pairs.data(pairs.index(2, PairPotentialModel::Columns::NameJColumn)).toString().toStdString(), "HA");
    EXPECT_DOUBLE_EQ(pairs.data(pairs.index(2, PairPotentialModel::Columns::ChargeIColumn)).toDouble(), 0.115);
    EXPECT_DOUBLE_EQ(pairs.data(pairs.index(2, PairPotentialModel::Columns::ChargeJColumn)).toDouble(), 0.115);
    EXPECT_EQ(pairs.data(pairs.index(2, PairPotentialModel::Columns::ShortRangeFormColumn)).toString().toStdString(),
              "LennardJones126");
    EXPECT_EQ(pairs.data(pairs.index(2, PairPotentialModel::Columns::ShortRangeParametersColumn)).toString().toStdString(),
              "epsilon=0.12552 sigma=2.42");

    // Try to set immutable data
    EXPECT_FALSE(pairs.setData(pairs.index(0, PairPotentialModel::Columns::NameIColumn), "XX"));
    EXPECT_FALSE(pairs.setData(pairs.index(1, PairPotentialModel::Columns::NameJColumn), "XX"));
    EXPECT_FALSE(pairs.setData(pairs.index(2, PairPotentialModel::Columns::ChargeIColumn), 1.0));
    EXPECT_FALSE(pairs.setData(pairs.index(2, PairPotentialModel::Columns::ChargeJColumn), -0.5));

    // Set data - model is not set to editable yet, so these should also fail
    EXPECT_FALSE(pairs.setData(pairs.index(0, PairPotentialModel::Columns::ShortRangeFormColumn),
                               QString::fromStdString(Functions1D::forms().keyword(Functions1D::GaussianPotential))));
    EXPECT_EQ(pairs.data(pairs.index(0, PairPotentialModel::Columns::ShortRangeFormColumn)).toString().toStdString(),
              "LennardJones126");
    EXPECT_FALSE(pairs.setData(pairs.index(0, PairPotentialModel::Columns::ShortRangeParametersColumn), "A=4 fwhm=1.0 x0=2.5"));
    EXPECT_EQ(pairs.data(pairs.index(0, PairPotentialModel::Columns::ShortRangeParametersColumn)).toString().toStdString(),
              "epsilon=0.29288 sigma=3.55");

    // Set model to be editable and repeat
    pairs.setEditable(true);
    EXPECT_TRUE(pairs.setData(pairs.index(0, PairPotentialModel::Columns::ShortRangeFormColumn),
                              QString::fromStdString(Functions1D::forms().keyword(Functions1D::GaussianPotential))));
    EXPECT_EQ(pairs.data(pairs.index(0, PairPotentialModel::Columns::ShortRangeFormColumn)).toString().toStdString(),
              Functions1D::forms().keyword(Functions1D::GaussianPotential));
    EXPECT_TRUE(pairs.setData(pairs.index(0, PairPotentialModel::Columns::ShortRangeParametersColumn), "A=4 fwhm=1.0 x0=2.5"));
    EXPECT_THAT(pairs.data(pairs.index(0, PairPotentialModel::Columns::ShortRangeParametersColumn)).toString().toStdString(),
                testing::AnyOf(testing::Eq("A=4 fwhm=1 x0=2.5"), testing::Eq("A=4.0 fwhm=1.0 x0=2.5")));
}
} // namespace UnitTest
