// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/scaleChargesDialogModel.h"
#include "main/dissolve.h"
#include <QTableView>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define NO_DISPLAY false;

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

    model.setOption(ScaleChargesDialogModel::Scale);
    ASSERT_EQ(model.getOption(), ScaleChargesDialogModel::Scale);

    model.setOption(ScaleChargesDialogModel::ScaleTo);
    ASSERT_EQ(model.getOption(), ScaleChargesDialogModel::ScaleTo);

    // Current scale value is 2.0, option is set to "ScaleTo"
    model.scaleCharges(species, NO_DISPLAY);

    // Still "ScaleTo" here, but now using a factor of 0.0
    model.updateValue(0);
    model.scaleCharges(species, NO_DISPLAY);
    for (auto &atom : species->atoms())
        ASSERT_EQ(atom.charge(), 2);

    // Still "ScaleTo" here, but now using a factor of 5.0
    model.updateValue(5);
    model.scaleCharges(species, NO_DISPLAY);
    for (auto &atom : species->atoms())
        ASSERT_EQ(atom.charge(), 5);

    // Only "Scale" here
    model.setOption(ScaleChargesDialogModel::Scale);
    model.scaleCharges(species, NO_DISPLAY);
    for (auto &atom : species->atoms())
        ASSERT_EQ(atom.charge(), 25);
}
} // namespace UnitTest
