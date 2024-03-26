// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class PCL2019CationsForcefieldTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;

    void SetUp() override { ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/ff-pcl2019-cations.txt")); }
};

TEST_F(PCL2019CationsForcefieldTest, benzc1im)
{
    auto *sp = systemTest.coreData().findSpecies("benzc1im");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "NA");
    systemTest.checkSpeciesAtomType(sp, 1, "CR");
    systemTest.checkSpeciesAtomType(sp, 2, "NA");
    systemTest.checkSpeciesAtomType(sp, 3, "CW");
    systemTest.checkSpeciesAtomType(sp, 4, "CW");
    systemTest.checkSpeciesAtomType(sp, 5, "C1");
    systemTest.checkSpeciesAtomType(sp, 6, "HCR");
    systemTest.checkSpeciesAtomType(sp, 7, "C1T");
    systemTest.checkSpeciesAtomType(sp, 8, "HCW");
    systemTest.checkSpeciesAtomType(sp, 9, "HCW");
    systemTest.checkSpeciesAtomType(sp, 10, "H1");
    systemTest.checkSpeciesAtomType(sp, 11, "H1");
    systemTest.checkSpeciesAtomType(sp, 12, "H1");
    systemTest.checkSpeciesAtomType(sp, 13, "H1");
    systemTest.checkSpeciesAtomType(sp, 14, "H1");
    systemTest.checkSpeciesAtomType(sp, 15, "CAT");
    systemTest.checkSpeciesAtomType(sp, 16, "CAO");
    systemTest.checkSpeciesAtomType(sp, 17, "CAM");
    systemTest.checkSpeciesAtomType(sp, 18, "CAP");
    systemTest.checkSpeciesAtomType(sp, 19, "CAM");
    systemTest.checkSpeciesAtomType(sp, 20, "CAO");
    systemTest.checkSpeciesAtomType(sp, 21, "HAT");
    systemTest.checkSpeciesAtomType(sp, 22, "HAT");
    systemTest.checkSpeciesAtomType(sp, 23, "HAT");
    systemTest.checkSpeciesAtomType(sp, 24, "HAT");
    systemTest.checkSpeciesAtomType(sp, 25, "HAT");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, c12c1im)
{
    auto *sp = systemTest.coreData().findSpecies("c12c1im");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "NA");
    systemTest.checkSpeciesAtomType(sp, 1, "CR");
    systemTest.checkSpeciesAtomType(sp, 2, "NA");
    systemTest.checkSpeciesAtomType(sp, 3, "CW");
    systemTest.checkSpeciesAtomType(sp, 4, "CW");
    systemTest.checkSpeciesAtomType(sp, 5, "C1");
    systemTest.checkSpeciesAtomType(sp, 6, "HCR");
    systemTest.checkSpeciesAtomType(sp, 7, "C1");
    systemTest.checkSpeciesAtomType(sp, 8, "HCW");
    systemTest.checkSpeciesAtomType(sp, 9, "HCW");
    systemTest.checkSpeciesAtomType(sp, 10, "H1");
    systemTest.checkSpeciesAtomType(sp, 11, "H1");
    systemTest.checkSpeciesAtomType(sp, 12, "H1");
    systemTest.checkSpeciesAtomType(sp, 13, "C2");
    systemTest.checkSpeciesAtomType(sp, 14, "H1");
    systemTest.checkSpeciesAtomType(sp, 15, "H1");
    systemTest.checkSpeciesAtomType(sp, 16, "CS");
    systemTest.checkSpeciesAtomType(sp, 17, "HC");
    systemTest.checkSpeciesAtomType(sp, 18, "HC");
    systemTest.checkSpeciesAtomType(sp, 19, "CS");
    systemTest.checkSpeciesAtomType(sp, 20, "HC");
    systemTest.checkSpeciesAtomType(sp, 21, "HC");
    systemTest.checkSpeciesAtomType(sp, 22, "CS");
    systemTest.checkSpeciesAtomType(sp, 23, "HC");
    systemTest.checkSpeciesAtomType(sp, 24, "HC");
    systemTest.checkSpeciesAtomType(sp, 25, "CS");
    systemTest.checkSpeciesAtomType(sp, 26, "HC");
    systemTest.checkSpeciesAtomType(sp, 27, "HC");
    systemTest.checkSpeciesAtomType(sp, 28, "CS");
    systemTest.checkSpeciesAtomType(sp, 29, "HC");
    systemTest.checkSpeciesAtomType(sp, 30, "HC");
    systemTest.checkSpeciesAtomType(sp, 31, "CS");
    systemTest.checkSpeciesAtomType(sp, 32, "HC");
    systemTest.checkSpeciesAtomType(sp, 33, "HC");
    systemTest.checkSpeciesAtomType(sp, 34, "CS");
    systemTest.checkSpeciesAtomType(sp, 35, "HC");
    systemTest.checkSpeciesAtomType(sp, 36, "HC");
    systemTest.checkSpeciesAtomType(sp, 37, "CS");
    systemTest.checkSpeciesAtomType(sp, 38, "HC");
    systemTest.checkSpeciesAtomType(sp, 39, "HC");
    systemTest.checkSpeciesAtomType(sp, 40, "CS");
    systemTest.checkSpeciesAtomType(sp, 41, "HC");
    systemTest.checkSpeciesAtomType(sp, 42, "HC");
    systemTest.checkSpeciesAtomType(sp, 43, "CT");
    systemTest.checkSpeciesAtomType(sp, 44, "HC");
    systemTest.checkSpeciesAtomType(sp, 45, "HC");
    systemTest.checkSpeciesAtomType(sp, 46, "HC");
    systemTest.checkSpeciesAtomType(sp, 47, "HC");
    systemTest.checkSpeciesAtomType(sp, 48, "HC");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, c1c1im)
{
    auto *sp = systemTest.coreData().findSpecies("c1c1im");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "NA");
    systemTest.checkSpeciesAtomType(sp, 1, "CR");
    systemTest.checkSpeciesAtomType(sp, 2, "NA");
    systemTest.checkSpeciesAtomType(sp, 3, "CW");
    systemTest.checkSpeciesAtomType(sp, 4, "CW");
    systemTest.checkSpeciesAtomType(sp, 5, "C1");
    systemTest.checkSpeciesAtomType(sp, 6, "HCR");
    systemTest.checkSpeciesAtomType(sp, 7, "C1");
    systemTest.checkSpeciesAtomType(sp, 8, "HCW");
    systemTest.checkSpeciesAtomType(sp, 9, "HCW");
    systemTest.checkSpeciesAtomType(sp, 10, "H1");
    systemTest.checkSpeciesAtomType(sp, 11, "H1");
    systemTest.checkSpeciesAtomType(sp, 12, "H1");
    systemTest.checkSpeciesAtomType(sp, 13, "H1");
    systemTest.checkSpeciesAtomType(sp, 14, "H1");
    systemTest.checkSpeciesAtomType(sp, 15, "H1");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, c1c1pyrr)
{
    auto *sp = systemTest.coreData().findSpecies("c1c1pyrr");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "N4");
    systemTest.checkSpeciesAtomType(sp, 1, "C1");
    systemTest.checkSpeciesAtomType(sp, 2, "C1");
    systemTest.checkSpeciesAtomType(sp, 3, "C2");
    systemTest.checkSpeciesAtomType(sp, 4, "C2");
    systemTest.checkSpeciesAtomType(sp, 5, "C1");
    systemTest.checkSpeciesAtomType(sp, 6, "C1");
    systemTest.checkSpeciesAtomType(sp, 7, "H1");
    systemTest.checkSpeciesAtomType(sp, 8, "H1");
    systemTest.checkSpeciesAtomType(sp, 9, "H1");
    systemTest.checkSpeciesAtomType(sp, 10, "H1");
    systemTest.checkSpeciesAtomType(sp, 11, "HC");
    systemTest.checkSpeciesAtomType(sp, 12, "HC");
    systemTest.checkSpeciesAtomType(sp, 13, "HC");
    systemTest.checkSpeciesAtomType(sp, 14, "HC");
    systemTest.checkSpeciesAtomType(sp, 15, "H1");
    systemTest.checkSpeciesAtomType(sp, 16, "H1");
    systemTest.checkSpeciesAtomType(sp, 17, "H1");
    systemTest.checkSpeciesAtomType(sp, 18, "H1");
    systemTest.checkSpeciesAtomType(sp, 19, "H1");
    systemTest.checkSpeciesAtomType(sp, 20, "H1");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, c2c1c1im)
{
    auto *sp = systemTest.coreData().findSpecies("c2c1c1im");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "NAM");
    systemTest.checkSpeciesAtomType(sp, 1, "CRM");
    systemTest.checkSpeciesAtomType(sp, 2, "NAM");
    systemTest.checkSpeciesAtomType(sp, 3, "CW");
    systemTest.checkSpeciesAtomType(sp, 4, "CW");
    systemTest.checkSpeciesAtomType(sp, 5, "C1");
    systemTest.checkSpeciesAtomType(sp, 6, "CCR");
    systemTest.checkSpeciesAtomType(sp, 7, "C1");
    systemTest.checkSpeciesAtomType(sp, 8, "HCW");
    systemTest.checkSpeciesAtomType(sp, 9, "HCW");
    systemTest.checkSpeciesAtomType(sp, 10, "H1");
    systemTest.checkSpeciesAtomType(sp, 11, "H1");
    systemTest.checkSpeciesAtomType(sp, 12, "H1");
    systemTest.checkSpeciesAtomType(sp, 13, "CE");
    systemTest.checkSpeciesAtomType(sp, 14, "H1");
    systemTest.checkSpeciesAtomType(sp, 15, "H1");
    systemTest.checkSpeciesAtomType(sp, 16, "HC");
    systemTest.checkSpeciesAtomType(sp, 17, "HC");
    systemTest.checkSpeciesAtomType(sp, 18, "HC");
    systemTest.checkSpeciesAtomType(sp, 19, "H1");
    systemTest.checkSpeciesAtomType(sp, 20, "H1");
    systemTest.checkSpeciesAtomType(sp, 21, "H1");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, c2c1im)
{
    auto *sp = systemTest.coreData().findSpecies("c2c1im");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "NA");
    systemTest.checkSpeciesAtomType(sp, 1, "CR");
    systemTest.checkSpeciesAtomType(sp, 2, "NA");
    systemTest.checkSpeciesAtomType(sp, 3, "CW");
    systemTest.checkSpeciesAtomType(sp, 4, "CW");
    systemTest.checkSpeciesAtomType(sp, 5, "C1");
    systemTest.checkSpeciesAtomType(sp, 6, "HCR");
    systemTest.checkSpeciesAtomType(sp, 7, "C1");
    systemTest.checkSpeciesAtomType(sp, 8, "HCW");
    systemTest.checkSpeciesAtomType(sp, 9, "HCW");
    systemTest.checkSpeciesAtomType(sp, 10, "H1");
    systemTest.checkSpeciesAtomType(sp, 11, "H1");
    systemTest.checkSpeciesAtomType(sp, 12, "H1");
    systemTest.checkSpeciesAtomType(sp, 13, "CE");
    systemTest.checkSpeciesAtomType(sp, 14, "H1");
    systemTest.checkSpeciesAtomType(sp, 15, "H1");
    systemTest.checkSpeciesAtomType(sp, 16, "HC");
    systemTest.checkSpeciesAtomType(sp, 17, "HC");
    systemTest.checkSpeciesAtomType(sp, 18, "HC");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, c2im)
{
    auto *sp = systemTest.coreData().findSpecies("c2im");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "NAH");
    systemTest.checkSpeciesAtomType(sp, 1, "CRH");
    systemTest.checkSpeciesAtomType(sp, 2, "NA");
    systemTest.checkSpeciesAtomType(sp, 3, "CW");
    systemTest.checkSpeciesAtomType(sp, 4, "CWH");
    systemTest.checkSpeciesAtomType(sp, 5, "HCR");
    systemTest.checkSpeciesAtomType(sp, 6, "C1");
    systemTest.checkSpeciesAtomType(sp, 7, "HCW");
    systemTest.checkSpeciesAtomType(sp, 8, "HCW");
    systemTest.checkSpeciesAtomType(sp, 9, "CE");
    systemTest.checkSpeciesAtomType(sp, 10, "H1");
    systemTest.checkSpeciesAtomType(sp, 11, "H1");
    systemTest.checkSpeciesAtomType(sp, 12, "HC");
    systemTest.checkSpeciesAtomType(sp, 13, "HC");
    systemTest.checkSpeciesAtomType(sp, 14, "HC");
    systemTest.checkSpeciesAtomType(sp, 15, "HNA");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, c2OHc1im)
{
    auto *sp = systemTest.coreData().findSpecies("c2OHc1im");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "NA");
    systemTest.checkSpeciesAtomType(sp, 1, "CR");
    systemTest.checkSpeciesAtomType(sp, 2, "NA");
    systemTest.checkSpeciesAtomType(sp, 3, "CW");
    systemTest.checkSpeciesAtomType(sp, 4, "CW");
    systemTest.checkSpeciesAtomType(sp, 5, "C1");
    systemTest.checkSpeciesAtomType(sp, 6, "HCR");
    systemTest.checkSpeciesAtomType(sp, 7, "C1");
    systemTest.checkSpeciesAtomType(sp, 8, "HCW");
    systemTest.checkSpeciesAtomType(sp, 9, "HCW");
    systemTest.checkSpeciesAtomType(sp, 10, "H1");
    systemTest.checkSpeciesAtomType(sp, 11, "H1");
    systemTest.checkSpeciesAtomType(sp, 12, "H1");
    systemTest.checkSpeciesAtomType(sp, 13, "C2O");
    systemTest.checkSpeciesAtomType(sp, 14, "H1");
    systemTest.checkSpeciesAtomType(sp, 15, "H1");
    systemTest.checkSpeciesAtomType(sp, 16, "OH");
    systemTest.checkSpeciesAtomType(sp, 17, "HC");
    systemTest.checkSpeciesAtomType(sp, 18, "HC");
    systemTest.checkSpeciesAtomType(sp, 19, "HO");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, c2py)
{
    auto *sp = systemTest.coreData().findSpecies("c2py");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "NAP");
    systemTest.checkSpeciesAtomType(sp, 1, "CAPO");
    systemTest.checkSpeciesAtomType(sp, 2, "CAPO");
    systemTest.checkSpeciesAtomType(sp, 3, "CAPM");
    systemTest.checkSpeciesAtomType(sp, 4, "HAP");
    systemTest.checkSpeciesAtomType(sp, 5, "CAPP");
    systemTest.checkSpeciesAtomType(sp, 6, "HAP");
    systemTest.checkSpeciesAtomType(sp, 7, "CAPM");
    systemTest.checkSpeciesAtomType(sp, 8, "HAP");
    systemTest.checkSpeciesAtomType(sp, 9, "HAP");
    systemTest.checkSpeciesAtomType(sp, 10, "HAP");
    systemTest.checkSpeciesAtomType(sp, 11, "C1");
    systemTest.checkSpeciesAtomType(sp, 12, "CE");
    systemTest.checkSpeciesAtomType(sp, 13, "HC");
    systemTest.checkSpeciesAtomType(sp, 14, "HC");
    systemTest.checkSpeciesAtomType(sp, 15, "HC");
    systemTest.checkSpeciesAtomType(sp, 16, "H1");
    systemTest.checkSpeciesAtomType(sp, 17, "H1");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, c3c1im)
{
    auto *sp = systemTest.coreData().findSpecies("c3c1im");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "NA");
    systemTest.checkSpeciesAtomType(sp, 1, "CR");
    systemTest.checkSpeciesAtomType(sp, 2, "NA");
    systemTest.checkSpeciesAtomType(sp, 3, "CW");
    systemTest.checkSpeciesAtomType(sp, 4, "CW");
    systemTest.checkSpeciesAtomType(sp, 5, "C1");
    systemTest.checkSpeciesAtomType(sp, 6, "HCR");
    systemTest.checkSpeciesAtomType(sp, 7, "C1");
    systemTest.checkSpeciesAtomType(sp, 8, "HCW");
    systemTest.checkSpeciesAtomType(sp, 9, "HCW");
    systemTest.checkSpeciesAtomType(sp, 10, "H1");
    systemTest.checkSpeciesAtomType(sp, 11, "H1");
    systemTest.checkSpeciesAtomType(sp, 12, "H1");
    systemTest.checkSpeciesAtomType(sp, 13, "C2");
    systemTest.checkSpeciesAtomType(sp, 14, "H1");
    systemTest.checkSpeciesAtomType(sp, 15, "H1");
    systemTest.checkSpeciesAtomType(sp, 16, "CT");
    systemTest.checkSpeciesAtomType(sp, 17, "HC");
    systemTest.checkSpeciesAtomType(sp, 18, "HC");
    systemTest.checkSpeciesAtomType(sp, 19, "HC");
    systemTest.checkSpeciesAtomType(sp, 20, "HC");
    systemTest.checkSpeciesAtomType(sp, 21, "HC");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, c3c1pyrr)
{
    auto *sp = systemTest.coreData().findSpecies("c3c1pyrr");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "N4");
    systemTest.checkSpeciesAtomType(sp, 1, "C1");
    systemTest.checkSpeciesAtomType(sp, 2, "C1");
    systemTest.checkSpeciesAtomType(sp, 3, "C2");
    systemTest.checkSpeciesAtomType(sp, 4, "C2");
    systemTest.checkSpeciesAtomType(sp, 5, "C1");
    systemTest.checkSpeciesAtomType(sp, 6, "C1");
    systemTest.checkSpeciesAtomType(sp, 7, "H1");
    systemTest.checkSpeciesAtomType(sp, 8, "H1");
    systemTest.checkSpeciesAtomType(sp, 9, "H1");
    systemTest.checkSpeciesAtomType(sp, 10, "H1");
    systemTest.checkSpeciesAtomType(sp, 11, "HC");
    systemTest.checkSpeciesAtomType(sp, 12, "HC");
    systemTest.checkSpeciesAtomType(sp, 13, "HC");
    systemTest.checkSpeciesAtomType(sp, 14, "HC");
    systemTest.checkSpeciesAtomType(sp, 15, "H1");
    systemTest.checkSpeciesAtomType(sp, 16, "H1");
    systemTest.checkSpeciesAtomType(sp, 17, "H1");
    systemTest.checkSpeciesAtomType(sp, 18, "H1");
    systemTest.checkSpeciesAtomType(sp, 19, "H1");
    systemTest.checkSpeciesAtomType(sp, 20, "C2");
    systemTest.checkSpeciesAtomType(sp, 21, "HC");
    systemTest.checkSpeciesAtomType(sp, 22, "HC");
    systemTest.checkSpeciesAtomType(sp, 23, "CT");
    systemTest.checkSpeciesAtomType(sp, 24, "HC");
    systemTest.checkSpeciesAtomType(sp, 25, "HC");
    systemTest.checkSpeciesAtomType(sp, 26, "HC");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, c4c1c1im)
{
    auto *sp = systemTest.coreData().findSpecies("c4c1c1im");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "NAM");
    systemTest.checkSpeciesAtomType(sp, 1, "CRM");
    systemTest.checkSpeciesAtomType(sp, 2, "NAM");
    systemTest.checkSpeciesAtomType(sp, 3, "CW");
    systemTest.checkSpeciesAtomType(sp, 4, "CW");
    systemTest.checkSpeciesAtomType(sp, 5, "C1");
    systemTest.checkSpeciesAtomType(sp, 6, "CCR");
    systemTest.checkSpeciesAtomType(sp, 7, "C1");
    systemTest.checkSpeciesAtomType(sp, 8, "HCW");
    systemTest.checkSpeciesAtomType(sp, 9, "HCW");
    systemTest.checkSpeciesAtomType(sp, 10, "H1");
    systemTest.checkSpeciesAtomType(sp, 11, "H1");
    systemTest.checkSpeciesAtomType(sp, 12, "H1");
    systemTest.checkSpeciesAtomType(sp, 13, "C2");
    systemTest.checkSpeciesAtomType(sp, 14, "H1");
    systemTest.checkSpeciesAtomType(sp, 15, "H1");
    systemTest.checkSpeciesAtomType(sp, 16, "CS");
    systemTest.checkSpeciesAtomType(sp, 17, "HC");
    systemTest.checkSpeciesAtomType(sp, 18, "HC");
    systemTest.checkSpeciesAtomType(sp, 19, "CT");
    systemTest.checkSpeciesAtomType(sp, 20, "HC");
    systemTest.checkSpeciesAtomType(sp, 21, "HC");
    systemTest.checkSpeciesAtomType(sp, 22, "HC");
    systemTest.checkSpeciesAtomType(sp, 23, "HC");
    systemTest.checkSpeciesAtomType(sp, 24, "HC");
    systemTest.checkSpeciesAtomType(sp, 25, "H1");
    systemTest.checkSpeciesAtomType(sp, 26, "H1");
    systemTest.checkSpeciesAtomType(sp, 27, "H1");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, c4c1im)
{
    auto *sp = systemTest.coreData().findSpecies("c4c1im");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "NA");
    systemTest.checkSpeciesAtomType(sp, 1, "CR");
    systemTest.checkSpeciesAtomType(sp, 2, "NA");
    systemTest.checkSpeciesAtomType(sp, 3, "CW");
    systemTest.checkSpeciesAtomType(sp, 4, "CW");
    systemTest.checkSpeciesAtomType(sp, 5, "C1");
    systemTest.checkSpeciesAtomType(sp, 6, "HCR");
    systemTest.checkSpeciesAtomType(sp, 7, "C1");
    systemTest.checkSpeciesAtomType(sp, 8, "HCW");
    systemTest.checkSpeciesAtomType(sp, 9, "HCW");
    systemTest.checkSpeciesAtomType(sp, 10, "H1");
    systemTest.checkSpeciesAtomType(sp, 11, "H1");
    systemTest.checkSpeciesAtomType(sp, 12, "H1");
    systemTest.checkSpeciesAtomType(sp, 13, "C2");
    systemTest.checkSpeciesAtomType(sp, 14, "H1");
    systemTest.checkSpeciesAtomType(sp, 15, "H1");
    systemTest.checkSpeciesAtomType(sp, 16, "CS");
    systemTest.checkSpeciesAtomType(sp, 17, "HC");
    systemTest.checkSpeciesAtomType(sp, 18, "HC");
    systemTest.checkSpeciesAtomType(sp, 19, "CT");
    systemTest.checkSpeciesAtomType(sp, 20, "HC");
    systemTest.checkSpeciesAtomType(sp, 21, "HC");
    systemTest.checkSpeciesAtomType(sp, 22, "HC");
    systemTest.checkSpeciesAtomType(sp, 23, "HC");
    systemTest.checkSpeciesAtomType(sp, 24, "HC");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, c4c1pyrr)
{
    auto *sp = systemTest.coreData().findSpecies("c4c1pyrr");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "N4");
    systemTest.checkSpeciesAtomType(sp, 1, "C1");
    systemTest.checkSpeciesAtomType(sp, 2, "C1");
    systemTest.checkSpeciesAtomType(sp, 3, "C2");
    systemTest.checkSpeciesAtomType(sp, 4, "C2");
    systemTest.checkSpeciesAtomType(sp, 5, "C1");
    systemTest.checkSpeciesAtomType(sp, 6, "C1");
    systemTest.checkSpeciesAtomType(sp, 7, "H1");
    systemTest.checkSpeciesAtomType(sp, 8, "H1");
    systemTest.checkSpeciesAtomType(sp, 9, "H1");
    systemTest.checkSpeciesAtomType(sp, 10, "H1");
    systemTest.checkSpeciesAtomType(sp, 11, "HC");
    systemTest.checkSpeciesAtomType(sp, 12, "HC");
    systemTest.checkSpeciesAtomType(sp, 13, "HC");
    systemTest.checkSpeciesAtomType(sp, 14, "HC");
    systemTest.checkSpeciesAtomType(sp, 15, "H1");
    systemTest.checkSpeciesAtomType(sp, 16, "H1");
    systemTest.checkSpeciesAtomType(sp, 17, "H1");
    systemTest.checkSpeciesAtomType(sp, 18, "H1");
    systemTest.checkSpeciesAtomType(sp, 19, "H1");
    systemTest.checkSpeciesAtomType(sp, 20, "C2");
    systemTest.checkSpeciesAtomType(sp, 21, "HC");
    systemTest.checkSpeciesAtomType(sp, 22, "HC");
    systemTest.checkSpeciesAtomType(sp, 23, "CS");
    systemTest.checkSpeciesAtomType(sp, 24, "HC");
    systemTest.checkSpeciesAtomType(sp, 25, "HC");
    systemTest.checkSpeciesAtomType(sp, 26, "CT");
    systemTest.checkSpeciesAtomType(sp, 27, "HC");
    systemTest.checkSpeciesAtomType(sp, 28, "HC");
    systemTest.checkSpeciesAtomType(sp, 29, "HC");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, c4c4im)
{
    auto *sp = systemTest.coreData().findSpecies("c4c4im");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "NA");
    systemTest.checkSpeciesAtomType(sp, 1, "CR");
    systemTest.checkSpeciesAtomType(sp, 2, "NA");
    systemTest.checkSpeciesAtomType(sp, 3, "CW");
    systemTest.checkSpeciesAtomType(sp, 4, "CW");
    systemTest.checkSpeciesAtomType(sp, 5, "C1");
    systemTest.checkSpeciesAtomType(sp, 6, "HCR");
    systemTest.checkSpeciesAtomType(sp, 7, "C1");
    systemTest.checkSpeciesAtomType(sp, 8, "HCW");
    systemTest.checkSpeciesAtomType(sp, 9, "HCW");
    systemTest.checkSpeciesAtomType(sp, 10, "C2");
    systemTest.checkSpeciesAtomType(sp, 11, "H1");
    systemTest.checkSpeciesAtomType(sp, 12, "H1");
    systemTest.checkSpeciesAtomType(sp, 13, "CS");
    systemTest.checkSpeciesAtomType(sp, 14, "HC");
    systemTest.checkSpeciesAtomType(sp, 15, "HC");
    systemTest.checkSpeciesAtomType(sp, 16, "CT");
    systemTest.checkSpeciesAtomType(sp, 17, "HC");
    systemTest.checkSpeciesAtomType(sp, 18, "HC");
    systemTest.checkSpeciesAtomType(sp, 19, "HC");
    systemTest.checkSpeciesAtomType(sp, 20, "HC");
    systemTest.checkSpeciesAtomType(sp, 21, "HC");
    systemTest.checkSpeciesAtomType(sp, 22, "C2");
    systemTest.checkSpeciesAtomType(sp, 23, "H1");
    systemTest.checkSpeciesAtomType(sp, 24, "H1");
    systemTest.checkSpeciesAtomType(sp, 25, "CS");
    systemTest.checkSpeciesAtomType(sp, 26, "HC");
    systemTest.checkSpeciesAtomType(sp, 27, "HC");
    systemTest.checkSpeciesAtomType(sp, 28, "CT");
    systemTest.checkSpeciesAtomType(sp, 29, "HC");
    systemTest.checkSpeciesAtomType(sp, 30, "HC");
    systemTest.checkSpeciesAtomType(sp, 31, "HC");
    systemTest.checkSpeciesAtomType(sp, 32, "HC");
    systemTest.checkSpeciesAtomType(sp, 33, "HC");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, c4pyri)
{
    auto *sp = systemTest.coreData().findSpecies("c4pyri");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "NAP");
    systemTest.checkSpeciesAtomType(sp, 1, "CAPO");
    systemTest.checkSpeciesAtomType(sp, 2, "CAPM");
    systemTest.checkSpeciesAtomType(sp, 3, "CAPP");
    systemTest.checkSpeciesAtomType(sp, 4, "CAPM");
    systemTest.checkSpeciesAtomType(sp, 5, "CAPO");
    systemTest.checkSpeciesAtomType(sp, 6, "C1");
    systemTest.checkSpeciesAtomType(sp, 7, "HAP");
    systemTest.checkSpeciesAtomType(sp, 8, "HAP");
    systemTest.checkSpeciesAtomType(sp, 9, "HAP");
    systemTest.checkSpeciesAtomType(sp, 10, "HAP");
    systemTest.checkSpeciesAtomType(sp, 11, "HAP");
    systemTest.checkSpeciesAtomType(sp, 12, "C2");
    systemTest.checkSpeciesAtomType(sp, 13, "H1");
    systemTest.checkSpeciesAtomType(sp, 14, "H1");
    systemTest.checkSpeciesAtomType(sp, 15, "CS");
    systemTest.checkSpeciesAtomType(sp, 16, "HC");
    systemTest.checkSpeciesAtomType(sp, 17, "HC");
    systemTest.checkSpeciesAtomType(sp, 18, "CT");
    systemTest.checkSpeciesAtomType(sp, 19, "HC");
    systemTest.checkSpeciesAtomType(sp, 20, "HC");
    systemTest.checkSpeciesAtomType(sp, 21, "HC");
    systemTest.checkSpeciesAtomType(sp, 22, "HC");
    systemTest.checkSpeciesAtomType(sp, 23, "HC");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, c6c1im)
{
    auto *sp = systemTest.coreData().findSpecies("c6c1im");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "NA");
    systemTest.checkSpeciesAtomType(sp, 1, "CR");
    systemTest.checkSpeciesAtomType(sp, 2, "NA");
    systemTest.checkSpeciesAtomType(sp, 3, "CW");
    systemTest.checkSpeciesAtomType(sp, 4, "CW");
    systemTest.checkSpeciesAtomType(sp, 5, "C1");
    systemTest.checkSpeciesAtomType(sp, 6, "HCR");
    systemTest.checkSpeciesAtomType(sp, 7, "C1");
    systemTest.checkSpeciesAtomType(sp, 8, "HCW");
    systemTest.checkSpeciesAtomType(sp, 9, "HCW");
    systemTest.checkSpeciesAtomType(sp, 10, "H1");
    systemTest.checkSpeciesAtomType(sp, 11, "H1");
    systemTest.checkSpeciesAtomType(sp, 12, "H1");
    systemTest.checkSpeciesAtomType(sp, 13, "C2");
    systemTest.checkSpeciesAtomType(sp, 14, "H1");
    systemTest.checkSpeciesAtomType(sp, 15, "H1");
    systemTest.checkSpeciesAtomType(sp, 16, "CS");
    systemTest.checkSpeciesAtomType(sp, 17, "HC");
    systemTest.checkSpeciesAtomType(sp, 18, "HC");
    systemTest.checkSpeciesAtomType(sp, 19, "CS");
    systemTest.checkSpeciesAtomType(sp, 20, "HC");
    systemTest.checkSpeciesAtomType(sp, 21, "HC");
    systemTest.checkSpeciesAtomType(sp, 22, "CS");
    systemTest.checkSpeciesAtomType(sp, 23, "HC");
    systemTest.checkSpeciesAtomType(sp, 24, "HC");
    systemTest.checkSpeciesAtomType(sp, 25, "CT");
    systemTest.checkSpeciesAtomType(sp, 26, "HC");
    systemTest.checkSpeciesAtomType(sp, 27, "HC");
    systemTest.checkSpeciesAtomType(sp, 28, "HC");
    systemTest.checkSpeciesAtomType(sp, 29, "HC");
    systemTest.checkSpeciesAtomType(sp, 30, "HC");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, c8c1im)
{
    auto *sp = systemTest.coreData().findSpecies("c8c1im");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "NA");
    systemTest.checkSpeciesAtomType(sp, 1, "CR");
    systemTest.checkSpeciesAtomType(sp, 2, "NA");
    systemTest.checkSpeciesAtomType(sp, 3, "CW");
    systemTest.checkSpeciesAtomType(sp, 4, "CW");
    systemTest.checkSpeciesAtomType(sp, 5, "C1");
    systemTest.checkSpeciesAtomType(sp, 6, "HCR");
    systemTest.checkSpeciesAtomType(sp, 7, "C1");
    systemTest.checkSpeciesAtomType(sp, 8, "HCW");
    systemTest.checkSpeciesAtomType(sp, 9, "HCW");
    systemTest.checkSpeciesAtomType(sp, 10, "H1");
    systemTest.checkSpeciesAtomType(sp, 11, "H1");
    systemTest.checkSpeciesAtomType(sp, 12, "H1");
    systemTest.checkSpeciesAtomType(sp, 13, "C2");
    systemTest.checkSpeciesAtomType(sp, 14, "H1");
    systemTest.checkSpeciesAtomType(sp, 15, "H1");
    systemTest.checkSpeciesAtomType(sp, 16, "CS");
    systemTest.checkSpeciesAtomType(sp, 17, "HC");
    systemTest.checkSpeciesAtomType(sp, 18, "HC");
    systemTest.checkSpeciesAtomType(sp, 19, "CS");
    systemTest.checkSpeciesAtomType(sp, 20, "HC");
    systemTest.checkSpeciesAtomType(sp, 21, "HC");
    systemTest.checkSpeciesAtomType(sp, 22, "CS");
    systemTest.checkSpeciesAtomType(sp, 23, "HC");
    systemTest.checkSpeciesAtomType(sp, 24, "HC");
    systemTest.checkSpeciesAtomType(sp, 25, "CS");
    systemTest.checkSpeciesAtomType(sp, 26, "HC");
    systemTest.checkSpeciesAtomType(sp, 27, "HC");
    systemTest.checkSpeciesAtomType(sp, 28, "CS");
    systemTest.checkSpeciesAtomType(sp, 29, "HC");
    systemTest.checkSpeciesAtomType(sp, 30, "HC");
    systemTest.checkSpeciesAtomType(sp, 31, "CT");
    systemTest.checkSpeciesAtomType(sp, 32, "HC");
    systemTest.checkSpeciesAtomType(sp, 33, "HC");
    systemTest.checkSpeciesAtomType(sp, 34, "HC");
    systemTest.checkSpeciesAtomType(sp, 35, "HC");
    systemTest.checkSpeciesAtomType(sp, 36, "HC");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, c8fc1im)
{
    auto *sp = systemTest.coreData().findSpecies("c8fc1im");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "NA");
    systemTest.checkSpeciesAtomType(sp, 1, "CR");
    systemTest.checkSpeciesAtomType(sp, 2, "NA");
    systemTest.checkSpeciesAtomType(sp, 3, "CW");
    systemTest.checkSpeciesAtomType(sp, 4, "CW");
    systemTest.checkSpeciesAtomType(sp, 5, "C1");
    systemTest.checkSpeciesAtomType(sp, 6, "HCR");
    systemTest.checkSpeciesAtomType(sp, 7, "C1H");
    systemTest.checkSpeciesAtomType(sp, 8, "HCW");
    systemTest.checkSpeciesAtomType(sp, 9, "HCW");
    systemTest.checkSpeciesAtomType(sp, 10, "H1");
    systemTest.checkSpeciesAtomType(sp, 11, "H1");
    systemTest.checkSpeciesAtomType(sp, 12, "H1");
    systemTest.checkSpeciesAtomType(sp, 13, "C2");
    systemTest.checkSpeciesAtomType(sp, 14, "H1");
    systemTest.checkSpeciesAtomType(sp, 15, "H1");
    systemTest.checkSpeciesAtomType(sp, 16, "CFH");
    systemTest.checkSpeciesAtomType(sp, 17, "HC");
    systemTest.checkSpeciesAtomType(sp, 18, "HC");
    systemTest.checkSpeciesAtomType(sp, 19, "CSF");
    systemTest.checkSpeciesAtomType(sp, 20, "F");
    systemTest.checkSpeciesAtomType(sp, 21, "F");
    systemTest.checkSpeciesAtomType(sp, 22, "CSF");
    systemTest.checkSpeciesAtomType(sp, 23, "F");
    systemTest.checkSpeciesAtomType(sp, 24, "F");
    systemTest.checkSpeciesAtomType(sp, 25, "CSF");
    systemTest.checkSpeciesAtomType(sp, 26, "F");
    systemTest.checkSpeciesAtomType(sp, 27, "F");
    systemTest.checkSpeciesAtomType(sp, 28, "CSF");
    systemTest.checkSpeciesAtomType(sp, 29, "F");
    systemTest.checkSpeciesAtomType(sp, 30, "F");
    systemTest.checkSpeciesAtomType(sp, 31, "CTF");
    systemTest.checkSpeciesAtomType(sp, 32, "F");
    systemTest.checkSpeciesAtomType(sp, 33, "F");
    systemTest.checkSpeciesAtomType(sp, 34, "F");
    systemTest.checkSpeciesAtomType(sp, 35, "F");
    systemTest.checkSpeciesAtomType(sp, 36, "F");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, c8isoqui)
{
    auto *sp = systemTest.coreData().findSpecies("c8isoqui");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "NAQ");
    systemTest.checkSpeciesAtomType(sp, 1, "CA");
    systemTest.checkSpeciesAtomType(sp, 2, "CAQ");
    systemTest.checkSpeciesAtomType(sp, 3, "CA");
    systemTest.checkSpeciesAtomType(sp, 4, "CA");
    systemTest.checkSpeciesAtomType(sp, 5, "CA");
    systemTest.checkSpeciesAtomType(sp, 6, "CA");
    systemTest.checkSpeciesAtomType(sp, 7, "CAQ");
    systemTest.checkSpeciesAtomType(sp, 8, "CA");
    systemTest.checkSpeciesAtomType(sp, 9, "CA");
    systemTest.checkSpeciesAtomType(sp, 10, "HAP");
    systemTest.checkSpeciesAtomType(sp, 11, "HA");
    systemTest.checkSpeciesAtomType(sp, 12, "HA");
    systemTest.checkSpeciesAtomType(sp, 13, "HA");
    systemTest.checkSpeciesAtomType(sp, 14, "HA");
    systemTest.checkSpeciesAtomType(sp, 15, "HAP");
    systemTest.checkSpeciesAtomType(sp, 16, "HAP");
    systemTest.checkSpeciesAtomType(sp, 17, "C1");
    systemTest.checkSpeciesAtomType(sp, 18, "H1");
    systemTest.checkSpeciesAtomType(sp, 19, "H1");
    systemTest.checkSpeciesAtomType(sp, 20, "C2");
    systemTest.checkSpeciesAtomType(sp, 21, "CS");
    systemTest.checkSpeciesAtomType(sp, 22, "CS");
    systemTest.checkSpeciesAtomType(sp, 23, "CS");
    systemTest.checkSpeciesAtomType(sp, 24, "CS");
    systemTest.checkSpeciesAtomType(sp, 25, "CS");
    systemTest.checkSpeciesAtomType(sp, 26, "CT");
    systemTest.checkSpeciesAtomType(sp, 27, "HC");
    systemTest.checkSpeciesAtomType(sp, 28, "HC");
    systemTest.checkSpeciesAtomType(sp, 29, "HC");
    systemTest.checkSpeciesAtomType(sp, 30, "HC");
    systemTest.checkSpeciesAtomType(sp, 31, "HC");
    systemTest.checkSpeciesAtomType(sp, 32, "HC");
    systemTest.checkSpeciesAtomType(sp, 33, "HC");
    systemTest.checkSpeciesAtomType(sp, 34, "HC");
    systemTest.checkSpeciesAtomType(sp, 35, "HC");
    systemTest.checkSpeciesAtomType(sp, 36, "HC");
    systemTest.checkSpeciesAtomType(sp, 37, "HC");
    systemTest.checkSpeciesAtomType(sp, 38, "HC");
    systemTest.checkSpeciesAtomType(sp, 39, "HC");
    systemTest.checkSpeciesAtomType(sp, 40, "HC");
    systemTest.checkSpeciesAtomType(sp, 41, "HC");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, cholinium)
{
    auto *sp = systemTest.coreData().findSpecies("cholinium");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "N4");
    systemTest.checkSpeciesAtomType(sp, 1, "C1");
    systemTest.checkSpeciesAtomType(sp, 2, "C1");
    systemTest.checkSpeciesAtomType(sp, 3, "C1");
    systemTest.checkSpeciesAtomType(sp, 4, "H1");
    systemTest.checkSpeciesAtomType(sp, 5, "H1");
    systemTest.checkSpeciesAtomType(sp, 6, "H1");
    systemTest.checkSpeciesAtomType(sp, 7, "H1");
    systemTest.checkSpeciesAtomType(sp, 8, "H1");
    systemTest.checkSpeciesAtomType(sp, 9, "H1");
    systemTest.checkSpeciesAtomType(sp, 10, "H1");
    systemTest.checkSpeciesAtomType(sp, 11, "H1");
    systemTest.checkSpeciesAtomType(sp, 12, "H1");
    systemTest.checkSpeciesAtomType(sp, 13, "C1");
    systemTest.checkSpeciesAtomType(sp, 14, "H1");
    systemTest.checkSpeciesAtomType(sp, 15, "COL");
    systemTest.checkSpeciesAtomType(sp, 16, "H1");
    systemTest.checkSpeciesAtomType(sp, 17, "HC");
    systemTest.checkSpeciesAtomType(sp, 18, "HC");
    systemTest.checkSpeciesAtomType(sp, 19, "OH");
    systemTest.checkSpeciesAtomType(sp, 20, "HO");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, gua)
{
    auto *sp = systemTest.coreData().findSpecies("gua");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "CG");
    systemTest.checkSpeciesAtomType(sp, 1, "NG");
    systemTest.checkSpeciesAtomType(sp, 2, "NG");
    systemTest.checkSpeciesAtomType(sp, 3, "NG");
    systemTest.checkSpeciesAtomType(sp, 4, "HG");
    systemTest.checkSpeciesAtomType(sp, 5, "HG");
    systemTest.checkSpeciesAtomType(sp, 6, "HG");
    systemTest.checkSpeciesAtomType(sp, 7, "HG");
    systemTest.checkSpeciesAtomType(sp, 8, "HG");
    systemTest.checkSpeciesAtomType(sp, 9, "HG");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, N1110)
{
    auto *sp = systemTest.coreData().findSpecies("N1110");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "N3");
    systemTest.checkSpeciesAtomType(sp, 1, "C1");
    systemTest.checkSpeciesAtomType(sp, 2, "C1");
    systemTest.checkSpeciesAtomType(sp, 3, "C1");
    systemTest.checkSpeciesAtomType(sp, 4, "H3");
    systemTest.checkSpeciesAtomType(sp, 5, "H1");
    systemTest.checkSpeciesAtomType(sp, 6, "H1");
    systemTest.checkSpeciesAtomType(sp, 7, "H1");
    systemTest.checkSpeciesAtomType(sp, 8, "H1");
    systemTest.checkSpeciesAtomType(sp, 9, "H1");
    systemTest.checkSpeciesAtomType(sp, 10, "H1");
    systemTest.checkSpeciesAtomType(sp, 11, "H1");
    systemTest.checkSpeciesAtomType(sp, 12, "H1");
    systemTest.checkSpeciesAtomType(sp, 13, "H1");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, N1111)
{
    auto *sp = systemTest.coreData().findSpecies("N1111");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "N4");
    systemTest.checkSpeciesAtomType(sp, 1, "C1");
    systemTest.checkSpeciesAtomType(sp, 2, "C1");
    systemTest.checkSpeciesAtomType(sp, 3, "C1");
    systemTest.checkSpeciesAtomType(sp, 4, "C1");
    systemTest.checkSpeciesAtomType(sp, 5, "H1");
    systemTest.checkSpeciesAtomType(sp, 6, "H1");
    systemTest.checkSpeciesAtomType(sp, 7, "H1");
    systemTest.checkSpeciesAtomType(sp, 8, "H1");
    systemTest.checkSpeciesAtomType(sp, 9, "H1");
    systemTest.checkSpeciesAtomType(sp, 10, "H1");
    systemTest.checkSpeciesAtomType(sp, 11, "H1");
    systemTest.checkSpeciesAtomType(sp, 12, "H1");
    systemTest.checkSpeciesAtomType(sp, 13, "H1");
    systemTest.checkSpeciesAtomType(sp, 14, "H1");
    systemTest.checkSpeciesAtomType(sp, 15, "H1");
    systemTest.checkSpeciesAtomType(sp, 16, "H1");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, N2220)
{
    auto *sp = systemTest.coreData().findSpecies("N2220");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "N3");
    systemTest.checkSpeciesAtomType(sp, 1, "C1");
    systemTest.checkSpeciesAtomType(sp, 2, "C1");
    systemTest.checkSpeciesAtomType(sp, 3, "C1");
    systemTest.checkSpeciesAtomType(sp, 4, "CE");
    systemTest.checkSpeciesAtomType(sp, 5, "H1");
    systemTest.checkSpeciesAtomType(sp, 6, "H1");
    systemTest.checkSpeciesAtomType(sp, 7, "HC");
    systemTest.checkSpeciesAtomType(sp, 8, "HC");
    systemTest.checkSpeciesAtomType(sp, 9, "HC");
    systemTest.checkSpeciesAtomType(sp, 10, "H1");
    systemTest.checkSpeciesAtomType(sp, 11, "H1");
    systemTest.checkSpeciesAtomType(sp, 12, "CE");
    systemTest.checkSpeciesAtomType(sp, 13, "HC");
    systemTest.checkSpeciesAtomType(sp, 14, "HC");
    systemTest.checkSpeciesAtomType(sp, 15, "HC");
    systemTest.checkSpeciesAtomType(sp, 16, "H1");
    systemTest.checkSpeciesAtomType(sp, 17, "H1");
    systemTest.checkSpeciesAtomType(sp, 18, "CE");
    systemTest.checkSpeciesAtomType(sp, 19, "HC");
    systemTest.checkSpeciesAtomType(sp, 20, "HC");
    systemTest.checkSpeciesAtomType(sp, 21, "HC");
    systemTest.checkSpeciesAtomType(sp, 22, "H3");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, N2222)
{
    auto *sp = systemTest.coreData().findSpecies("N2222");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "N4");
    systemTest.checkSpeciesAtomType(sp, 1, "C1");
    systemTest.checkSpeciesAtomType(sp, 2, "C1");
    systemTest.checkSpeciesAtomType(sp, 3, "C1");
    systemTest.checkSpeciesAtomType(sp, 4, "C1");
    systemTest.checkSpeciesAtomType(sp, 5, "H1");
    systemTest.checkSpeciesAtomType(sp, 6, "H1");
    systemTest.checkSpeciesAtomType(sp, 7, "H1");
    systemTest.checkSpeciesAtomType(sp, 8, "H1");
    systemTest.checkSpeciesAtomType(sp, 9, "H1");
    systemTest.checkSpeciesAtomType(sp, 10, "H1");
    systemTest.checkSpeciesAtomType(sp, 11, "H1");
    systemTest.checkSpeciesAtomType(sp, 12, "H1");
    systemTest.checkSpeciesAtomType(sp, 13, "CE");
    systemTest.checkSpeciesAtomType(sp, 14, "CE");
    systemTest.checkSpeciesAtomType(sp, 15, "CE");
    systemTest.checkSpeciesAtomType(sp, 16, "CE");
    systemTest.checkSpeciesAtomType(sp, 17, "HC");
    systemTest.checkSpeciesAtomType(sp, 18, "HC");
    systemTest.checkSpeciesAtomType(sp, 19, "HC");
    systemTest.checkSpeciesAtomType(sp, 20, "HC");
    systemTest.checkSpeciesAtomType(sp, 21, "HC");
    systemTest.checkSpeciesAtomType(sp, 22, "HC");
    systemTest.checkSpeciesAtomType(sp, 23, "HC");
    systemTest.checkSpeciesAtomType(sp, 24, "HC");
    systemTest.checkSpeciesAtomType(sp, 25, "HC");
    systemTest.checkSpeciesAtomType(sp, 26, "HC");
    systemTest.checkSpeciesAtomType(sp, 27, "HC");
    systemTest.checkSpeciesAtomType(sp, 28, "HC");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, N4444)
{
    auto *sp = systemTest.coreData().findSpecies("N4444");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "N4");
    systemTest.checkSpeciesAtomType(sp, 1, "C1");
    systemTest.checkSpeciesAtomType(sp, 2, "C1");
    systemTest.checkSpeciesAtomType(sp, 3, "C1");
    systemTest.checkSpeciesAtomType(sp, 4, "C1");
    systemTest.checkSpeciesAtomType(sp, 5, "H1");
    systemTest.checkSpeciesAtomType(sp, 6, "H1");
    systemTest.checkSpeciesAtomType(sp, 7, "H1");
    systemTest.checkSpeciesAtomType(sp, 8, "H1");
    systemTest.checkSpeciesAtomType(sp, 9, "H1");
    systemTest.checkSpeciesAtomType(sp, 10, "H1");
    systemTest.checkSpeciesAtomType(sp, 11, "H1");
    systemTest.checkSpeciesAtomType(sp, 12, "H1");
    systemTest.checkSpeciesAtomType(sp, 13, "C2");
    systemTest.checkSpeciesAtomType(sp, 14, "C2");
    systemTest.checkSpeciesAtomType(sp, 15, "C2");
    systemTest.checkSpeciesAtomType(sp, 16, "C2");
    systemTest.checkSpeciesAtomType(sp, 17, "HC");
    systemTest.checkSpeciesAtomType(sp, 18, "HC");
    systemTest.checkSpeciesAtomType(sp, 19, "HC");
    systemTest.checkSpeciesAtomType(sp, 20, "HC");
    systemTest.checkSpeciesAtomType(sp, 21, "HC");
    systemTest.checkSpeciesAtomType(sp, 22, "HC");
    systemTest.checkSpeciesAtomType(sp, 23, "HC");
    systemTest.checkSpeciesAtomType(sp, 24, "HC");
    systemTest.checkSpeciesAtomType(sp, 25, "CS");
    systemTest.checkSpeciesAtomType(sp, 26, "CS");
    systemTest.checkSpeciesAtomType(sp, 27, "CS");
    systemTest.checkSpeciesAtomType(sp, 28, "CS");
    systemTest.checkSpeciesAtomType(sp, 29, "CT");
    systemTest.checkSpeciesAtomType(sp, 30, "CT");
    systemTest.checkSpeciesAtomType(sp, 31, "CT");
    systemTest.checkSpeciesAtomType(sp, 32, "CT");
    systemTest.checkSpeciesAtomType(sp, 33, "HC");
    systemTest.checkSpeciesAtomType(sp, 34, "HC");
    systemTest.checkSpeciesAtomType(sp, 35, "HC");
    systemTest.checkSpeciesAtomType(sp, 36, "HC");
    systemTest.checkSpeciesAtomType(sp, 37, "HC");
    systemTest.checkSpeciesAtomType(sp, 38, "HC");
    systemTest.checkSpeciesAtomType(sp, 39, "HC");
    systemTest.checkSpeciesAtomType(sp, 40, "HC");
    systemTest.checkSpeciesAtomType(sp, 41, "HC");
    systemTest.checkSpeciesAtomType(sp, 42, "HC");
    systemTest.checkSpeciesAtomType(sp, 43, "HC");
    systemTest.checkSpeciesAtomType(sp, 44, "HC");
    systemTest.checkSpeciesAtomType(sp, 45, "HC");
    systemTest.checkSpeciesAtomType(sp, 46, "HC");
    systemTest.checkSpeciesAtomType(sp, 47, "HC");
    systemTest.checkSpeciesAtomType(sp, 48, "HC");
    systemTest.checkSpeciesAtomType(sp, 49, "HC");
    systemTest.checkSpeciesAtomType(sp, 50, "HC");
    systemTest.checkSpeciesAtomType(sp, 51, "HC");
    systemTest.checkSpeciesAtomType(sp, 52, "HC");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}

TEST_F(PCL2019CationsForcefieldTest, P66614)
{
    auto *sp = systemTest.coreData().findSpecies("P66614");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "P4");
    systemTest.checkSpeciesAtomType(sp, 1, "C1P");
    systemTest.checkSpeciesAtomType(sp, 2, "C1P");
    systemTest.checkSpeciesAtomType(sp, 3, "C1P");
    systemTest.checkSpeciesAtomType(sp, 4, "C1P");
    systemTest.checkSpeciesAtomType(sp, 5, "C2");
    systemTest.checkSpeciesAtomType(sp, 6, "H1");
    systemTest.checkSpeciesAtomType(sp, 7, "H1");
    systemTest.checkSpeciesAtomType(sp, 8, "H1");
    systemTest.checkSpeciesAtomType(sp, 9, "H1");
    systemTest.checkSpeciesAtomType(sp, 10, "C2");
    systemTest.checkSpeciesAtomType(sp, 11, "C2");
    systemTest.checkSpeciesAtomType(sp, 12, "H1");
    systemTest.checkSpeciesAtomType(sp, 13, "H1");
    systemTest.checkSpeciesAtomType(sp, 14, "H1");
    systemTest.checkSpeciesAtomType(sp, 15, "H1");
    systemTest.checkSpeciesAtomType(sp, 16, "C2");
    systemTest.checkSpeciesAtomType(sp, 17, "CS");
    systemTest.checkSpeciesAtomType(sp, 18, "HC");
    systemTest.checkSpeciesAtomType(sp, 19, "HC");
    systemTest.checkSpeciesAtomType(sp, 20, "CS");
    systemTest.checkSpeciesAtomType(sp, 21, "HC");
    systemTest.checkSpeciesAtomType(sp, 22, "HC");
    systemTest.checkSpeciesAtomType(sp, 23, "CS");
    systemTest.checkSpeciesAtomType(sp, 24, "HC");
    systemTest.checkSpeciesAtomType(sp, 25, "HC");
    systemTest.checkSpeciesAtomType(sp, 26, "CT");
    systemTest.checkSpeciesAtomType(sp, 27, "HC");
    systemTest.checkSpeciesAtomType(sp, 28, "HC");
    systemTest.checkSpeciesAtomType(sp, 29, "HC");
    systemTest.checkSpeciesAtomType(sp, 30, "HC");
    systemTest.checkSpeciesAtomType(sp, 31, "HC");
    systemTest.checkSpeciesAtomType(sp, 32, "CS");
    systemTest.checkSpeciesAtomType(sp, 33, "HC");
    systemTest.checkSpeciesAtomType(sp, 34, "HC");
    systemTest.checkSpeciesAtomType(sp, 35, "CS");
    systemTest.checkSpeciesAtomType(sp, 36, "HC");
    systemTest.checkSpeciesAtomType(sp, 37, "HC");
    systemTest.checkSpeciesAtomType(sp, 38, "CS");
    systemTest.checkSpeciesAtomType(sp, 39, "HC");
    systemTest.checkSpeciesAtomType(sp, 40, "HC");
    systemTest.checkSpeciesAtomType(sp, 41, "CT");
    systemTest.checkSpeciesAtomType(sp, 42, "HC");
    systemTest.checkSpeciesAtomType(sp, 43, "HC");
    systemTest.checkSpeciesAtomType(sp, 44, "HC");
    systemTest.checkSpeciesAtomType(sp, 45, "HC");
    systemTest.checkSpeciesAtomType(sp, 46, "HC");
    systemTest.checkSpeciesAtomType(sp, 47, "CS");
    systemTest.checkSpeciesAtomType(sp, 48, "HC");
    systemTest.checkSpeciesAtomType(sp, 49, "HC");
    systemTest.checkSpeciesAtomType(sp, 50, "CS");
    systemTest.checkSpeciesAtomType(sp, 51, "HC");
    systemTest.checkSpeciesAtomType(sp, 52, "HC");
    systemTest.checkSpeciesAtomType(sp, 53, "CS");
    systemTest.checkSpeciesAtomType(sp, 54, "HC");
    systemTest.checkSpeciesAtomType(sp, 55, "HC");
    systemTest.checkSpeciesAtomType(sp, 56, "CT");
    systemTest.checkSpeciesAtomType(sp, 57, "HC");
    systemTest.checkSpeciesAtomType(sp, 58, "HC");
    systemTest.checkSpeciesAtomType(sp, 59, "HC");
    systemTest.checkSpeciesAtomType(sp, 60, "HC");
    systemTest.checkSpeciesAtomType(sp, 61, "HC");
    systemTest.checkSpeciesAtomType(sp, 62, "CS");
    systemTest.checkSpeciesAtomType(sp, 63, "HC");
    systemTest.checkSpeciesAtomType(sp, 64, "HC");
    systemTest.checkSpeciesAtomType(sp, 65, "CS");
    systemTest.checkSpeciesAtomType(sp, 66, "HC");
    systemTest.checkSpeciesAtomType(sp, 67, "HC");
    systemTest.checkSpeciesAtomType(sp, 68, "CS");
    systemTest.checkSpeciesAtomType(sp, 69, "HC");
    systemTest.checkSpeciesAtomType(sp, 70, "HC");
    systemTest.checkSpeciesAtomType(sp, 71, "CS");
    systemTest.checkSpeciesAtomType(sp, 72, "HC");
    systemTest.checkSpeciesAtomType(sp, 73, "HC");
    systemTest.checkSpeciesAtomType(sp, 74, "CS");
    systemTest.checkSpeciesAtomType(sp, 75, "HC");
    systemTest.checkSpeciesAtomType(sp, 76, "HC");
    systemTest.checkSpeciesAtomType(sp, 77, "CS");
    systemTest.checkSpeciesAtomType(sp, 78, "HC");
    systemTest.checkSpeciesAtomType(sp, 79, "HC");
    systemTest.checkSpeciesAtomType(sp, 80, "CS");
    systemTest.checkSpeciesAtomType(sp, 81, "HC");
    systemTest.checkSpeciesAtomType(sp, 82, "HC");
    systemTest.checkSpeciesAtomType(sp, 83, "CS");
    systemTest.checkSpeciesAtomType(sp, 84, "HC");
    systemTest.checkSpeciesAtomType(sp, 85, "HC");
    systemTest.checkSpeciesAtomType(sp, 86, "CS");
    systemTest.checkSpeciesAtomType(sp, 87, "HC");
    systemTest.checkSpeciesAtomType(sp, 88, "HC");
    systemTest.checkSpeciesAtomType(sp, 89, "CS");
    systemTest.checkSpeciesAtomType(sp, 90, "HC");
    systemTest.checkSpeciesAtomType(sp, 91, "HC");
    systemTest.checkSpeciesAtomType(sp, 92, "CS");
    systemTest.checkSpeciesAtomType(sp, 93, "HC");
    systemTest.checkSpeciesAtomType(sp, 94, "HC");
    systemTest.checkSpeciesAtomType(sp, 95, "CT");
    systemTest.checkSpeciesAtomType(sp, 96, "HC");
    systemTest.checkSpeciesAtomType(sp, 97, "HC");
    systemTest.checkSpeciesAtomType(sp, 98, "HC");
    systemTest.checkSpeciesAtomType(sp, 99, "HC");
    systemTest.checkSpeciesAtomType(sp, 100, "HC");
    EXPECT_NEAR(sp->totalCharge(true), 1, 1.0e-6);
}
}; // namespace UnitTest
