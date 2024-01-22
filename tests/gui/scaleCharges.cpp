// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/scaleChargesDialogModel.h"
#include "main/dissolve.h"
#include <QTableView>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace UnitTest
{

class ScaleDialogModelTest : public ::testing::Test
{
    public:
    ScaleDialogModelTest() = default;

    protected:
    void SetUp() override{};
};

TEST_F(ScaleDialogModelTest, scale)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    dissolve.clear();
    ASSERT_TRUE(dissolve.loadInput("dissolve/input/full-benzene.txt"));
    auto species = dissolve.coreData().findSpecies("benzene");
    ASSERT_NE(species, nullptr);

    ScaleChargesDialogModel model;

    auto initialValue = model.value();
    ASSERT_EQ(initialValue, 1.0);

    model.updateValue(2.0);

    auto updatedValue = model.value();
    ASSERT_EQ(updatedValue, 2.0);

    model.setOption(Scale);
    ASSERT_EQ(model.getOption(), Scale);

    model.setOption(ScaleTo);
    ASSERT_EQ(model.getOption(), ScaleTo);

    // Current scale value is 2.0, option is set to "ScaleTo"
    ASSERT_TRUE(model.scaleCharges(species));

    // Still "ScaleTo" here, but now using a factor of 0.0
    model.updateValue(0);
    ASSERT_FALSE(model.scaleCharges(species));

    // Only "Scale" here
    model.setOption(Scale);
    ASSERT_TRUE(model.scaleCharges(species));
    for (auto &atom : species.atoms())
        ASSERT_EQ(atom.charge(), 0);
}
} // namespace UnitTest
