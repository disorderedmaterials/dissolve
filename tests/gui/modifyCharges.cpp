// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/species.h"
#include "gui/models/modifyChargesModel.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define NO_DISPLAY false

namespace UnitTest
{
TEST(ModifyChargesModelTest, Scale)
{
    Species benzene;
    ASSERT_NO_THROW(benzene.load("species/benzene.toml"));

    ModifyChargesModel model;

    // Test initial scale value and response to value update by user
    auto initialValue = model.scaleValue();
    ASSERT_EQ(initialValue, 1.0);

    model.updateScaleValue(2.0);

    auto updatedValue = model.scaleValue();
    ASSERT_EQ(updatedValue, 2.0);

    // Set charges equal for simplicity
    for (auto &atom : benzene.atoms())
        atom.setQ(1.0);

    // Test "Scale"
    model.setScaleType(ModifyChargesModel::Scale);
    ASSERT_EQ(model.getScaleType(), ModifyChargesModel::Scale);

    EXPECT_TRUE(model.scale(&benzene, NO_DISPLAY));
    for (auto &atom : benzene.atoms())
        ASSERT_EQ(atom.q(), 2);

    // Test "ScaleTo" (returns false if input is zero, else we test the sum of the charges)
    model.setScaleType(ModifyChargesModel::ScaleTo);
    ASSERT_EQ(model.getScaleType(), ModifyChargesModel::ScaleTo);

    model.updateScaleValue(0.0);
    ASSERT_FALSE(model.scale(&benzene, NO_DISPLAY));

    model.updateScaleValue(10.0);
    EXPECT_TRUE(model.scale(&benzene, NO_DISPLAY));
    auto sum = 0.0;
    for (auto &atom : benzene.atoms())
        sum += atom.q();
    ASSERT_EQ(sum, 10);
}

TEST(ModifyChargesModelTest, Smooth)
{
    Species benzene;
    ASSERT_NO_THROW(benzene.load("species/benzene.toml"));

    ModifyChargesModel model;

    // Test initial smooth value and response to value update by user
    auto initialValue = model.smoothValue();
    ASSERT_EQ(initialValue, 0.0);

    model.updateSmoothValue(2.0);

    auto updatedValue = model.smoothValue();
    ASSERT_EQ(updatedValue, 2.0);

    // Set charges equal for simplicity
    for (auto &atom : benzene.atoms())
        atom.setQ(1.0);

    // Target smooth value to 20
    model.updateSmoothValue(20.0);
    model.smooth(&benzene);

    auto sum = 0.0;
    for (auto &atom : benzene.atoms())
        sum += atom.q();

    ASSERT_EQ(sum, 20.0);
}

TEST(ModifyChargesModelTest, ReduceSigFig)
{
    Species benzene;
    ASSERT_NO_THROW(benzene.load("species/benzene.toml"));

    ModifyChargesModel model;

    // Test initial sig fig value and response to value update by user
    auto initialValue = model.sigFigValue();
    ASSERT_EQ(initialValue, 3.0);

    model.updateSigFigValue(2.0);

    auto updatedValue = model.sigFigValue();
    ASSERT_EQ(updatedValue, 2.0);

    // Set charges equal for simplicity
    for (auto &atom : benzene.atoms())
        atom.setQ(1.235);

    model.reduceSignificantFigures(&benzene);

    // Set charges equal for simplicity
    for (auto &atom : benzene.atoms())
        ASSERT_EQ(atom.q(), 1.24);
}
} // namespace UnitTest
