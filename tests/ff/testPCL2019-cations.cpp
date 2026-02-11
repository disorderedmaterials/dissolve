// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/library.h"
#include "io/import/species.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(PCL2019CationsForcefieldTest, benzc1im)
{
    Species species("benzc1im");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/benzc1im.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 27);
    ASSERT_EQ(species.nAngles(), 45);
    ASSERT_EQ(species.nTorsions(), 62);
    ASSERT_EQ(species.nImpropers(), 11);

    systemTest.checkSpeciesAtomType(&species, 0, "NA");
    systemTest.checkSpeciesAtomType(&species, 1, "CR");
    systemTest.checkSpeciesAtomType(&species, 2, "NA");
    systemTest.checkSpeciesAtomType(&species, 3, "CW");
    systemTest.checkSpeciesAtomType(&species, 4, "CW");
    systemTest.checkSpeciesAtomType(&species, 5, "C1");
    systemTest.checkSpeciesAtomType(&species, 6, "HCR");
    systemTest.checkSpeciesAtomType(&species, 7, "C1T");
    systemTest.checkSpeciesAtomType(&species, 8, "HCW");
    systemTest.checkSpeciesAtomType(&species, 9, "HCW");
    systemTest.checkSpeciesAtomType(&species, 10, "H1");
    systemTest.checkSpeciesAtomType(&species, 11, "H1");
    systemTest.checkSpeciesAtomType(&species, 12, "H1");
    systemTest.checkSpeciesAtomType(&species, 13, "H1");
    systemTest.checkSpeciesAtomType(&species, 14, "H1");
    systemTest.checkSpeciesAtomType(&species, 15, "CAT");
    systemTest.checkSpeciesAtomType(&species, 16, "CAO");
    systemTest.checkSpeciesAtomType(&species, 17, "CAM");
    systemTest.checkSpeciesAtomType(&species, 18, "CAP");
    systemTest.checkSpeciesAtomType(&species, 19, "CAM");
    systemTest.checkSpeciesAtomType(&species, 20, "CAO");
    systemTest.checkSpeciesAtomType(&species, 21, "HAT");
    systemTest.checkSpeciesAtomType(&species, 22, "HAT");
    systemTest.checkSpeciesAtomType(&species, 23, "HAT");
    systemTest.checkSpeciesAtomType(&species, 24, "HAT");
    systemTest.checkSpeciesAtomType(&species, 25, "HAT");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, c12c1im)
{
    Species species("c12c1im");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/c12c1im.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 49);
    ASSERT_EQ(species.nAngles(), 93);
    ASSERT_EQ(species.nTorsions(), 131);
    ASSERT_EQ(species.nImpropers(), 5);

    systemTest.checkSpeciesAtomType(&species, 0, "NA");
    systemTest.checkSpeciesAtomType(&species, 1, "CR");
    systemTest.checkSpeciesAtomType(&species, 2, "NA");
    systemTest.checkSpeciesAtomType(&species, 3, "CW");
    systemTest.checkSpeciesAtomType(&species, 4, "CW");
    systemTest.checkSpeciesAtomType(&species, 5, "C1");
    systemTest.checkSpeciesAtomType(&species, 6, "HCR");
    systemTest.checkSpeciesAtomType(&species, 7, "C1");
    systemTest.checkSpeciesAtomType(&species, 8, "HCW");
    systemTest.checkSpeciesAtomType(&species, 9, "HCW");
    systemTest.checkSpeciesAtomType(&species, 10, "H1");
    systemTest.checkSpeciesAtomType(&species, 11, "H1");
    systemTest.checkSpeciesAtomType(&species, 12, "H1");
    systemTest.checkSpeciesAtomType(&species, 13, "C2");
    systemTest.checkSpeciesAtomType(&species, 14, "H1");
    systemTest.checkSpeciesAtomType(&species, 15, "H1");
    systemTest.checkSpeciesAtomType(&species, 16, "CS");
    systemTest.checkSpeciesAtomType(&species, 17, "HC");
    systemTest.checkSpeciesAtomType(&species, 18, "HC");
    systemTest.checkSpeciesAtomType(&species, 19, "CS");
    systemTest.checkSpeciesAtomType(&species, 20, "HC");
    systemTest.checkSpeciesAtomType(&species, 21, "HC");
    systemTest.checkSpeciesAtomType(&species, 22, "CS");
    systemTest.checkSpeciesAtomType(&species, 23, "HC");
    systemTest.checkSpeciesAtomType(&species, 24, "HC");
    systemTest.checkSpeciesAtomType(&species, 25, "CS");
    systemTest.checkSpeciesAtomType(&species, 26, "HC");
    systemTest.checkSpeciesAtomType(&species, 27, "HC");
    systemTest.checkSpeciesAtomType(&species, 28, "CS");
    systemTest.checkSpeciesAtomType(&species, 29, "HC");
    systemTest.checkSpeciesAtomType(&species, 30, "HC");
    systemTest.checkSpeciesAtomType(&species, 31, "CS");
    systemTest.checkSpeciesAtomType(&species, 32, "HC");
    systemTest.checkSpeciesAtomType(&species, 33, "HC");
    systemTest.checkSpeciesAtomType(&species, 34, "CS");
    systemTest.checkSpeciesAtomType(&species, 35, "HC");
    systemTest.checkSpeciesAtomType(&species, 36, "HC");
    systemTest.checkSpeciesAtomType(&species, 37, "CS");
    systemTest.checkSpeciesAtomType(&species, 38, "HC");
    systemTest.checkSpeciesAtomType(&species, 39, "HC");
    systemTest.checkSpeciesAtomType(&species, 40, "CS");
    systemTest.checkSpeciesAtomType(&species, 41, "HC");
    systemTest.checkSpeciesAtomType(&species, 42, "HC");
    systemTest.checkSpeciesAtomType(&species, 43, "CT");
    systemTest.checkSpeciesAtomType(&species, 44, "HC");
    systemTest.checkSpeciesAtomType(&species, 45, "HC");
    systemTest.checkSpeciesAtomType(&species, 46, "HC");
    systemTest.checkSpeciesAtomType(&species, 47, "HC");
    systemTest.checkSpeciesAtomType(&species, 48, "HC");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, c1c1im)
{
    Species species("c1c1im");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/c1c1im.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 16);
    ASSERT_EQ(species.nAngles(), 27);
    ASSERT_EQ(species.nTorsions(), 32);
    ASSERT_EQ(species.nImpropers(), 5);

    systemTest.checkSpeciesAtomType(&species, 0, "NA");
    systemTest.checkSpeciesAtomType(&species, 1, "CR");
    systemTest.checkSpeciesAtomType(&species, 2, "NA");
    systemTest.checkSpeciesAtomType(&species, 3, "CW");
    systemTest.checkSpeciesAtomType(&species, 4, "CW");
    systemTest.checkSpeciesAtomType(&species, 5, "C1");
    systemTest.checkSpeciesAtomType(&species, 6, "HCR");
    systemTest.checkSpeciesAtomType(&species, 7, "C1");
    systemTest.checkSpeciesAtomType(&species, 8, "HCW");
    systemTest.checkSpeciesAtomType(&species, 9, "HCW");
    systemTest.checkSpeciesAtomType(&species, 10, "H1");
    systemTest.checkSpeciesAtomType(&species, 11, "H1");
    systemTest.checkSpeciesAtomType(&species, 12, "H1");
    systemTest.checkSpeciesAtomType(&species, 13, "H1");
    systemTest.checkSpeciesAtomType(&species, 14, "H1");
    systemTest.checkSpeciesAtomType(&species, 15, "H1");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, c1c1pyrr)
{
    Species species("c1c1pyrr");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/c1c1pyrr.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 21);
    ASSERT_EQ(species.nAngles(), 42);
    ASSERT_EQ(species.nTorsions(), 63);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "N4");
    systemTest.checkSpeciesAtomType(&species, 1, "C1");
    systemTest.checkSpeciesAtomType(&species, 2, "C1");
    systemTest.checkSpeciesAtomType(&species, 3, "C2");
    systemTest.checkSpeciesAtomType(&species, 4, "C2");
    systemTest.checkSpeciesAtomType(&species, 5, "C1");
    systemTest.checkSpeciesAtomType(&species, 6, "C1");
    systemTest.checkSpeciesAtomType(&species, 7, "H1");
    systemTest.checkSpeciesAtomType(&species, 8, "H1");
    systemTest.checkSpeciesAtomType(&species, 9, "H1");
    systemTest.checkSpeciesAtomType(&species, 10, "H1");
    systemTest.checkSpeciesAtomType(&species, 11, "HC");
    systemTest.checkSpeciesAtomType(&species, 12, "HC");
    systemTest.checkSpeciesAtomType(&species, 13, "HC");
    systemTest.checkSpeciesAtomType(&species, 14, "HC");
    systemTest.checkSpeciesAtomType(&species, 15, "H1");
    systemTest.checkSpeciesAtomType(&species, 16, "H1");
    systemTest.checkSpeciesAtomType(&species, 17, "H1");
    systemTest.checkSpeciesAtomType(&species, 18, "H1");
    systemTest.checkSpeciesAtomType(&species, 19, "H1");
    systemTest.checkSpeciesAtomType(&species, 20, "H1");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, c2c1c1im)
{
    Species species("c2c1c1im");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/c2c1c1im.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 22);
    ASSERT_EQ(species.nAngles(), 39);
    ASSERT_EQ(species.nTorsions(), 47);
    ASSERT_EQ(species.nImpropers(), 5);

    systemTest.checkSpeciesAtomType(&species, 0, "NAM");
    systemTest.checkSpeciesAtomType(&species, 1, "CRM");
    systemTest.checkSpeciesAtomType(&species, 2, "NAM");
    systemTest.checkSpeciesAtomType(&species, 3, "CW");
    systemTest.checkSpeciesAtomType(&species, 4, "CW");
    systemTest.checkSpeciesAtomType(&species, 5, "C1");
    systemTest.checkSpeciesAtomType(&species, 6, "CCR");
    systemTest.checkSpeciesAtomType(&species, 7, "C1");
    systemTest.checkSpeciesAtomType(&species, 8, "HCW");
    systemTest.checkSpeciesAtomType(&species, 9, "HCW");
    systemTest.checkSpeciesAtomType(&species, 10, "H1");
    systemTest.checkSpeciesAtomType(&species, 11, "H1");
    systemTest.checkSpeciesAtomType(&species, 12, "H1");
    systemTest.checkSpeciesAtomType(&species, 13, "CE");
    systemTest.checkSpeciesAtomType(&species, 14, "H1");
    systemTest.checkSpeciesAtomType(&species, 15, "H1");
    systemTest.checkSpeciesAtomType(&species, 16, "HC");
    systemTest.checkSpeciesAtomType(&species, 17, "HC");
    systemTest.checkSpeciesAtomType(&species, 18, "HC");
    systemTest.checkSpeciesAtomType(&species, 19, "H1");
    systemTest.checkSpeciesAtomType(&species, 20, "H1");
    systemTest.checkSpeciesAtomType(&species, 21, "H1");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, c2c1im)
{
    Species species("c2c1im");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/c2c1im.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 19);
    ASSERT_EQ(species.nAngles(), 33);
    ASSERT_EQ(species.nTorsions(), 41);
    ASSERT_EQ(species.nImpropers(), 5);

    systemTest.checkSpeciesAtomType(&species, 0, "NA");
    systemTest.checkSpeciesAtomType(&species, 1, "CR");
    systemTest.checkSpeciesAtomType(&species, 2, "NA");
    systemTest.checkSpeciesAtomType(&species, 3, "CW");
    systemTest.checkSpeciesAtomType(&species, 4, "CW");
    systemTest.checkSpeciesAtomType(&species, 5, "C1");
    systemTest.checkSpeciesAtomType(&species, 6, "HCR");
    systemTest.checkSpeciesAtomType(&species, 7, "C1");
    systemTest.checkSpeciesAtomType(&species, 8, "HCW");
    systemTest.checkSpeciesAtomType(&species, 9, "HCW");
    systemTest.checkSpeciesAtomType(&species, 10, "H1");
    systemTest.checkSpeciesAtomType(&species, 11, "H1");
    systemTest.checkSpeciesAtomType(&species, 12, "H1");
    systemTest.checkSpeciesAtomType(&species, 13, "CE");
    systemTest.checkSpeciesAtomType(&species, 14, "H1");
    systemTest.checkSpeciesAtomType(&species, 15, "H1");
    systemTest.checkSpeciesAtomType(&species, 16, "HC");
    systemTest.checkSpeciesAtomType(&species, 17, "HC");
    systemTest.checkSpeciesAtomType(&species, 18, "HC");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, c2im)
{
    Species species("c2im");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/c2im.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 16);
    ASSERT_EQ(species.nAngles(), 27);
    ASSERT_EQ(species.nTorsions(), 35);
    ASSERT_EQ(species.nImpropers(), 4);

    systemTest.checkSpeciesAtomType(&species, 0, "NAH");
    systemTest.checkSpeciesAtomType(&species, 1, "CRH");
    systemTest.checkSpeciesAtomType(&species, 2, "NA");
    systemTest.checkSpeciesAtomType(&species, 3, "CW");
    systemTest.checkSpeciesAtomType(&species, 4, "CWH");
    systemTest.checkSpeciesAtomType(&species, 5, "HCR");
    systemTest.checkSpeciesAtomType(&species, 6, "C1");
    systemTest.checkSpeciesAtomType(&species, 7, "HCW");
    systemTest.checkSpeciesAtomType(&species, 8, "HCW");
    systemTest.checkSpeciesAtomType(&species, 9, "CE");
    systemTest.checkSpeciesAtomType(&species, 10, "H1");
    systemTest.checkSpeciesAtomType(&species, 11, "H1");
    systemTest.checkSpeciesAtomType(&species, 12, "HC");
    systemTest.checkSpeciesAtomType(&species, 13, "HC");
    systemTest.checkSpeciesAtomType(&species, 14, "HC");
    systemTest.checkSpeciesAtomType(&species, 15, "HNA");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, c2OHc1im)
{
    Species species("c2OHc1im");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/c2ohc1im.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 20);
    ASSERT_EQ(species.nAngles(), 34);
    ASSERT_EQ(species.nTorsions(), 44);
    ASSERT_EQ(species.nImpropers(), 5);

    systemTest.checkSpeciesAtomType(&species, 0, "NA");
    systemTest.checkSpeciesAtomType(&species, 1, "CR");
    systemTest.checkSpeciesAtomType(&species, 2, "NA");
    systemTest.checkSpeciesAtomType(&species, 3, "CW");
    systemTest.checkSpeciesAtomType(&species, 4, "CW");
    systemTest.checkSpeciesAtomType(&species, 5, "C1");
    systemTest.checkSpeciesAtomType(&species, 6, "HCR");
    systemTest.checkSpeciesAtomType(&species, 7, "C1");
    systemTest.checkSpeciesAtomType(&species, 8, "HCW");
    systemTest.checkSpeciesAtomType(&species, 9, "HCW");
    systemTest.checkSpeciesAtomType(&species, 10, "H1");
    systemTest.checkSpeciesAtomType(&species, 11, "H1");
    systemTest.checkSpeciesAtomType(&species, 12, "H1");
    systemTest.checkSpeciesAtomType(&species, 13, "C2O");
    systemTest.checkSpeciesAtomType(&species, 14, "H1");
    systemTest.checkSpeciesAtomType(&species, 15, "H1");
    systemTest.checkSpeciesAtomType(&species, 16, "OH");
    systemTest.checkSpeciesAtomType(&species, 17, "HC");
    systemTest.checkSpeciesAtomType(&species, 18, "HC");
    systemTest.checkSpeciesAtomType(&species, 19, "HO");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, c2py)
{
    Species species("c2py");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/c2py.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 18);
    ASSERT_EQ(species.nAngles(), 30);
    ASSERT_EQ(species.nTorsions(), 39);
    ASSERT_EQ(species.nImpropers(), 6);

    systemTest.checkSpeciesAtomType(&species, 0, "NAP");
    systemTest.checkSpeciesAtomType(&species, 1, "CAPO");
    systemTest.checkSpeciesAtomType(&species, 2, "CAPO");
    systemTest.checkSpeciesAtomType(&species, 3, "CAPM");
    systemTest.checkSpeciesAtomType(&species, 4, "HAP");
    systemTest.checkSpeciesAtomType(&species, 5, "CAPP");
    systemTest.checkSpeciesAtomType(&species, 6, "HAP");
    systemTest.checkSpeciesAtomType(&species, 7, "CAPM");
    systemTest.checkSpeciesAtomType(&species, 8, "HAP");
    systemTest.checkSpeciesAtomType(&species, 9, "HAP");
    systemTest.checkSpeciesAtomType(&species, 10, "HAP");
    systemTest.checkSpeciesAtomType(&species, 11, "C1");
    systemTest.checkSpeciesAtomType(&species, 12, "CE");
    systemTest.checkSpeciesAtomType(&species, 13, "HC");
    systemTest.checkSpeciesAtomType(&species, 14, "HC");
    systemTest.checkSpeciesAtomType(&species, 15, "HC");
    systemTest.checkSpeciesAtomType(&species, 16, "H1");
    systemTest.checkSpeciesAtomType(&species, 17, "H1");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, c3c1im)
{
    Species species("c3c1im");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/c3c1im.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 22);
    ASSERT_EQ(species.nAngles(), 39);
    ASSERT_EQ(species.nTorsions(), 50);
    ASSERT_EQ(species.nImpropers(), 5);

    systemTest.checkSpeciesAtomType(&species, 0, "NA");
    systemTest.checkSpeciesAtomType(&species, 1, "CR");
    systemTest.checkSpeciesAtomType(&species, 2, "NA");
    systemTest.checkSpeciesAtomType(&species, 3, "CW");
    systemTest.checkSpeciesAtomType(&species, 4, "CW");
    systemTest.checkSpeciesAtomType(&species, 5, "C1");
    systemTest.checkSpeciesAtomType(&species, 6, "HCR");
    systemTest.checkSpeciesAtomType(&species, 7, "C1");
    systemTest.checkSpeciesAtomType(&species, 8, "HCW");
    systemTest.checkSpeciesAtomType(&species, 9, "HCW");
    systemTest.checkSpeciesAtomType(&species, 10, "H1");
    systemTest.checkSpeciesAtomType(&species, 11, "H1");
    systemTest.checkSpeciesAtomType(&species, 12, "H1");
    systemTest.checkSpeciesAtomType(&species, 13, "C2");
    systemTest.checkSpeciesAtomType(&species, 14, "H1");
    systemTest.checkSpeciesAtomType(&species, 15, "H1");
    systemTest.checkSpeciesAtomType(&species, 16, "CT");
    systemTest.checkSpeciesAtomType(&species, 17, "HC");
    systemTest.checkSpeciesAtomType(&species, 18, "HC");
    systemTest.checkSpeciesAtomType(&species, 19, "HC");
    systemTest.checkSpeciesAtomType(&species, 20, "HC");
    systemTest.checkSpeciesAtomType(&species, 21, "HC");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, c3c1pyrr)
{
    Species species("c3c1pyrr");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/c3c1pyrr.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 27);
    ASSERT_EQ(species.nAngles(), 54);
    ASSERT_EQ(species.nTorsions(), 81);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "N4");
    systemTest.checkSpeciesAtomType(&species, 1, "C1");
    systemTest.checkSpeciesAtomType(&species, 2, "C1");
    systemTest.checkSpeciesAtomType(&species, 3, "C2");
    systemTest.checkSpeciesAtomType(&species, 4, "C2");
    systemTest.checkSpeciesAtomType(&species, 5, "C1");
    systemTest.checkSpeciesAtomType(&species, 6, "C1");
    systemTest.checkSpeciesAtomType(&species, 7, "H1");
    systemTest.checkSpeciesAtomType(&species, 8, "H1");
    systemTest.checkSpeciesAtomType(&species, 9, "H1");
    systemTest.checkSpeciesAtomType(&species, 10, "H1");
    systemTest.checkSpeciesAtomType(&species, 11, "HC");
    systemTest.checkSpeciesAtomType(&species, 12, "HC");
    systemTest.checkSpeciesAtomType(&species, 13, "HC");
    systemTest.checkSpeciesAtomType(&species, 14, "HC");
    systemTest.checkSpeciesAtomType(&species, 15, "H1");
    systemTest.checkSpeciesAtomType(&species, 16, "H1");
    systemTest.checkSpeciesAtomType(&species, 17, "H1");
    systemTest.checkSpeciesAtomType(&species, 18, "H1");
    systemTest.checkSpeciesAtomType(&species, 19, "H1");
    systemTest.checkSpeciesAtomType(&species, 20, "C2");
    systemTest.checkSpeciesAtomType(&species, 21, "HC");
    systemTest.checkSpeciesAtomType(&species, 22, "HC");
    systemTest.checkSpeciesAtomType(&species, 23, "CT");
    systemTest.checkSpeciesAtomType(&species, 24, "HC");
    systemTest.checkSpeciesAtomType(&species, 25, "HC");
    systemTest.checkSpeciesAtomType(&species, 26, "HC");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, c4c1c1im)
{
    Species species("c4c1c1im");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/c4c1c1im.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 28);
    ASSERT_EQ(species.nAngles(), 51);
    ASSERT_EQ(species.nTorsions(), 65);
    ASSERT_EQ(species.nImpropers(), 5);

    systemTest.checkSpeciesAtomType(&species, 0, "NAM");
    systemTest.checkSpeciesAtomType(&species, 1, "CRM");
    systemTest.checkSpeciesAtomType(&species, 2, "NAM");
    systemTest.checkSpeciesAtomType(&species, 3, "CW");
    systemTest.checkSpeciesAtomType(&species, 4, "CW");
    systemTest.checkSpeciesAtomType(&species, 5, "C1");
    systemTest.checkSpeciesAtomType(&species, 6, "CCR");
    systemTest.checkSpeciesAtomType(&species, 7, "C1");
    systemTest.checkSpeciesAtomType(&species, 8, "HCW");
    systemTest.checkSpeciesAtomType(&species, 9, "HCW");
    systemTest.checkSpeciesAtomType(&species, 10, "H1");
    systemTest.checkSpeciesAtomType(&species, 11, "H1");
    systemTest.checkSpeciesAtomType(&species, 12, "H1");
    systemTest.checkSpeciesAtomType(&species, 13, "C2");
    systemTest.checkSpeciesAtomType(&species, 14, "H1");
    systemTest.checkSpeciesAtomType(&species, 15, "H1");
    systemTest.checkSpeciesAtomType(&species, 16, "CS");
    systemTest.checkSpeciesAtomType(&species, 17, "HC");
    systemTest.checkSpeciesAtomType(&species, 18, "HC");
    systemTest.checkSpeciesAtomType(&species, 19, "CT");
    systemTest.checkSpeciesAtomType(&species, 20, "HC");
    systemTest.checkSpeciesAtomType(&species, 21, "HC");
    systemTest.checkSpeciesAtomType(&species, 22, "HC");
    systemTest.checkSpeciesAtomType(&species, 23, "HC");
    systemTest.checkSpeciesAtomType(&species, 24, "HC");
    systemTest.checkSpeciesAtomType(&species, 25, "H1");
    systemTest.checkSpeciesAtomType(&species, 26, "H1");
    systemTest.checkSpeciesAtomType(&species, 27, "H1");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, c4c1im)
{
    Species species("c4c1im");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/c4c1im.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 25);
    ASSERT_EQ(species.nAngles(), 45);
    ASSERT_EQ(species.nTorsions(), 59);
    ASSERT_EQ(species.nImpropers(), 5);

    systemTest.checkSpeciesAtomType(&species, 0, "NA");
    systemTest.checkSpeciesAtomType(&species, 1, "CR");
    systemTest.checkSpeciesAtomType(&species, 2, "NA");
    systemTest.checkSpeciesAtomType(&species, 3, "CW");
    systemTest.checkSpeciesAtomType(&species, 4, "CW");
    systemTest.checkSpeciesAtomType(&species, 5, "C1");
    systemTest.checkSpeciesAtomType(&species, 6, "HCR");
    systemTest.checkSpeciesAtomType(&species, 7, "C1");
    systemTest.checkSpeciesAtomType(&species, 8, "HCW");
    systemTest.checkSpeciesAtomType(&species, 9, "HCW");
    systemTest.checkSpeciesAtomType(&species, 10, "H1");
    systemTest.checkSpeciesAtomType(&species, 11, "H1");
    systemTest.checkSpeciesAtomType(&species, 12, "H1");
    systemTest.checkSpeciesAtomType(&species, 13, "C2");
    systemTest.checkSpeciesAtomType(&species, 14, "H1");
    systemTest.checkSpeciesAtomType(&species, 15, "H1");
    systemTest.checkSpeciesAtomType(&species, 16, "CS");
    systemTest.checkSpeciesAtomType(&species, 17, "HC");
    systemTest.checkSpeciesAtomType(&species, 18, "HC");
    systemTest.checkSpeciesAtomType(&species, 19, "CT");
    systemTest.checkSpeciesAtomType(&species, 20, "HC");
    systemTest.checkSpeciesAtomType(&species, 21, "HC");
    systemTest.checkSpeciesAtomType(&species, 22, "HC");
    systemTest.checkSpeciesAtomType(&species, 23, "HC");
    systemTest.checkSpeciesAtomType(&species, 24, "HC");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, c4c1pyrr)
{
    Species species("c4c1pyrr");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/c4c1pyrr.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 30);
    ASSERT_EQ(species.nAngles(), 60);
    ASSERT_EQ(species.nTorsions(), 90);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "N4");
    systemTest.checkSpeciesAtomType(&species, 1, "C1");
    systemTest.checkSpeciesAtomType(&species, 2, "C1");
    systemTest.checkSpeciesAtomType(&species, 3, "C2");
    systemTest.checkSpeciesAtomType(&species, 4, "C2");
    systemTest.checkSpeciesAtomType(&species, 5, "C1");
    systemTest.checkSpeciesAtomType(&species, 6, "C1");
    systemTest.checkSpeciesAtomType(&species, 7, "H1");
    systemTest.checkSpeciesAtomType(&species, 8, "H1");
    systemTest.checkSpeciesAtomType(&species, 9, "H1");
    systemTest.checkSpeciesAtomType(&species, 10, "H1");
    systemTest.checkSpeciesAtomType(&species, 11, "HC");
    systemTest.checkSpeciesAtomType(&species, 12, "HC");
    systemTest.checkSpeciesAtomType(&species, 13, "HC");
    systemTest.checkSpeciesAtomType(&species, 14, "HC");
    systemTest.checkSpeciesAtomType(&species, 15, "H1");
    systemTest.checkSpeciesAtomType(&species, 16, "H1");
    systemTest.checkSpeciesAtomType(&species, 17, "H1");
    systemTest.checkSpeciesAtomType(&species, 18, "H1");
    systemTest.checkSpeciesAtomType(&species, 19, "H1");
    systemTest.checkSpeciesAtomType(&species, 20, "C2");
    systemTest.checkSpeciesAtomType(&species, 21, "HC");
    systemTest.checkSpeciesAtomType(&species, 22, "HC");
    systemTest.checkSpeciesAtomType(&species, 23, "CS");
    systemTest.checkSpeciesAtomType(&species, 24, "HC");
    systemTest.checkSpeciesAtomType(&species, 25, "HC");
    systemTest.checkSpeciesAtomType(&species, 26, "CT");
    systemTest.checkSpeciesAtomType(&species, 27, "HC");
    systemTest.checkSpeciesAtomType(&species, 28, "HC");
    systemTest.checkSpeciesAtomType(&species, 29, "HC");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, c4c4im)
{
    Species species("c4c4im");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/c4c4im.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 34);
    ASSERT_EQ(species.nAngles(), 63);
    ASSERT_EQ(species.nTorsions(), 86);
    ASSERT_EQ(species.nImpropers(), 5);

    systemTest.checkSpeciesAtomType(&species, 0, "NA");
    systemTest.checkSpeciesAtomType(&species, 1, "CR");
    systemTest.checkSpeciesAtomType(&species, 2, "NA");
    systemTest.checkSpeciesAtomType(&species, 3, "CW");
    systemTest.checkSpeciesAtomType(&species, 4, "CW");
    systemTest.checkSpeciesAtomType(&species, 5, "C1");
    systemTest.checkSpeciesAtomType(&species, 6, "HCR");
    systemTest.checkSpeciesAtomType(&species, 7, "C1");
    systemTest.checkSpeciesAtomType(&species, 8, "HCW");
    systemTest.checkSpeciesAtomType(&species, 9, "HCW");
    systemTest.checkSpeciesAtomType(&species, 10, "C2");
    systemTest.checkSpeciesAtomType(&species, 11, "H1");
    systemTest.checkSpeciesAtomType(&species, 12, "H1");
    systemTest.checkSpeciesAtomType(&species, 13, "CS");
    systemTest.checkSpeciesAtomType(&species, 14, "HC");
    systemTest.checkSpeciesAtomType(&species, 15, "HC");
    systemTest.checkSpeciesAtomType(&species, 16, "CT");
    systemTest.checkSpeciesAtomType(&species, 17, "HC");
    systemTest.checkSpeciesAtomType(&species, 18, "HC");
    systemTest.checkSpeciesAtomType(&species, 19, "HC");
    systemTest.checkSpeciesAtomType(&species, 20, "HC");
    systemTest.checkSpeciesAtomType(&species, 21, "HC");
    systemTest.checkSpeciesAtomType(&species, 22, "C2");
    systemTest.checkSpeciesAtomType(&species, 23, "H1");
    systemTest.checkSpeciesAtomType(&species, 24, "H1");
    systemTest.checkSpeciesAtomType(&species, 25, "CS");
    systemTest.checkSpeciesAtomType(&species, 26, "HC");
    systemTest.checkSpeciesAtomType(&species, 27, "HC");
    systemTest.checkSpeciesAtomType(&species, 28, "CT");
    systemTest.checkSpeciesAtomType(&species, 29, "HC");
    systemTest.checkSpeciesAtomType(&species, 30, "HC");
    systemTest.checkSpeciesAtomType(&species, 31, "HC");
    systemTest.checkSpeciesAtomType(&species, 32, "HC");
    systemTest.checkSpeciesAtomType(&species, 33, "HC");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, c4pyri)
{
    Species species("c4pyri");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/c4pyri.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 24);
    ASSERT_EQ(species.nAngles(), 42);
    ASSERT_EQ(species.nTorsions(), 57);
    ASSERT_EQ(species.nImpropers(), 6);

    systemTest.checkSpeciesAtomType(&species, 0, "NAP");
    systemTest.checkSpeciesAtomType(&species, 1, "CAPO");
    systemTest.checkSpeciesAtomType(&species, 2, "CAPM");
    systemTest.checkSpeciesAtomType(&species, 3, "CAPP");
    systemTest.checkSpeciesAtomType(&species, 4, "CAPM");
    systemTest.checkSpeciesAtomType(&species, 5, "CAPO");
    systemTest.checkSpeciesAtomType(&species, 6, "C1");
    systemTest.checkSpeciesAtomType(&species, 7, "HAP");
    systemTest.checkSpeciesAtomType(&species, 8, "HAP");
    systemTest.checkSpeciesAtomType(&species, 9, "HAP");
    systemTest.checkSpeciesAtomType(&species, 10, "HAP");
    systemTest.checkSpeciesAtomType(&species, 11, "HAP");
    systemTest.checkSpeciesAtomType(&species, 12, "C2");
    systemTest.checkSpeciesAtomType(&species, 13, "H1");
    systemTest.checkSpeciesAtomType(&species, 14, "H1");
    systemTest.checkSpeciesAtomType(&species, 15, "CS");
    systemTest.checkSpeciesAtomType(&species, 16, "HC");
    systemTest.checkSpeciesAtomType(&species, 17, "HC");
    systemTest.checkSpeciesAtomType(&species, 18, "CT");
    systemTest.checkSpeciesAtomType(&species, 19, "HC");
    systemTest.checkSpeciesAtomType(&species, 20, "HC");
    systemTest.checkSpeciesAtomType(&species, 21, "HC");
    systemTest.checkSpeciesAtomType(&species, 22, "HC");
    systemTest.checkSpeciesAtomType(&species, 23, "HC");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, c6c1im)
{
    Species species("c6c1im");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/c6c1im.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 31);
    ASSERT_EQ(species.nAngles(), 57);
    ASSERT_EQ(species.nTorsions(), 77);
    ASSERT_EQ(species.nImpropers(), 5);

    systemTest.checkSpeciesAtomType(&species, 0, "NA");
    systemTest.checkSpeciesAtomType(&species, 1, "CR");
    systemTest.checkSpeciesAtomType(&species, 2, "NA");
    systemTest.checkSpeciesAtomType(&species, 3, "CW");
    systemTest.checkSpeciesAtomType(&species, 4, "CW");
    systemTest.checkSpeciesAtomType(&species, 5, "C1");
    systemTest.checkSpeciesAtomType(&species, 6, "HCR");
    systemTest.checkSpeciesAtomType(&species, 7, "C1");
    systemTest.checkSpeciesAtomType(&species, 8, "HCW");
    systemTest.checkSpeciesAtomType(&species, 9, "HCW");
    systemTest.checkSpeciesAtomType(&species, 10, "H1");
    systemTest.checkSpeciesAtomType(&species, 11, "H1");
    systemTest.checkSpeciesAtomType(&species, 12, "H1");
    systemTest.checkSpeciesAtomType(&species, 13, "C2");
    systemTest.checkSpeciesAtomType(&species, 14, "H1");
    systemTest.checkSpeciesAtomType(&species, 15, "H1");
    systemTest.checkSpeciesAtomType(&species, 16, "CS");
    systemTest.checkSpeciesAtomType(&species, 17, "HC");
    systemTest.checkSpeciesAtomType(&species, 18, "HC");
    systemTest.checkSpeciesAtomType(&species, 19, "CS");
    systemTest.checkSpeciesAtomType(&species, 20, "HC");
    systemTest.checkSpeciesAtomType(&species, 21, "HC");
    systemTest.checkSpeciesAtomType(&species, 22, "CS");
    systemTest.checkSpeciesAtomType(&species, 23, "HC");
    systemTest.checkSpeciesAtomType(&species, 24, "HC");
    systemTest.checkSpeciesAtomType(&species, 25, "CT");
    systemTest.checkSpeciesAtomType(&species, 26, "HC");
    systemTest.checkSpeciesAtomType(&species, 27, "HC");
    systemTest.checkSpeciesAtomType(&species, 28, "HC");
    systemTest.checkSpeciesAtomType(&species, 29, "HC");
    systemTest.checkSpeciesAtomType(&species, 30, "HC");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, c8c1im)
{
    Species species("c8c1im");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/c8c1im.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 37);
    ASSERT_EQ(species.nAngles(), 69);
    ASSERT_EQ(species.nTorsions(), 95);
    ASSERT_EQ(species.nImpropers(), 5);

    systemTest.checkSpeciesAtomType(&species, 0, "NA");
    systemTest.checkSpeciesAtomType(&species, 1, "CR");
    systemTest.checkSpeciesAtomType(&species, 2, "NA");
    systemTest.checkSpeciesAtomType(&species, 3, "CW");
    systemTest.checkSpeciesAtomType(&species, 4, "CW");
    systemTest.checkSpeciesAtomType(&species, 5, "C1");
    systemTest.checkSpeciesAtomType(&species, 6, "HCR");
    systemTest.checkSpeciesAtomType(&species, 7, "C1");
    systemTest.checkSpeciesAtomType(&species, 8, "HCW");
    systemTest.checkSpeciesAtomType(&species, 9, "HCW");
    systemTest.checkSpeciesAtomType(&species, 10, "H1");
    systemTest.checkSpeciesAtomType(&species, 11, "H1");
    systemTest.checkSpeciesAtomType(&species, 12, "H1");
    systemTest.checkSpeciesAtomType(&species, 13, "C2");
    systemTest.checkSpeciesAtomType(&species, 14, "H1");
    systemTest.checkSpeciesAtomType(&species, 15, "H1");
    systemTest.checkSpeciesAtomType(&species, 16, "CS");
    systemTest.checkSpeciesAtomType(&species, 17, "HC");
    systemTest.checkSpeciesAtomType(&species, 18, "HC");
    systemTest.checkSpeciesAtomType(&species, 19, "CS");
    systemTest.checkSpeciesAtomType(&species, 20, "HC");
    systemTest.checkSpeciesAtomType(&species, 21, "HC");
    systemTest.checkSpeciesAtomType(&species, 22, "CS");
    systemTest.checkSpeciesAtomType(&species, 23, "HC");
    systemTest.checkSpeciesAtomType(&species, 24, "HC");
    systemTest.checkSpeciesAtomType(&species, 25, "CS");
    systemTest.checkSpeciesAtomType(&species, 26, "HC");
    systemTest.checkSpeciesAtomType(&species, 27, "HC");
    systemTest.checkSpeciesAtomType(&species, 28, "CS");
    systemTest.checkSpeciesAtomType(&species, 29, "HC");
    systemTest.checkSpeciesAtomType(&species, 30, "HC");
    systemTest.checkSpeciesAtomType(&species, 31, "CT");
    systemTest.checkSpeciesAtomType(&species, 32, "HC");
    systemTest.checkSpeciesAtomType(&species, 33, "HC");
    systemTest.checkSpeciesAtomType(&species, 34, "HC");
    systemTest.checkSpeciesAtomType(&species, 35, "HC");
    systemTest.checkSpeciesAtomType(&species, 36, "HC");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, c8fc1im)
{
    Species species("c8fc1im");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/c8fc1im.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 37);
    ASSERT_EQ(species.nAngles(), 69);
    ASSERT_EQ(species.nTorsions(), 95);
    ASSERT_EQ(species.nImpropers(), 5);

    systemTest.checkSpeciesAtomType(&species, 0, "NA");
    systemTest.checkSpeciesAtomType(&species, 1, "CR");
    systemTest.checkSpeciesAtomType(&species, 2, "NA");
    systemTest.checkSpeciesAtomType(&species, 3, "CW");
    systemTest.checkSpeciesAtomType(&species, 4, "CW");
    systemTest.checkSpeciesAtomType(&species, 5, "C1");
    systemTest.checkSpeciesAtomType(&species, 6, "HCR");
    systemTest.checkSpeciesAtomType(&species, 7, "C1H");
    systemTest.checkSpeciesAtomType(&species, 8, "HCW");
    systemTest.checkSpeciesAtomType(&species, 9, "HCW");
    systemTest.checkSpeciesAtomType(&species, 10, "H1");
    systemTest.checkSpeciesAtomType(&species, 11, "H1");
    systemTest.checkSpeciesAtomType(&species, 12, "H1");
    systemTest.checkSpeciesAtomType(&species, 13, "C2");
    systemTest.checkSpeciesAtomType(&species, 14, "H1");
    systemTest.checkSpeciesAtomType(&species, 15, "H1");
    systemTest.checkSpeciesAtomType(&species, 16, "CFH");
    systemTest.checkSpeciesAtomType(&species, 17, "HC");
    systemTest.checkSpeciesAtomType(&species, 18, "HC");
    systemTest.checkSpeciesAtomType(&species, 19, "CSF");
    systemTest.checkSpeciesAtomType(&species, 20, "F");
    systemTest.checkSpeciesAtomType(&species, 21, "F");
    systemTest.checkSpeciesAtomType(&species, 22, "CSF");
    systemTest.checkSpeciesAtomType(&species, 23, "F");
    systemTest.checkSpeciesAtomType(&species, 24, "F");
    systemTest.checkSpeciesAtomType(&species, 25, "CSF");
    systemTest.checkSpeciesAtomType(&species, 26, "F");
    systemTest.checkSpeciesAtomType(&species, 27, "F");
    systemTest.checkSpeciesAtomType(&species, 28, "CSF");
    systemTest.checkSpeciesAtomType(&species, 29, "F");
    systemTest.checkSpeciesAtomType(&species, 30, "F");
    systemTest.checkSpeciesAtomType(&species, 31, "CTF");
    systemTest.checkSpeciesAtomType(&species, 32, "F");
    systemTest.checkSpeciesAtomType(&species, 33, "F");
    systemTest.checkSpeciesAtomType(&species, 34, "F");
    systemTest.checkSpeciesAtomType(&species, 35, "F");
    systemTest.checkSpeciesAtomType(&species, 36, "F");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, c8isoqui)
{
    Species species("c8isoqui");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/c8isoqui.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 43);
    ASSERT_EQ(species.nAngles(), 78);
    ASSERT_EQ(species.nTorsions(), 113);
    ASSERT_EQ(species.nImpropers(), 10);

    systemTest.checkSpeciesAtomType(&species, 0, "NAQ");
    systemTest.checkSpeciesAtomType(&species, 1, "CA");
    systemTest.checkSpeciesAtomType(&species, 2, "CAQ");
    systemTest.checkSpeciesAtomType(&species, 3, "CA");
    systemTest.checkSpeciesAtomType(&species, 4, "CA");
    systemTest.checkSpeciesAtomType(&species, 5, "CA");
    systemTest.checkSpeciesAtomType(&species, 6, "CA");
    systemTest.checkSpeciesAtomType(&species, 7, "CAQ");
    systemTest.checkSpeciesAtomType(&species, 8, "CA");
    systemTest.checkSpeciesAtomType(&species, 9, "CA");
    systemTest.checkSpeciesAtomType(&species, 10, "HAP");
    systemTest.checkSpeciesAtomType(&species, 11, "HA");
    systemTest.checkSpeciesAtomType(&species, 12, "HA");
    systemTest.checkSpeciesAtomType(&species, 13, "HA");
    systemTest.checkSpeciesAtomType(&species, 14, "HA");
    systemTest.checkSpeciesAtomType(&species, 15, "HAP");
    systemTest.checkSpeciesAtomType(&species, 16, "HAP");
    systemTest.checkSpeciesAtomType(&species, 17, "C1");
    systemTest.checkSpeciesAtomType(&species, 18, "H1");
    systemTest.checkSpeciesAtomType(&species, 19, "H1");
    systemTest.checkSpeciesAtomType(&species, 20, "C2");
    systemTest.checkSpeciesAtomType(&species, 21, "CS");
    systemTest.checkSpeciesAtomType(&species, 22, "CS");
    systemTest.checkSpeciesAtomType(&species, 23, "CS");
    systemTest.checkSpeciesAtomType(&species, 24, "CS");
    systemTest.checkSpeciesAtomType(&species, 25, "CS");
    systemTest.checkSpeciesAtomType(&species, 26, "CT");
    systemTest.checkSpeciesAtomType(&species, 27, "HC");
    systemTest.checkSpeciesAtomType(&species, 28, "HC");
    systemTest.checkSpeciesAtomType(&species, 29, "HC");
    systemTest.checkSpeciesAtomType(&species, 30, "HC");
    systemTest.checkSpeciesAtomType(&species, 31, "HC");
    systemTest.checkSpeciesAtomType(&species, 32, "HC");
    systemTest.checkSpeciesAtomType(&species, 33, "HC");
    systemTest.checkSpeciesAtomType(&species, 34, "HC");
    systemTest.checkSpeciesAtomType(&species, 35, "HC");
    systemTest.checkSpeciesAtomType(&species, 36, "HC");
    systemTest.checkSpeciesAtomType(&species, 37, "HC");
    systemTest.checkSpeciesAtomType(&species, 38, "HC");
    systemTest.checkSpeciesAtomType(&species, 39, "HC");
    systemTest.checkSpeciesAtomType(&species, 40, "HC");
    systemTest.checkSpeciesAtomType(&species, 41, "HC");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, cholinium)
{
    Species species("cholinium");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/cholinium.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 20);
    ASSERT_EQ(species.nAngles(), 37);
    ASSERT_EQ(species.nTorsions(), 48);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "N4");
    systemTest.checkSpeciesAtomType(&species, 1, "C1");
    systemTest.checkSpeciesAtomType(&species, 2, "C1");
    systemTest.checkSpeciesAtomType(&species, 3, "C1");
    systemTest.checkSpeciesAtomType(&species, 4, "H1");
    systemTest.checkSpeciesAtomType(&species, 5, "H1");
    systemTest.checkSpeciesAtomType(&species, 6, "H1");
    systemTest.checkSpeciesAtomType(&species, 7, "H1");
    systemTest.checkSpeciesAtomType(&species, 8, "H1");
    systemTest.checkSpeciesAtomType(&species, 9, "H1");
    systemTest.checkSpeciesAtomType(&species, 10, "H1");
    systemTest.checkSpeciesAtomType(&species, 11, "H1");
    systemTest.checkSpeciesAtomType(&species, 12, "H1");
    systemTest.checkSpeciesAtomType(&species, 13, "C1");
    systemTest.checkSpeciesAtomType(&species, 14, "H1");
    systemTest.checkSpeciesAtomType(&species, 15, "COL");
    systemTest.checkSpeciesAtomType(&species, 16, "H1");
    systemTest.checkSpeciesAtomType(&species, 17, "HC");
    systemTest.checkSpeciesAtomType(&species, 18, "HC");
    systemTest.checkSpeciesAtomType(&species, 19, "OH");
    systemTest.checkSpeciesAtomType(&species, 20, "HO");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, gua)
{
    Species species("Gua");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/gua.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 9);
    ASSERT_EQ(species.nAngles(), 12);
    ASSERT_EQ(species.nTorsions(), 12);
    ASSERT_EQ(species.nImpropers(), 4);

    systemTest.checkSpeciesAtomType(&species, 0, "CG");
    systemTest.checkSpeciesAtomType(&species, 1, "NG");
    systemTest.checkSpeciesAtomType(&species, 2, "NG");
    systemTest.checkSpeciesAtomType(&species, 3, "NG");
    systemTest.checkSpeciesAtomType(&species, 4, "HG");
    systemTest.checkSpeciesAtomType(&species, 5, "HG");
    systemTest.checkSpeciesAtomType(&species, 6, "HG");
    systemTest.checkSpeciesAtomType(&species, 7, "HG");
    systemTest.checkSpeciesAtomType(&species, 8, "HG");
    systemTest.checkSpeciesAtomType(&species, 9, "HG");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, N1110)
{
    Species species("N1110");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/n1110.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 13);
    ASSERT_EQ(species.nAngles(), 24);
    ASSERT_EQ(species.nTorsions(), 27);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "N3");
    systemTest.checkSpeciesAtomType(&species, 1, "C1");
    systemTest.checkSpeciesAtomType(&species, 2, "C1");
    systemTest.checkSpeciesAtomType(&species, 3, "C1");
    systemTest.checkSpeciesAtomType(&species, 4, "H3");
    systemTest.checkSpeciesAtomType(&species, 5, "H1");
    systemTest.checkSpeciesAtomType(&species, 6, "H1");
    systemTest.checkSpeciesAtomType(&species, 7, "H1");
    systemTest.checkSpeciesAtomType(&species, 8, "H1");
    systemTest.checkSpeciesAtomType(&species, 9, "H1");
    systemTest.checkSpeciesAtomType(&species, 10, "H1");
    systemTest.checkSpeciesAtomType(&species, 11, "H1");
    systemTest.checkSpeciesAtomType(&species, 12, "H1");
    systemTest.checkSpeciesAtomType(&species, 13, "H1");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, N1111)
{
    Species species("N1111");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/n1111.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 16);
    ASSERT_EQ(species.nAngles(), 30);
    ASSERT_EQ(species.nTorsions(), 36);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "N4");
    systemTest.checkSpeciesAtomType(&species, 1, "C1");
    systemTest.checkSpeciesAtomType(&species, 2, "C1");
    systemTest.checkSpeciesAtomType(&species, 3, "C1");
    systemTest.checkSpeciesAtomType(&species, 4, "C1");
    systemTest.checkSpeciesAtomType(&species, 5, "H1");
    systemTest.checkSpeciesAtomType(&species, 6, "H1");
    systemTest.checkSpeciesAtomType(&species, 7, "H1");
    systemTest.checkSpeciesAtomType(&species, 8, "H1");
    systemTest.checkSpeciesAtomType(&species, 9, "H1");
    systemTest.checkSpeciesAtomType(&species, 10, "H1");
    systemTest.checkSpeciesAtomType(&species, 11, "H1");
    systemTest.checkSpeciesAtomType(&species, 12, "H1");
    systemTest.checkSpeciesAtomType(&species, 13, "H1");
    systemTest.checkSpeciesAtomType(&species, 14, "H1");
    systemTest.checkSpeciesAtomType(&species, 15, "H1");
    systemTest.checkSpeciesAtomType(&species, 16, "H1");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, N2220)
{
    Species species("N2220");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/n2220.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 22);
    ASSERT_EQ(species.nAngles(), 42);
    ASSERT_EQ(species.nTorsions(), 54);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "N3");
    systemTest.checkSpeciesAtomType(&species, 1, "C1");
    systemTest.checkSpeciesAtomType(&species, 2, "C1");
    systemTest.checkSpeciesAtomType(&species, 3, "C1");
    systemTest.checkSpeciesAtomType(&species, 4, "CE");
    systemTest.checkSpeciesAtomType(&species, 5, "H1");
    systemTest.checkSpeciesAtomType(&species, 6, "H1");
    systemTest.checkSpeciesAtomType(&species, 7, "HC");
    systemTest.checkSpeciesAtomType(&species, 8, "HC");
    systemTest.checkSpeciesAtomType(&species, 9, "HC");
    systemTest.checkSpeciesAtomType(&species, 10, "H1");
    systemTest.checkSpeciesAtomType(&species, 11, "H1");
    systemTest.checkSpeciesAtomType(&species, 12, "CE");
    systemTest.checkSpeciesAtomType(&species, 13, "HC");
    systemTest.checkSpeciesAtomType(&species, 14, "HC");
    systemTest.checkSpeciesAtomType(&species, 15, "HC");
    systemTest.checkSpeciesAtomType(&species, 16, "H1");
    systemTest.checkSpeciesAtomType(&species, 17, "H1");
    systemTest.checkSpeciesAtomType(&species, 18, "CE");
    systemTest.checkSpeciesAtomType(&species, 19, "HC");
    systemTest.checkSpeciesAtomType(&species, 20, "HC");
    systemTest.checkSpeciesAtomType(&species, 21, "HC");
    systemTest.checkSpeciesAtomType(&species, 22, "H3");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, N2222)
{
    Species species("N2222");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/n2222.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 28);
    ASSERT_EQ(species.nAngles(), 54);
    ASSERT_EQ(species.nTorsions(), 72);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "N4");
    systemTest.checkSpeciesAtomType(&species, 1, "C1");
    systemTest.checkSpeciesAtomType(&species, 2, "C1");
    systemTest.checkSpeciesAtomType(&species, 3, "C1");
    systemTest.checkSpeciesAtomType(&species, 4, "C1");
    systemTest.checkSpeciesAtomType(&species, 5, "H1");
    systemTest.checkSpeciesAtomType(&species, 6, "H1");
    systemTest.checkSpeciesAtomType(&species, 7, "H1");
    systemTest.checkSpeciesAtomType(&species, 8, "H1");
    systemTest.checkSpeciesAtomType(&species, 9, "H1");
    systemTest.checkSpeciesAtomType(&species, 10, "H1");
    systemTest.checkSpeciesAtomType(&species, 11, "H1");
    systemTest.checkSpeciesAtomType(&species, 12, "H1");
    systemTest.checkSpeciesAtomType(&species, 13, "CE");
    systemTest.checkSpeciesAtomType(&species, 14, "CE");
    systemTest.checkSpeciesAtomType(&species, 15, "CE");
    systemTest.checkSpeciesAtomType(&species, 16, "CE");
    systemTest.checkSpeciesAtomType(&species, 17, "HC");
    systemTest.checkSpeciesAtomType(&species, 18, "HC");
    systemTest.checkSpeciesAtomType(&species, 19, "HC");
    systemTest.checkSpeciesAtomType(&species, 20, "HC");
    systemTest.checkSpeciesAtomType(&species, 21, "HC");
    systemTest.checkSpeciesAtomType(&species, 22, "HC");
    systemTest.checkSpeciesAtomType(&species, 23, "HC");
    systemTest.checkSpeciesAtomType(&species, 24, "HC");
    systemTest.checkSpeciesAtomType(&species, 25, "HC");
    systemTest.checkSpeciesAtomType(&species, 26, "HC");
    systemTest.checkSpeciesAtomType(&species, 27, "HC");
    systemTest.checkSpeciesAtomType(&species, 28, "HC");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, N4444)
{
    Species species("N4444");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/n4444.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 52);
    ASSERT_EQ(species.nAngles(), 102);
    ASSERT_EQ(species.nTorsions(), 144);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "N4");
    systemTest.checkSpeciesAtomType(&species, 1, "C1");
    systemTest.checkSpeciesAtomType(&species, 2, "C1");
    systemTest.checkSpeciesAtomType(&species, 3, "C1");
    systemTest.checkSpeciesAtomType(&species, 4, "C1");
    systemTest.checkSpeciesAtomType(&species, 5, "H1");
    systemTest.checkSpeciesAtomType(&species, 6, "H1");
    systemTest.checkSpeciesAtomType(&species, 7, "H1");
    systemTest.checkSpeciesAtomType(&species, 8, "H1");
    systemTest.checkSpeciesAtomType(&species, 9, "H1");
    systemTest.checkSpeciesAtomType(&species, 10, "H1");
    systemTest.checkSpeciesAtomType(&species, 11, "H1");
    systemTest.checkSpeciesAtomType(&species, 12, "H1");
    systemTest.checkSpeciesAtomType(&species, 13, "C2");
    systemTest.checkSpeciesAtomType(&species, 14, "C2");
    systemTest.checkSpeciesAtomType(&species, 15, "C2");
    systemTest.checkSpeciesAtomType(&species, 16, "C2");
    systemTest.checkSpeciesAtomType(&species, 17, "HC");
    systemTest.checkSpeciesAtomType(&species, 18, "HC");
    systemTest.checkSpeciesAtomType(&species, 19, "HC");
    systemTest.checkSpeciesAtomType(&species, 20, "HC");
    systemTest.checkSpeciesAtomType(&species, 21, "HC");
    systemTest.checkSpeciesAtomType(&species, 22, "HC");
    systemTest.checkSpeciesAtomType(&species, 23, "HC");
    systemTest.checkSpeciesAtomType(&species, 24, "HC");
    systemTest.checkSpeciesAtomType(&species, 25, "CS");
    systemTest.checkSpeciesAtomType(&species, 26, "CS");
    systemTest.checkSpeciesAtomType(&species, 27, "CS");
    systemTest.checkSpeciesAtomType(&species, 28, "CS");
    systemTest.checkSpeciesAtomType(&species, 29, "CT");
    systemTest.checkSpeciesAtomType(&species, 30, "CT");
    systemTest.checkSpeciesAtomType(&species, 31, "CT");
    systemTest.checkSpeciesAtomType(&species, 32, "CT");
    systemTest.checkSpeciesAtomType(&species, 33, "HC");
    systemTest.checkSpeciesAtomType(&species, 34, "HC");
    systemTest.checkSpeciesAtomType(&species, 35, "HC");
    systemTest.checkSpeciesAtomType(&species, 36, "HC");
    systemTest.checkSpeciesAtomType(&species, 37, "HC");
    systemTest.checkSpeciesAtomType(&species, 38, "HC");
    systemTest.checkSpeciesAtomType(&species, 39, "HC");
    systemTest.checkSpeciesAtomType(&species, 40, "HC");
    systemTest.checkSpeciesAtomType(&species, 41, "HC");
    systemTest.checkSpeciesAtomType(&species, 42, "HC");
    systemTest.checkSpeciesAtomType(&species, 43, "HC");
    systemTest.checkSpeciesAtomType(&species, 44, "HC");
    systemTest.checkSpeciesAtomType(&species, 45, "HC");
    systemTest.checkSpeciesAtomType(&species, 46, "HC");
    systemTest.checkSpeciesAtomType(&species, 47, "HC");
    systemTest.checkSpeciesAtomType(&species, 48, "HC");
    systemTest.checkSpeciesAtomType(&species, 49, "HC");
    systemTest.checkSpeciesAtomType(&species, 50, "HC");
    systemTest.checkSpeciesAtomType(&species, 51, "HC");
    systemTest.checkSpeciesAtomType(&species, 52, "HC");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsForcefieldTest, P66614)
{
    Species species("P66614");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/p66614.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Cations"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 100);
    ASSERT_EQ(species.nAngles(), 198);
    ASSERT_EQ(species.nTorsions(), 288);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "P4");
    systemTest.checkSpeciesAtomType(&species, 1, "C1P");
    systemTest.checkSpeciesAtomType(&species, 2, "C1P");
    systemTest.checkSpeciesAtomType(&species, 3, "C1P");
    systemTest.checkSpeciesAtomType(&species, 4, "C1P");
    systemTest.checkSpeciesAtomType(&species, 5, "C2");
    systemTest.checkSpeciesAtomType(&species, 6, "H1");
    systemTest.checkSpeciesAtomType(&species, 7, "H1");
    systemTest.checkSpeciesAtomType(&species, 8, "H1");
    systemTest.checkSpeciesAtomType(&species, 9, "H1");
    systemTest.checkSpeciesAtomType(&species, 10, "C2");
    systemTest.checkSpeciesAtomType(&species, 11, "C2");
    systemTest.checkSpeciesAtomType(&species, 12, "H1");
    systemTest.checkSpeciesAtomType(&species, 13, "H1");
    systemTest.checkSpeciesAtomType(&species, 14, "H1");
    systemTest.checkSpeciesAtomType(&species, 15, "H1");
    systemTest.checkSpeciesAtomType(&species, 16, "C2");
    systemTest.checkSpeciesAtomType(&species, 17, "CS");
    systemTest.checkSpeciesAtomType(&species, 18, "HC");
    systemTest.checkSpeciesAtomType(&species, 19, "HC");
    systemTest.checkSpeciesAtomType(&species, 20, "CS");
    systemTest.checkSpeciesAtomType(&species, 21, "HC");
    systemTest.checkSpeciesAtomType(&species, 22, "HC");
    systemTest.checkSpeciesAtomType(&species, 23, "CS");
    systemTest.checkSpeciesAtomType(&species, 24, "HC");
    systemTest.checkSpeciesAtomType(&species, 25, "HC");
    systemTest.checkSpeciesAtomType(&species, 26, "CT");
    systemTest.checkSpeciesAtomType(&species, 27, "HC");
    systemTest.checkSpeciesAtomType(&species, 28, "HC");
    systemTest.checkSpeciesAtomType(&species, 29, "HC");
    systemTest.checkSpeciesAtomType(&species, 30, "HC");
    systemTest.checkSpeciesAtomType(&species, 31, "HC");
    systemTest.checkSpeciesAtomType(&species, 32, "CS");
    systemTest.checkSpeciesAtomType(&species, 33, "HC");
    systemTest.checkSpeciesAtomType(&species, 34, "HC");
    systemTest.checkSpeciesAtomType(&species, 35, "CS");
    systemTest.checkSpeciesAtomType(&species, 36, "HC");
    systemTest.checkSpeciesAtomType(&species, 37, "HC");
    systemTest.checkSpeciesAtomType(&species, 38, "CS");
    systemTest.checkSpeciesAtomType(&species, 39, "HC");
    systemTest.checkSpeciesAtomType(&species, 40, "HC");
    systemTest.checkSpeciesAtomType(&species, 41, "CT");
    systemTest.checkSpeciesAtomType(&species, 42, "HC");
    systemTest.checkSpeciesAtomType(&species, 43, "HC");
    systemTest.checkSpeciesAtomType(&species, 44, "HC");
    systemTest.checkSpeciesAtomType(&species, 45, "HC");
    systemTest.checkSpeciesAtomType(&species, 46, "HC");
    systemTest.checkSpeciesAtomType(&species, 47, "CS");
    systemTest.checkSpeciesAtomType(&species, 48, "HC");
    systemTest.checkSpeciesAtomType(&species, 49, "HC");
    systemTest.checkSpeciesAtomType(&species, 50, "CS");
    systemTest.checkSpeciesAtomType(&species, 51, "HC");
    systemTest.checkSpeciesAtomType(&species, 52, "HC");
    systemTest.checkSpeciesAtomType(&species, 53, "CS");
    systemTest.checkSpeciesAtomType(&species, 54, "HC");
    systemTest.checkSpeciesAtomType(&species, 55, "HC");
    systemTest.checkSpeciesAtomType(&species, 56, "CT");
    systemTest.checkSpeciesAtomType(&species, 57, "HC");
    systemTest.checkSpeciesAtomType(&species, 58, "HC");
    systemTest.checkSpeciesAtomType(&species, 59, "HC");
    systemTest.checkSpeciesAtomType(&species, 60, "HC");
    systemTest.checkSpeciesAtomType(&species, 61, "HC");
    systemTest.checkSpeciesAtomType(&species, 62, "CS");
    systemTest.checkSpeciesAtomType(&species, 63, "HC");
    systemTest.checkSpeciesAtomType(&species, 64, "HC");
    systemTest.checkSpeciesAtomType(&species, 65, "CS");
    systemTest.checkSpeciesAtomType(&species, 66, "HC");
    systemTest.checkSpeciesAtomType(&species, 67, "HC");
    systemTest.checkSpeciesAtomType(&species, 68, "CS");
    systemTest.checkSpeciesAtomType(&species, 69, "HC");
    systemTest.checkSpeciesAtomType(&species, 70, "HC");
    systemTest.checkSpeciesAtomType(&species, 71, "CS");
    systemTest.checkSpeciesAtomType(&species, 72, "HC");
    systemTest.checkSpeciesAtomType(&species, 73, "HC");
    systemTest.checkSpeciesAtomType(&species, 74, "CS");
    systemTest.checkSpeciesAtomType(&species, 75, "HC");
    systemTest.checkSpeciesAtomType(&species, 76, "HC");
    systemTest.checkSpeciesAtomType(&species, 77, "CS");
    systemTest.checkSpeciesAtomType(&species, 78, "HC");
    systemTest.checkSpeciesAtomType(&species, 79, "HC");
    systemTest.checkSpeciesAtomType(&species, 80, "CS");
    systemTest.checkSpeciesAtomType(&species, 81, "HC");
    systemTest.checkSpeciesAtomType(&species, 82, "HC");
    systemTest.checkSpeciesAtomType(&species, 83, "CS");
    systemTest.checkSpeciesAtomType(&species, 84, "HC");
    systemTest.checkSpeciesAtomType(&species, 85, "HC");
    systemTest.checkSpeciesAtomType(&species, 86, "CS");
    systemTest.checkSpeciesAtomType(&species, 87, "HC");
    systemTest.checkSpeciesAtomType(&species, 88, "HC");
    systemTest.checkSpeciesAtomType(&species, 89, "CS");
    systemTest.checkSpeciesAtomType(&species, 90, "HC");
    systemTest.checkSpeciesAtomType(&species, 91, "HC");
    systemTest.checkSpeciesAtomType(&species, 92, "CS");
    systemTest.checkSpeciesAtomType(&species, 93, "HC");
    systemTest.checkSpeciesAtomType(&species, 94, "HC");
    systemTest.checkSpeciesAtomType(&species, 95, "CT");
    systemTest.checkSpeciesAtomType(&species, 96, "HC");
    systemTest.checkSpeciesAtomType(&species, 97, "HC");
    systemTest.checkSpeciesAtomType(&species, 98, "HC");
    systemTest.checkSpeciesAtomType(&species, 99, "HC");
    systemTest.checkSpeciesAtomType(&species, 100, "HC");
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}
}; // namespace UnitTest
