// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class QSpeciesModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

TEST_F(QSpeciesModuleTest, Simple)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/qSpecies-simple.txt"));
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    EXPECT_TRUE(systemTest.checkData1D(
        "Q//Process1D//QSpecies", {"dissolve/input/qSpecies-simple.dat", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 3},
        1.0e-8));
}

} // namespace UnitTest
