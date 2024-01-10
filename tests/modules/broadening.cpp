// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/function1D.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
// Water Molecule
class BroadeningModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;

    void SetUp() override { ASSERT_NO_THROW(systemTest.setUp("dissolve/input/broadening-argon.txt")); }
};

TEST_F(BroadeningModuleTest, Dep1Indep2)
{
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    EXPECT_TRUE(
        systemTest.checkData1D("NeutronSQ01//WeightedSQ//Total", {"epsr25/argon10000/argon_dep0.1indep0.2.EPSR.u01",
                                                                  Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}));
}

TEST_F(BroadeningModuleTest, Dep2Indep1)
{
    // Set QBroadening
    auto *sqModule = Module::find(systemTest.coreData(),  "SQ01");
    ASSERT_TRUE(sqModule);
    ASSERT_NO_THROW(
        sqModule->keywords().set("QBroadening", Functions::Function1DWrapper(Functions::Function1D::GaussianC2, {0.1, 0.2})));

    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    EXPECT_TRUE(
        systemTest.checkData1D("NeutronSQ01//WeightedSQ//Total", {"epsr25/argon10000/argon_dep0.2indep0.1.EPSR.u01",
                                                                  Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}));
}

TEST_F(BroadeningModuleTest, Dep1)
{
    // Set QBroadening
    auto *sqModule = Module::find(systemTest.coreData(), "SQ01");
    ASSERT_TRUE(sqModule);
    ASSERT_NO_THROW(sqModule->keywords().set(
        "QBroadening", Functions::Function1DWrapper(Functions::Function1D::OmegaDependentGaussian, {0.1})));

    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    EXPECT_TRUE(
        systemTest.checkData1D("NeutronSQ01//WeightedSQ//Total", {"epsr25/argon10000/argon_dep0.1.EPSR.u01",
                                                                  Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}));
}

TEST_F(BroadeningModuleTest, Dep2)
{
    // Set QBroadening
    auto *sqModule = Module::find(systemTest.coreData(), "SQ01");
    ASSERT_TRUE(sqModule);
    ASSERT_NO_THROW(sqModule->keywords().set(
        "QBroadening", Functions::Function1DWrapper(Functions::Function1D::OmegaDependentGaussian, {0.2})));

    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    EXPECT_TRUE(
        systemTest.checkData1D("NeutronSQ01//WeightedSQ//Total", {"epsr25/argon10000/argon_dep0.2.EPSR.u01",
                                                                  Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}));
}

TEST_F(BroadeningModuleTest, Indep1)
{
    // Set QBroadening
    auto *sqModule = Module::find(systemTest.coreData(), "SQ01");
    ASSERT_TRUE(sqModule);
    ASSERT_NO_THROW(
        sqModule->keywords().set("QBroadening", Functions::Function1DWrapper(Functions::Function1D::Gaussian, {0.1})));

    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    EXPECT_TRUE(
        systemTest.checkData1D("NeutronSQ01//WeightedSQ//Total", {"epsr25/argon10000/argon_indep0.1.EPSR.u01",
                                                                  Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}));
}

TEST_F(BroadeningModuleTest, Indep2)
{
    // Set QBroadening
    auto *sqModule = Module::find(systemTest.coreData(), "SQ01");
    ASSERT_TRUE(sqModule);
    ASSERT_NO_THROW(
        sqModule->keywords().set("QBroadening", Functions::Function1DWrapper(Functions::Function1D::Gaussian, {0.2})));

    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    EXPECT_TRUE(
        systemTest.checkData1D("NeutronSQ01//WeightedSQ//Total", {"epsr25/argon10000/argon_indep0.2.EPSR.u01",
                                                                  Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}));
}

} // namespace UnitTest
