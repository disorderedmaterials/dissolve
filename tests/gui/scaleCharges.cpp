// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/scaleChargesDialogModel.h"
#include "main/dissolve.h"
#include <QTableView>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define NO_DISPLAY false

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

    // Test initial scale value and response to value update by user
    auto initialValue = model.value();
    ASSERT_EQ(initialValue, 1.0);

    model.updateValue(2.0);

    auto updatedValue = model.value();
    ASSERT_EQ(updatedValue, 2.0);

    // Set charges equal for simplicity
    for (auto &atom : species->atoms())
        atom.setCharge(1.0);

    // Test "Scale"
    model.setOption(ScaleChargesDialogModel::Scale);
    ASSERT_EQ(model.getOption(), ScaleChargesDialogModel::Scale);

    auto success = model.scaleCharges(species, NO_DISPLAY);
    for (auto &atom : species->atoms())
        ASSERT_EQ(atom.charge(), 2);

    // Test "ScaleTo" (returns false if input is zero, else we test the sum of the charges)
    model.setOption(ScaleChargesDialogModel::ScaleTo);
    ASSERT_EQ(model.getOption(), ScaleChargesDialogModel::ScaleTo);

    model.updateValue(0.0);
    ASSERT_FALSE(model.scaleCharges(species, NO_DISPLAY));

    model.updateValue(10.0);
    success = model.scaleCharges(species, NO_DISPLAY);
    auto sum = 0.0;
    for (auto &atom : species->atoms())
        sum += atom.charge();
    ASSERT_EQ(sum, 10);
}
} // namespace UnitTest
