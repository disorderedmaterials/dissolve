// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/library.h"
#include "io/import/species.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(PCL2019AnionsAssignmentTest, beti)
{
    Species species("Hydronium");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/beti.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Anions"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 20);
    ASSERT_EQ(species.nAngles(), 37);
    ASSERT_EQ(species.nTorsions(), 42);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "C1F");
    systemTest.checkSpeciesAtomType(&species, 1, "CEF");
    systemTest.checkSpeciesAtomType(&species, 2, "F1");
    systemTest.checkSpeciesAtomType(&species, 3, "F1");
    systemTest.checkSpeciesAtomType(&species, 4, "SBT");
    systemTest.checkSpeciesAtomType(&species, 5, "NBT");
    systemTest.checkSpeciesAtomType(&species, 6, "OBT");
    systemTest.checkSpeciesAtomType(&species, 7, "OBT");
    systemTest.checkSpeciesAtomType(&species, 8, "SBT");
    systemTest.checkSpeciesAtomType(&species, 9, "OBT");
    systemTest.checkSpeciesAtomType(&species, 10, "OBT");
    systemTest.checkSpeciesAtomType(&species, 11, "C1F");
    systemTest.checkSpeciesAtomType(&species, 12, "CEF");
    systemTest.checkSpeciesAtomType(&species, 13, "F1");
    systemTest.checkSpeciesAtomType(&species, 14, "F1");
    systemTest.checkSpeciesAtomType(&species, 15, "F");
    systemTest.checkSpeciesAtomType(&species, 16, "F");
    systemTest.checkSpeciesAtomType(&species, 17, "F");
    systemTest.checkSpeciesAtomType(&species, 18, "F");
    systemTest.checkSpeciesAtomType(&species, 19, "F");
    systemTest.checkSpeciesAtomType(&species, 20, "F");
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, BF4)
{
    Species species("BF4");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/bf4.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Anions"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 4);
    ASSERT_EQ(species.nAngles(), 6);
    ASSERT_EQ(species.nTorsions(), 0);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "B");
    systemTest.checkSpeciesAtomType(&species, 1, "FB");
    systemTest.checkSpeciesAtomType(&species, 2, "FB");
    systemTest.checkSpeciesAtomType(&species, 3, "FB");
    systemTest.checkSpeciesAtomType(&species, 4, "FB");
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, c1SO3)
{
    Species species("C1SO3");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/c1so3.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Anions"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 7);
    ASSERT_EQ(species.nAngles(), 12);
    ASSERT_EQ(species.nTorsions(), 9);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "SO");
    systemTest.checkSpeciesAtomType(&species, 1, "OS3");
    systemTest.checkSpeciesAtomType(&species, 2, "OS3");
    systemTest.checkSpeciesAtomType(&species, 3, "OS3");
    systemTest.checkSpeciesAtomType(&species, 4, "CS3");
    systemTest.checkSpeciesAtomType(&species, 5, "HS3");
    systemTest.checkSpeciesAtomType(&species, 6, "HS3");
    systemTest.checkSpeciesAtomType(&species, 7, "HS3");
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, c1SO4)
{
    Species species("C1SO4");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/c1so4.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Anions"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 8);
    ASSERT_EQ(species.nAngles(), 13);
    ASSERT_EQ(species.nTorsions(), 6);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "HS4");
    systemTest.checkSpeciesAtomType(&species, 1, "CS4");
    systemTest.checkSpeciesAtomType(&species, 2, "OC4");
    systemTest.checkSpeciesAtomType(&species, 3, "HS4");
    systemTest.checkSpeciesAtomType(&species, 4, "HS4");
    systemTest.checkSpeciesAtomType(&species, 5, "SO");
    systemTest.checkSpeciesAtomType(&species, 6, "OS4");
    systemTest.checkSpeciesAtomType(&species, 7, "OS4");
    systemTest.checkSpeciesAtomType(&species, 8, "OS4");
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, c2SO3)
{
    Species species("C2SO3");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/c2so3.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Anions"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 10);
    ASSERT_EQ(species.nAngles(), 18);
    ASSERT_EQ(species.nTorsions(), 18);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "SO");
    systemTest.checkSpeciesAtomType(&species, 1, "OS3");
    systemTest.checkSpeciesAtomType(&species, 2, "OS3");
    systemTest.checkSpeciesAtomType(&species, 3, "OS3");
    systemTest.checkSpeciesAtomType(&species, 4, "CS3");
    systemTest.checkSpeciesAtomType(&species, 5, "CT3");
    systemTest.checkSpeciesAtomType(&species, 6, "HS3");
    systemTest.checkSpeciesAtomType(&species, 7, "HS3");
    systemTest.checkSpeciesAtomType(&species, 8, "HC");
    systemTest.checkSpeciesAtomType(&species, 9, "HC");
    systemTest.checkSpeciesAtomType(&species, 10, "HC");
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, c2SO4)
{
    Species species("C2SO4");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/c2so4.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Anions"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 11);
    ASSERT_EQ(species.nAngles(), 19);
    ASSERT_EQ(species.nTorsions(), 15);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "CT3");
    systemTest.checkSpeciesAtomType(&species, 1, "CS4");
    systemTest.checkSpeciesAtomType(&species, 2, "OC4");
    systemTest.checkSpeciesAtomType(&species, 3, "HC");
    systemTest.checkSpeciesAtomType(&species, 4, "HC");
    systemTest.checkSpeciesAtomType(&species, 5, "HC");
    systemTest.checkSpeciesAtomType(&species, 6, "HS4");
    systemTest.checkSpeciesAtomType(&species, 7, "HS4");
    systemTest.checkSpeciesAtomType(&species, 8, "SO");
    systemTest.checkSpeciesAtomType(&species, 9, "OS4");
    systemTest.checkSpeciesAtomType(&species, 10, "OS4");
    systemTest.checkSpeciesAtomType(&species, 11, "OS4");
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, c4fc1fsi)
{
    Species species("C4FC1FSI");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/c4fc1fsi.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Anions"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 23);
    ASSERT_EQ(species.nAngles(), 43);
    ASSERT_EQ(species.nTorsions(), 51);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "CBT");
    systemTest.checkSpeciesAtomType(&species, 1, "F1");
    systemTest.checkSpeciesAtomType(&species, 2, "F1");
    systemTest.checkSpeciesAtomType(&species, 3, "F1");
    systemTest.checkSpeciesAtomType(&species, 4, "SBT");
    systemTest.checkSpeciesAtomType(&species, 5, "NBT");
    systemTest.checkSpeciesAtomType(&species, 6, "OBT");
    systemTest.checkSpeciesAtomType(&species, 7, "OBT");
    systemTest.checkSpeciesAtomType(&species, 8, "SBT");
    systemTest.checkSpeciesAtomType(&species, 9, "OBT");
    systemTest.checkSpeciesAtomType(&species, 10, "OBT");
    systemTest.checkSpeciesAtomType(&species, 11, "C1F");
    systemTest.checkSpeciesAtomType(&species, 12, "CSF");
    systemTest.checkSpeciesAtomType(&species, 13, "F1");
    systemTest.checkSpeciesAtomType(&species, 14, "F1");
    systemTest.checkSpeciesAtomType(&species, 15, "CSF");
    systemTest.checkSpeciesAtomType(&species, 16, "F");
    systemTest.checkSpeciesAtomType(&species, 17, "F");
    systemTest.checkSpeciesAtomType(&species, 18, "CTF");
    systemTest.checkSpeciesAtomType(&species, 19, "F");
    systemTest.checkSpeciesAtomType(&species, 20, "F");
    systemTest.checkSpeciesAtomType(&species, 21, "F");
    systemTest.checkSpeciesAtomType(&species, 22, "F");
    systemTest.checkSpeciesAtomType(&species, 23, "F");
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, CCN3)
{
    Species species("CCN3");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/ccn3.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Anions"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 6);
    ASSERT_EQ(species.nAngles(), 6);
    ASSERT_EQ(species.nTorsions(), 6);
    ASSERT_EQ(species.nImpropers(), 1);

    systemTest.checkSpeciesAtomType(&species, 0, "C3A");
    systemTest.checkSpeciesAtomType(&species, 1, "CN");
    systemTest.checkSpeciesAtomType(&species, 2, "CN");
    systemTest.checkSpeciesAtomType(&species, 3, "CN");
    systemTest.checkSpeciesAtomType(&species, 4, "NC");
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, dca)
{
    Species species("DCA");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/dca.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Anions"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 4);
    ASSERT_EQ(species.nAngles(), 3);
    ASSERT_EQ(species.nTorsions(), 2);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "N3A");
    systemTest.checkSpeciesAtomType(&species, 1, "CZA");
    systemTest.checkSpeciesAtomType(&species, 2, "NZA");
    systemTest.checkSpeciesAtomType(&species, 3, "CZA");
    systemTest.checkSpeciesAtomType(&species, 4, "NZA");
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, fsi)
{
    Species species("FSI");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/fsi.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Anions"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 8);
    ASSERT_EQ(species.nAngles(), 13);
    ASSERT_EQ(species.nTorsions(), 6);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "FSI");
    systemTest.checkSpeciesAtomType(&species, 1, "SBT");
    systemTest.checkSpeciesAtomType(&species, 2, "NBT");
    systemTest.checkSpeciesAtomType(&species, 3, "OBT");
    systemTest.checkSpeciesAtomType(&species, 4, "OBT");
    systemTest.checkSpeciesAtomType(&species, 5, "SBT");
    systemTest.checkSpeciesAtomType(&species, 6, "OBT");
    systemTest.checkSpeciesAtomType(&species, 7, "OBT");
    systemTest.checkSpeciesAtomType(&species, 8, "FSI");
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, ntf2)
{
    Species species("NTf2");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/ntf2.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Anions"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 14);
    ASSERT_EQ(species.nAngles(), 25);
    ASSERT_EQ(species.nTorsions(), 24);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "CBT");
    systemTest.checkSpeciesAtomType(&species, 1, "F1");
    systemTest.checkSpeciesAtomType(&species, 2, "F1");
    systemTest.checkSpeciesAtomType(&species, 3, "F1");
    systemTest.checkSpeciesAtomType(&species, 4, "SBT");
    systemTest.checkSpeciesAtomType(&species, 5, "NBT");
    systemTest.checkSpeciesAtomType(&species, 6, "OBT");
    systemTest.checkSpeciesAtomType(&species, 7, "OBT");
    systemTest.checkSpeciesAtomType(&species, 8, "SBT");
    systemTest.checkSpeciesAtomType(&species, 9, "OBT");
    systemTest.checkSpeciesAtomType(&species, 10, "OBT");
    systemTest.checkSpeciesAtomType(&species, 11, "CBT");
    systemTest.checkSpeciesAtomType(&species, 12, "F1");
    systemTest.checkSpeciesAtomType(&species, 13, "F1");
    systemTest.checkSpeciesAtomType(&species, 14, "F1");
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, oac)
{
    Species species("OAc");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/oac.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Anions"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 6);
    ASSERT_EQ(species.nAngles(), 9);
    ASSERT_EQ(species.nTorsions(), 6);
    ASSERT_EQ(species.nImpropers(), 1);

    systemTest.checkSpeciesAtomType(&species, 0, "CTA");
    systemTest.checkSpeciesAtomType(&species, 1, "HC");
    systemTest.checkSpeciesAtomType(&species, 2, "HC");
    systemTest.checkSpeciesAtomType(&species, 3, "HC");
    systemTest.checkSpeciesAtomType(&species, 4, "CO2");
    systemTest.checkSpeciesAtomType(&species, 5, "O2");
    systemTest.checkSpeciesAtomType(&species, 6, "O2");
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, otf)
{
    Species species("OTf");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/otf.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Anions"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 7);
    ASSERT_EQ(species.nAngles(), 12);
    ASSERT_EQ(species.nTorsions(), 9);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "CBT");
    systemTest.checkSpeciesAtomType(&species, 1, "F1");
    systemTest.checkSpeciesAtomType(&species, 2, "F1");
    systemTest.checkSpeciesAtomType(&species, 3, "F1");
    systemTest.checkSpeciesAtomType(&species, 4, "SBT");
    systemTest.checkSpeciesAtomType(&species, 5, "OTF");
    systemTest.checkSpeciesAtomType(&species, 6, "OTF");
    systemTest.checkSpeciesAtomType(&species, 7, "OTF");
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, PF6)
{
    Species species("PF6");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/pf6.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Anions"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 6);
    ASSERT_EQ(species.nAngles(), 15);
    ASSERT_EQ(species.nTorsions(), 0);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "P");
    systemTest.checkSpeciesAtomType(&species, 1, "FP");
    systemTest.checkSpeciesAtomType(&species, 2, "FP");
    systemTest.checkSpeciesAtomType(&species, 3, "FP");
    systemTest.checkSpeciesAtomType(&species, 4, "FP");
    systemTest.checkSpeciesAtomType(&species, 5, "FP");
    systemTest.checkSpeciesAtomType(&species, 6, "FP");
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, SCN)
{
    Species species("SCN");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/scn.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Anions"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 2);
    ASSERT_EQ(species.nAngles(), 1);
    ASSERT_EQ(species.nTorsions(), 0);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "SK");
    systemTest.checkSpeciesAtomType(&species, 1, "CK");
    systemTest.checkSpeciesAtomType(&species, 2, "NK");
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, tfa)
{
    Species species("TFA");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/tfa.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Anions"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 6);
    ASSERT_EQ(species.nAngles(), 9);
    ASSERT_EQ(species.nTorsions(), 6);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "CFA");
    systemTest.checkSpeciesAtomType(&species, 1, "FFA");
    systemTest.checkSpeciesAtomType(&species, 2, "FFA");
    systemTest.checkSpeciesAtomType(&species, 3, "FFA");
    systemTest.checkSpeciesAtomType(&species, 4, "CO2");
    systemTest.checkSpeciesAtomType(&species, 5, "O2F");
    systemTest.checkSpeciesAtomType(&species, 6, "O2F");
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, tso)
{
    Species species("TSO");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/tso.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("PCL2019/Anions"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 18);
    ASSERT_EQ(species.nAngles(), 30);
    ASSERT_EQ(species.nTorsions(), 36);
    ASSERT_EQ(species.nImpropers(), 6);

    systemTest.checkSpeciesAtomType(&species, 0, "CATS");
    systemTest.checkSpeciesAtomType(&species, 1, "CAOS");
    systemTest.checkSpeciesAtomType(&species, 2, "CAMS");
    systemTest.checkSpeciesAtomType(&species, 3, "CAOS");
    systemTest.checkSpeciesAtomType(&species, 4, "CAMS");
    systemTest.checkSpeciesAtomType(&species, 5, "CAPS");
    systemTest.checkSpeciesAtomType(&species, 6, "CTTS");
    systemTest.checkSpeciesAtomType(&species, 7, "SO");
    systemTest.checkSpeciesAtomType(&species, 8, "HATS");
    systemTest.checkSpeciesAtomType(&species, 9, "HATS");
    systemTest.checkSpeciesAtomType(&species, 10, "HATS");
    systemTest.checkSpeciesAtomType(&species, 11, "HATS");
    systemTest.checkSpeciesAtomType(&species, 12, "HTS");
    systemTest.checkSpeciesAtomType(&species, 13, "HTS");
    systemTest.checkSpeciesAtomType(&species, 14, "HTS");
    systemTest.checkSpeciesAtomType(&species, 15, "OS3");
    systemTest.checkSpeciesAtomType(&species, 16, "OS3");
    systemTest.checkSpeciesAtomType(&species, 17, "OS3");
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}
}; // namespace UnitTest
