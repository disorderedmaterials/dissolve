// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/modifyChargesModel.h"
#include "main/dissolve.h"
#include <QTableView>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <QDebug>

#define NO_DISPLAY false

namespace UnitTest
{

class ModifyChargesModelTest : public ::testing::Test
{
    public:
    ModifyChargesModelTest() = default;

    protected:
    void SetUp() override{};
};

TEST_F(ModifyChargesModelTest, Scale)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    dissolve.clear();
    ASSERT_TRUE(dissolve.loadInput("dissolve/input/full-benzene.txt"));
    auto species = dissolve.coreData().findSpecies("benzene");
    ASSERT_NE(species, nullptr);

    ModifyChargesModel model;

    // Test initial scale value and response to value update by user
    auto initialValue = model.scaleValue();
    ASSERT_EQ(initialValue, 1.0);

    model.updateScaleValue(2.0);

    auto updatedValue = model.scaleValue();
    ASSERT_EQ(updatedValue, 2.0);

    // Set charges equal for simplicity
    for (auto &atom : species->atoms())
        atom.setCharge(1.0);

    // Test "Scale"
    model.setScaleType(ModifyChargesModel::Scale);
    ASSERT_EQ(model.getScaleType(), ModifyChargesModel::Scale);

    auto success = model.scale(species, NO_DISPLAY);
    for (auto &atom : species->atoms())
        ASSERT_EQ(atom.charge(), 2);

    // Test "ScaleTo" (returns false if input is zero, else we test the sum of the charges)
    model.setScaleType(ModifyChargesModel::ScaleTo);
    ASSERT_EQ(model.getScaleType(), ModifyChargesModel::ScaleTo);

    model.updateScaleValue(0.0);
    ASSERT_FALSE(model.scale(species, NO_DISPLAY));

    model.updateScaleValue(10.0);
    success = model.scale(species, NO_DISPLAY);
    auto sum = 0.0;
    for (auto &atom : species->atoms())
        sum += atom.charge();
    ASSERT_EQ(sum, 10);
}

TEST_F(ModifyChargesModelTest, Smooth)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    dissolve.clear();
    ASSERT_TRUE(dissolve.loadInput("dissolve/input/full-benzene.txt"));
    auto species = dissolve.coreData().findSpecies("benzene");
    ASSERT_NE(species, nullptr);

    ModifyChargesModel model;

    // Test initial scale value and response to value update by user
    auto initialValue = model.smoothValue();
    ASSERT_EQ(initialValue, 0.0);

    model.updateSmoothValue(2.0);

    auto updatedValue = model.smoothValue();
    ASSERT_EQ(updatedValue, 2.0);

    // Set charges equal for simplicity
    for (auto &atom : species->atoms())
        atom.setCharge(1.0);

    // Target smooth value to 20
    model.updateSmoothValue(20.0);
    model.smooth(species);

    auto sum = 0.0;
    for (auto &atom : species->atoms())
        sum += atom.charge();

    ASSERT_EQ(sum, 20.0);
}

TEST_F(ModifyChargesModelTest, ReduceSigFig)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    dissolve.clear();
    ASSERT_TRUE(dissolve.loadInput("dissolve/input/full-benzene.txt"));
    auto species = dissolve.coreData().findSpecies("benzene");
    ASSERT_NE(species, nullptr);

    ModifyChargesModel model;

    // Test initial scale value and response to value update by user
    auto initialValue = model.sigFigValue();
    ASSERT_EQ(initialValue, 3.0);

    model.updateSigFigValue(2.0);

    auto updatedValue = model.sigFigValue();
    ASSERT_EQ(updatedValue, 2.0);

    // Set charges equal for simplicity
    for (auto &atom : species->atoms())
        atom.setCharge(1.235);

    model.reduceSignificantFigures(species);

    // Set charges equal for simplicity
    for (auto &atom : species->atoms())
        ASSERT_EQ(atom.charge(), 1.24);
}
} // namespace UnitTest
