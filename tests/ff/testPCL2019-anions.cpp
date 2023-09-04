// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class PCL2019AnionsForcefieldTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;

    void SetUp() override { ASSERT_NO_THROW(systemTest.setUp("dissolve/input/ff-pcl2019-anions.txt")); }
};

TEST_F(PCL2019AnionsForcefieldTest, beti)
{
    auto *sp = systemTest.coreData().findSpecies("beti");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "C1F");
    systemTest.checkSpeciesAtomType(sp, 1, "CEF");
    systemTest.checkSpeciesAtomType(sp, 2, "F1");
    systemTest.checkSpeciesAtomType(sp, 3, "F1");
    systemTest.checkSpeciesAtomType(sp, 4, "SBT");
    systemTest.checkSpeciesAtomType(sp, 5, "NBT");
    systemTest.checkSpeciesAtomType(sp, 6, "OBT");
    systemTest.checkSpeciesAtomType(sp, 7, "OBT");
    systemTest.checkSpeciesAtomType(sp, 8, "SBT");
    systemTest.checkSpeciesAtomType(sp, 9, "OBT");
    systemTest.checkSpeciesAtomType(sp, 10, "OBT");
    systemTest.checkSpeciesAtomType(sp, 11, "C1F");
    systemTest.checkSpeciesAtomType(sp, 12, "CEF");
    systemTest.checkSpeciesAtomType(sp, 13, "F1");
    systemTest.checkSpeciesAtomType(sp, 14, "F1");
    systemTest.checkSpeciesAtomType(sp, 15, "F");
    systemTest.checkSpeciesAtomType(sp, 16, "F");
    systemTest.checkSpeciesAtomType(sp, 17, "F");
    systemTest.checkSpeciesAtomType(sp, 18, "F");
    systemTest.checkSpeciesAtomType(sp, 19, "F");
    systemTest.checkSpeciesAtomType(sp, 20, "F");
    EXPECT_NEAR(sp->totalCharge(true), -1, 1.0e-6);
}

TEST_F(PCL2019AnionsForcefieldTest, BF4)
{
    auto *sp = systemTest.coreData().findSpecies("BF4");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "B");
    systemTest.checkSpeciesAtomType(sp, 1, "FB");
    systemTest.checkSpeciesAtomType(sp, 2, "FB");
    systemTest.checkSpeciesAtomType(sp, 3, "FB");
    systemTest.checkSpeciesAtomType(sp, 4, "FB");
    EXPECT_NEAR(sp->totalCharge(true), -1, 1.0e-6);
}

TEST_F(PCL2019AnionsForcefieldTest, c1SO3)
{
    auto *sp = systemTest.coreData().findSpecies("c1SO3");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "SO");
    systemTest.checkSpeciesAtomType(sp, 1, "OS3");
    systemTest.checkSpeciesAtomType(sp, 2, "OS3");
    systemTest.checkSpeciesAtomType(sp, 3, "OS3");
    systemTest.checkSpeciesAtomType(sp, 4, "CS3");
    systemTest.checkSpeciesAtomType(sp, 5, "HS3");
    systemTest.checkSpeciesAtomType(sp, 6, "HS3");
    systemTest.checkSpeciesAtomType(sp, 7, "HS3");
    EXPECT_NEAR(sp->totalCharge(true), -1, 1.0e-6);
}

TEST_F(PCL2019AnionsForcefieldTest, c1SO4)
{
    auto *sp = systemTest.coreData().findSpecies("c1SO4");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "HS4");
    systemTest.checkSpeciesAtomType(sp, 1, "CS4");
    systemTest.checkSpeciesAtomType(sp, 2, "OC4");
    systemTest.checkSpeciesAtomType(sp, 3, "HS4");
    systemTest.checkSpeciesAtomType(sp, 4, "HS4");
    systemTest.checkSpeciesAtomType(sp, 5, "SO");
    systemTest.checkSpeciesAtomType(sp, 6, "OS4");
    systemTest.checkSpeciesAtomType(sp, 7, "OS4");
    systemTest.checkSpeciesAtomType(sp, 8, "OS4");
    EXPECT_NEAR(sp->totalCharge(true), -1, 1.0e-6);
}

TEST_F(PCL2019AnionsForcefieldTest, c2SO3)
{
    auto *sp = systemTest.coreData().findSpecies("c2SO3");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "SO");
    systemTest.checkSpeciesAtomType(sp, 1, "OS3");
    systemTest.checkSpeciesAtomType(sp, 2, "OS3");
    systemTest.checkSpeciesAtomType(sp, 3, "OS3");
    systemTest.checkSpeciesAtomType(sp, 4, "CS3");
    systemTest.checkSpeciesAtomType(sp, 5, "CT3");
    systemTest.checkSpeciesAtomType(sp, 6, "HS3");
    systemTest.checkSpeciesAtomType(sp, 7, "HS3");
    systemTest.checkSpeciesAtomType(sp, 8, "HC");
    systemTest.checkSpeciesAtomType(sp, 9, "HC");
    systemTest.checkSpeciesAtomType(sp, 10, "HC");
    EXPECT_NEAR(sp->totalCharge(true), -1, 1.0e-6);
}

TEST_F(PCL2019AnionsForcefieldTest, c2SO4)
{
    auto *sp = systemTest.coreData().findSpecies("c2SO4");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "CT3");
    systemTest.checkSpeciesAtomType(sp, 1, "CS4");
    systemTest.checkSpeciesAtomType(sp, 2, "OC4");
    systemTest.checkSpeciesAtomType(sp, 3, "HC");
    systemTest.checkSpeciesAtomType(sp, 4, "HC");
    systemTest.checkSpeciesAtomType(sp, 5, "HC");
    systemTest.checkSpeciesAtomType(sp, 6, "HS4");
    systemTest.checkSpeciesAtomType(sp, 7, "HS4");
    systemTest.checkSpeciesAtomType(sp, 8, "SO");
    systemTest.checkSpeciesAtomType(sp, 9, "OS4");
    systemTest.checkSpeciesAtomType(sp, 10, "OS4");
    systemTest.checkSpeciesAtomType(sp, 11, "OS4");
    EXPECT_NEAR(sp->totalCharge(true), -1, 1.0e-6);
}

TEST_F(PCL2019AnionsForcefieldTest, c4fc1fsi)
{
    auto *sp = systemTest.coreData().findSpecies("c4fc1fsi");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "CBT");
    systemTest.checkSpeciesAtomType(sp, 1, "F1");
    systemTest.checkSpeciesAtomType(sp, 2, "F1");
    systemTest.checkSpeciesAtomType(sp, 3, "F1");
    systemTest.checkSpeciesAtomType(sp, 4, "SBT");
    systemTest.checkSpeciesAtomType(sp, 5, "NBT");
    systemTest.checkSpeciesAtomType(sp, 6, "OBT");
    systemTest.checkSpeciesAtomType(sp, 7, "OBT");
    systemTest.checkSpeciesAtomType(sp, 8, "SBT");
    systemTest.checkSpeciesAtomType(sp, 9, "OBT");
    systemTest.checkSpeciesAtomType(sp, 10, "OBT");
    systemTest.checkSpeciesAtomType(sp, 11, "C1F");
    systemTest.checkSpeciesAtomType(sp, 12, "CSF");
    systemTest.checkSpeciesAtomType(sp, 13, "F1");
    systemTest.checkSpeciesAtomType(sp, 14, "F1");
    systemTest.checkSpeciesAtomType(sp, 15, "CSF");
    systemTest.checkSpeciesAtomType(sp, 16, "F");
    systemTest.checkSpeciesAtomType(sp, 17, "F");
    systemTest.checkSpeciesAtomType(sp, 18, "CTF");
    systemTest.checkSpeciesAtomType(sp, 19, "F");
    systemTest.checkSpeciesAtomType(sp, 20, "F");
    systemTest.checkSpeciesAtomType(sp, 21, "F");
    systemTest.checkSpeciesAtomType(sp, 22, "F");
    systemTest.checkSpeciesAtomType(sp, 23, "F");
    EXPECT_NEAR(sp->totalCharge(true), -1, 1.0e-6);
}

TEST_F(PCL2019AnionsForcefieldTest, CCN3)
{
    auto *sp = systemTest.coreData().findSpecies("CCN3");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "C3A");
    systemTest.checkSpeciesAtomType(sp, 1, "CN");
    systemTest.checkSpeciesAtomType(sp, 2, "CN");
    systemTest.checkSpeciesAtomType(sp, 3, "CN");
    systemTest.checkSpeciesAtomType(sp, 4, "NC");
    EXPECT_NEAR(sp->totalCharge(true), -1, 1.0e-6);
}

TEST_F(PCL2019AnionsForcefieldTest, dca)
{
    auto *sp = systemTest.coreData().findSpecies("dca");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "N3A");
    systemTest.checkSpeciesAtomType(sp, 1, "CZA");
    systemTest.checkSpeciesAtomType(sp, 2, "NZA");
    systemTest.checkSpeciesAtomType(sp, 3, "CZA");
    systemTest.checkSpeciesAtomType(sp, 4, "NZA");
    EXPECT_NEAR(sp->totalCharge(true), -1, 1.0e-6);
}

TEST_F(PCL2019AnionsForcefieldTest, fsi)
{
    auto *sp = systemTest.coreData().findSpecies("fsi");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "FSI");
    systemTest.checkSpeciesAtomType(sp, 1, "SBT");
    systemTest.checkSpeciesAtomType(sp, 2, "NBT");
    systemTest.checkSpeciesAtomType(sp, 3, "OBT");
    systemTest.checkSpeciesAtomType(sp, 4, "OBT");
    systemTest.checkSpeciesAtomType(sp, 5, "SBT");
    systemTest.checkSpeciesAtomType(sp, 6, "OBT");
    systemTest.checkSpeciesAtomType(sp, 7, "OBT");
    systemTest.checkSpeciesAtomType(sp, 8, "FSI");
    EXPECT_NEAR(sp->totalCharge(true), -1, 1.0e-6);
}

TEST_F(PCL2019AnionsForcefieldTest, ntf2)
{
    auto *sp = systemTest.coreData().findSpecies("ntf2");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "CBT");
    systemTest.checkSpeciesAtomType(sp, 1, "F1");
    systemTest.checkSpeciesAtomType(sp, 2, "F1");
    systemTest.checkSpeciesAtomType(sp, 3, "F1");
    systemTest.checkSpeciesAtomType(sp, 4, "SBT");
    systemTest.checkSpeciesAtomType(sp, 5, "NBT");
    systemTest.checkSpeciesAtomType(sp, 6, "OBT");
    systemTest.checkSpeciesAtomType(sp, 7, "OBT");
    systemTest.checkSpeciesAtomType(sp, 8, "SBT");
    systemTest.checkSpeciesAtomType(sp, 9, "OBT");
    systemTest.checkSpeciesAtomType(sp, 10, "OBT");
    systemTest.checkSpeciesAtomType(sp, 11, "CBT");
    systemTest.checkSpeciesAtomType(sp, 12, "F1");
    systemTest.checkSpeciesAtomType(sp, 13, "F1");
    systemTest.checkSpeciesAtomType(sp, 14, "F1");
    EXPECT_NEAR(sp->totalCharge(true), -1, 1.0e-6);
}

TEST_F(PCL2019AnionsForcefieldTest, oac)
{
    auto *sp = systemTest.coreData().findSpecies("oac");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "CTA");
    systemTest.checkSpeciesAtomType(sp, 1, "HC");
    systemTest.checkSpeciesAtomType(sp, 2, "HC");
    systemTest.checkSpeciesAtomType(sp, 3, "HC");
    systemTest.checkSpeciesAtomType(sp, 4, "CO2");
    systemTest.checkSpeciesAtomType(sp, 5, "O2");
    systemTest.checkSpeciesAtomType(sp, 6, "O2");
    EXPECT_NEAR(sp->totalCharge(true), -1, 1.0e-6);
}

TEST_F(PCL2019AnionsForcefieldTest, otf)
{
    auto *sp = systemTest.coreData().findSpecies("otf");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "CBT");
    systemTest.checkSpeciesAtomType(sp, 1, "F1");
    systemTest.checkSpeciesAtomType(sp, 2, "F1");
    systemTest.checkSpeciesAtomType(sp, 3, "F1");
    systemTest.checkSpeciesAtomType(sp, 4, "SBT");
    systemTest.checkSpeciesAtomType(sp, 5, "OTF");
    systemTest.checkSpeciesAtomType(sp, 6, "OTF");
    systemTest.checkSpeciesAtomType(sp, 7, "OTF");
    EXPECT_NEAR(sp->totalCharge(true), -1, 1.0e-6);
}

TEST_F(PCL2019AnionsForcefieldTest, PF6)
{
    auto *sp = systemTest.coreData().findSpecies("PF6");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "P");
    systemTest.checkSpeciesAtomType(sp, 1, "FP");
    systemTest.checkSpeciesAtomType(sp, 2, "FP");
    systemTest.checkSpeciesAtomType(sp, 3, "FP");
    systemTest.checkSpeciesAtomType(sp, 4, "FP");
    systemTest.checkSpeciesAtomType(sp, 5, "FP");
    systemTest.checkSpeciesAtomType(sp, 6, "FP");
    EXPECT_NEAR(sp->totalCharge(true), -1, 1.0e-6);
}

TEST_F(PCL2019AnionsForcefieldTest, SCN)
{
    auto *sp = systemTest.coreData().findSpecies("SCN");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "SK");
    systemTest.checkSpeciesAtomType(sp, 1, "CK");
    systemTest.checkSpeciesAtomType(sp, 2, "NK");
    EXPECT_NEAR(sp->totalCharge(true), -1, 1.0e-6);
}

TEST_F(PCL2019AnionsForcefieldTest, tfa)
{
    auto *sp = systemTest.coreData().findSpecies("tfa");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "CFA");
    systemTest.checkSpeciesAtomType(sp, 1, "FFA");
    systemTest.checkSpeciesAtomType(sp, 2, "FFA");
    systemTest.checkSpeciesAtomType(sp, 3, "FFA");
    systemTest.checkSpeciesAtomType(sp, 4, "CO2");
    systemTest.checkSpeciesAtomType(sp, 5, "O2F");
    systemTest.checkSpeciesAtomType(sp, 6, "O2F");
    EXPECT_NEAR(sp->totalCharge(true), -1, 1.0e-6);
}

TEST_F(PCL2019AnionsForcefieldTest, tso)
{
    auto *sp = systemTest.coreData().findSpecies("tso");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "CATS");
    systemTest.checkSpeciesAtomType(sp, 1, "CAOS");
    systemTest.checkSpeciesAtomType(sp, 2, "CAMS");
    systemTest.checkSpeciesAtomType(sp, 3, "CAOS");
    systemTest.checkSpeciesAtomType(sp, 4, "CAMS");
    systemTest.checkSpeciesAtomType(sp, 5, "CAPS");
    systemTest.checkSpeciesAtomType(sp, 6, "CTTS");
    systemTest.checkSpeciesAtomType(sp, 7, "SO");
    systemTest.checkSpeciesAtomType(sp, 8, "HATS");
    systemTest.checkSpeciesAtomType(sp, 9, "HATS");
    systemTest.checkSpeciesAtomType(sp, 10, "HATS");
    systemTest.checkSpeciesAtomType(sp, 11, "HATS");
    systemTest.checkSpeciesAtomType(sp, 12, "HTS");
    systemTest.checkSpeciesAtomType(sp, 13, "HTS");
    systemTest.checkSpeciesAtomType(sp, 14, "HTS");
    systemTest.checkSpeciesAtomType(sp, 15, "OS3");
    systemTest.checkSpeciesAtomType(sp, 16, "OS3");
    systemTest.checkSpeciesAtomType(sp, 17, "OS3");
    EXPECT_NEAR(sp->totalCharge(true), -1, 1.0e-6);
}
}; // namespace UnitTest
