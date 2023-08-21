// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class AtomShakeModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;

    void SetUp() override
    {
        ASSERT_NO_THROW(systemTest.setUp("dissolve/input/atomShake-water.txt"));
        ASSERT_TRUE(systemTest.dissolve().iterate(100));
    }
};

TEST_F(AtomShakeModuleTest, Water)
{
    // Check the geometry of the molecule
    auto *box = systemTest.coreData().configuration(0)->box();
    auto &atoms = systemTest.coreData().configuration(0)->atoms();
    EXPECT_NEAR(box->minimumDistance(atoms[0].r(), atoms[1].r()), 1.012, 1.0e-4);
    EXPECT_NEAR(box->minimumDistance(atoms[0].r(), atoms[2].r()), 1.012, 1.0e-4);
    EXPECT_NEAR(box->angleInDegrees(atoms[1].r(), atoms[0].r(), atoms[2].r()), 113.24, 1.7e-2);
}

} // namespace UnitTest
