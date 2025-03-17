// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class MoleculeTorsionModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

TEST_F(MoleculeTorsionModuleTest, Benzene)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/moleculeTorsion-benzene.txt"));
    ASSERT_TRUE(systemTest.iterateRestart(80));

    EXPECT_TRUE(systemTest.checkData1D(
        "MoleculeTorsion01//NormalisedHistogram",
        {"dlpoly/benzene181/benzene181.01-03-05-07.tors.norm", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 1.0e-3));
}

} // namespace UnitTest
