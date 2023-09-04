// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/atomType.h"
#include "data/elements.h"
#include "data/ff/atomType.h"
#include "data/ff/library.h"
#include "data/ff/xml/base.h"
#include "gui/models/xmlAngleModel.h"
#include "gui/models/xmlAtomModel.h"
#include "gui/models/xmlBondModel.h"
#include "gui/models/xmlImproperModel.h"
#include "gui/models/xmlTorsionModel.h"
#include "gui/models/xmlTreeModel.h"
#include "main/dissolve.h"
#include <gtest/gtest.h>
#include <memory>
#include <tuple>
#include <vector>

namespace UnitTest
{

class XmlFFTest : public ::testing::Test
{
    public:
    XmlFFTest() = default;

    protected:
    void SetUp() override { ASSERT_TRUE(doc.load_file("dissolve/ff/methanol.xml")); }

    pugi::xml_document doc;
};

TEST_F(XmlFFTest, XmlBond)
{
    XmlBondModel bonds;

    bonds.readFile(doc.root());

    EXPECT_EQ(bonds.columnCount(), 4);
    EXPECT_EQ(bonds.rowCount(), 5);

    std::vector<XmlBondData> reference = {{"O801", "C800", 1.41000, 2677.760000},
                                          {"H802", "C800", 1.09000, 2845.120000},
                                          {"H803", "C800", 1.09000, 2845.120000},
                                          {"H804", "C800", 1.09000, 2845.120000},
                                          {"H805", "O801", 0.94500, 4627.50400000}};

    int row = 0;
    for (auto b : reference)
    {
        EXPECT_EQ(bonds.data(bonds.index(row, 0)).toString().toStdString(), std::get<0>(b));
        EXPECT_EQ(bonds.data(bonds.index(row, 1)).toString().toStdString(), std::get<1>(b));
        EXPECT_DOUBLE_EQ(bonds.data(bonds.index(row, 2)).toDouble(), std::get<2>(b));
        EXPECT_DOUBLE_EQ(bonds.data(bonds.index(row, 3)).toDouble(), std::get<3>(b));
        ++row;
    }
}

TEST_F(XmlFFTest, XmlAngle)
{
    XmlAngleModel angles;

    angles.readFile(doc.root());

    EXPECT_EQ(angles.columnCount(), 5);
    EXPECT_EQ(angles.rowCount(), 7);

    std::vector<XmlAngleData> reference = {
        {"O801", "C800", "H802", 1.911136, 292.880000}, {"O801", "C800", "H803", 1.911136, 292.880000},
        {"O801", "C800", "H804", 1.911136, 292.880000}, {"C800", "O801", "H805", 1.893682, 460.240000},
        {"H802", "C800", "H803", 1.881465, 276.144000}, {"H802", "C800", "H804", 1.881465, 276.144000},
        {"H803", "C800", "H804", 1.881465, 276.144000}};

    int row = 0;
    for (auto b : reference)
    {
        EXPECT_EQ(angles.data(angles.index(row, 0)).toString().toStdString(), std::get<0>(b));
        EXPECT_EQ(angles.data(angles.index(row, 1)).toString().toStdString(), std::get<1>(b));
        EXPECT_EQ(angles.data(angles.index(row, 2)).toString().toStdString(), std::get<2>(b));
        EXPECT_DOUBLE_EQ(angles.data(angles.index(row, 3)).toDouble(), std::get<3>(b) * 180 / PI);
        EXPECT_DOUBLE_EQ(angles.data(angles.index(row, 4)).toDouble(), std::get<4>(b));
        ++row;
    }
}

TEST_F(XmlFFTest, XmlTorsion)
{
    XmlTorsionModel torsions;

    torsions.readFile(doc.root());

    EXPECT_EQ(torsions.columnCount(), 16);
    EXPECT_EQ(torsions.rowCount(), 3);

    std::vector<XmlTorsionData> reference = {{"H805", "O801", "C800", "H802", 0.000000, 0.000000, 0.736384, 0.000000, 1, 2, 3,
                                              4, 0.00, 3.141592653589793, 0.00, 3.141592653589793},
                                             {"H805", "O801", "C800", "H803", 0.000000, 0.000000, 0.736384, 0.000000, 1, 2, 3,
                                              4, 0.00, 3.141592653589793, 0.00, 3.141592653589793},
                                             {"H805", "O801", "C800", "H804", 0.000000, 0.000000, 0.736384, 0.000000, 1, 2, 3,
                                              4, 0.00, 3.141592653589793, 0.00, 3.141592653589793}};

    int row = 0;
    for (auto b : reference)
    {
        EXPECT_EQ(torsions.data(torsions.index(row, 0)).toString().toStdString(), std::get<0>(b));
        EXPECT_EQ(torsions.data(torsions.index(row, 1)).toString().toStdString(), std::get<1>(b));
        EXPECT_EQ(torsions.data(torsions.index(row, 2)).toString().toStdString(), std::get<2>(b));
        EXPECT_EQ(torsions.data(torsions.index(row, 3)).toString().toStdString(), std::get<3>(b));

        EXPECT_DOUBLE_EQ(torsions.data(torsions.index(row, 4)).toDouble(), std::get<4>(b));
        EXPECT_DOUBLE_EQ(torsions.data(torsions.index(row, 5)).toDouble(), std::get<5>(b));
        EXPECT_DOUBLE_EQ(torsions.data(torsions.index(row, 6)).toDouble(), std::get<6>(b));
        EXPECT_DOUBLE_EQ(torsions.data(torsions.index(row, 7)).toDouble(), std::get<7>(b));
        ++row;
    }
}

TEST_F(XmlFFTest, XmlImproper)
{
    XmlImproperModel impropers;

    impropers.readFile(doc.root());

    EXPECT_EQ(impropers.columnCount(), 16);
    EXPECT_EQ(impropers.rowCount(), 2);

    std::vector<XmlImproperData> reference = {{"C800", "O801", "H802", "H803", 0.000000, 0.000000, 0.000000, 0.000000, 1, 2, 3,
                                               4, 0.00, 3.141592653589793, 0.00, 3.141592653589793},
                                              {"C800", "O801", "H802", "H804", 0.000000, 0.000000, 0.000000, 0.000000, 1, 2, 3,
                                               4, 0.00, 3.141592653589793, 0.00, 3.141592653589793}};

    int row = 0;
    for (auto b : reference)
    {
        EXPECT_EQ(impropers.data(impropers.index(row, 0)).toString().toStdString(), std::get<0>(b));
        EXPECT_EQ(impropers.data(impropers.index(row, 1)).toString().toStdString(), std::get<1>(b));
        EXPECT_EQ(impropers.data(impropers.index(row, 2)).toString().toStdString(), std::get<2>(b));
        EXPECT_EQ(impropers.data(impropers.index(row, 3)).toString().toStdString(), std::get<3>(b));

        EXPECT_DOUBLE_EQ(impropers.data(impropers.index(row, 4)).toDouble(), std::get<4>(b));
        EXPECT_DOUBLE_EQ(impropers.data(impropers.index(row, 5)).toDouble(), std::get<5>(b));
        EXPECT_DOUBLE_EQ(impropers.data(impropers.index(row, 6)).toDouble(), std::get<6>(b));
        EXPECT_DOUBLE_EQ(impropers.data(impropers.index(row, 7)).toDouble(), std::get<7>(b));
        ++row;
    }
}

TEST_F(XmlFFTest, XmlAtom)
{
    CoreData coreData;
    XmlAtomModel atoms(coreData);

    atoms.readFile(doc.root());

    EXPECT_EQ(atoms.columnCount(), 7);
    EXPECT_EQ(atoms.rowCount(), 6);

    std::vector<XmlAtomData> reference = {{"opls_802", "H802", "H", 1.008000, 0.0768, 2.5, 0.12552},
                                          {"opls_804", "H804", "H", 1.008000, 0.0768, 2.5, 0.125520},
                                          {"opls_801", "O801", "O", 15.999000, -0.5873, 3.12, 0.71128},
                                          {"opls_803", "H803", "H", 1.008000, 0.0768, 2.5, 0.125520},
                                          {"opls_800", "C800", "C", 12.011000, -0.0491, 3.5, 0.276144},
                                          {"opls_805", "H805", "H", 1.008000, 0.4061, 0.0, 0.0}};

    int row = 0;
    for (auto b : reference)
    {
        EXPECT_EQ(atoms.data(atoms.index(row, 0)).toString().toStdString(), std::get<0>(b));
        EXPECT_EQ(atoms.data(atoms.index(row, 1)).toString().toStdString(), std::get<1>(b));
        EXPECT_EQ(atoms.data(atoms.index(row, 2)).toString().toStdString(), std::get<2>(b));
        EXPECT_DOUBLE_EQ(atoms.data(atoms.index(row, 3)).toDouble(), std::get<3>(b));
        EXPECT_DOUBLE_EQ(atoms.data(atoms.index(row, 4)).toDouble(), std::get<4>(b));
        EXPECT_DOUBLE_EQ(atoms.data(atoms.index(row, 5)).toDouble(), std::get<5>(b));
        EXPECT_DOUBLE_EQ(atoms.data(atoms.index(row, 6)).toDouble(), std::get<6>(b));
        ++row;
    }
}

TEST_F(XmlFFTest, XmlTree)
{
    CoreData coreData;
    XmlTreeModel treeModel(coreData);

    treeModel.setName("TestFF");

    treeModel.readFile(doc.root());

    // Test the top level branches of the tree
    auto atomIndex = treeModel.index(0, 0);
    EXPECT_EQ(treeModel.data(atomIndex).toString().toStdString(), "Atom Types (6)");
    auto bondIndex = treeModel.index(1, 0);
    EXPECT_EQ(treeModel.data(bondIndex).toString().toStdString(), "Bonds (5)");
    auto angleIndex = treeModel.index(2, 0);
    EXPECT_EQ(treeModel.data(angleIndex).toString().toStdString(), "Angles (7)");
    auto torsionIndex = treeModel.index(3, 0);
    EXPECT_EQ(treeModel.data(torsionIndex).toString().toStdString(), "Torsions (3)");
    auto improperIndex = treeModel.index(4, 0);
    EXPECT_EQ(treeModel.data(improperIndex).toString().toStdString(), "Impropers (2)");

    // Test the atoms.  Since we're mostly repeating the underlying
    // model, we're mostly just checking that the model passed through
    // properly
    EXPECT_EQ(treeModel.data(treeModel.index(0, 0, atomIndex)).toString().toStdString(), "opls_802");
    EXPECT_EQ(treeModel.data(treeModel.index(0, 1, atomIndex)).toString().toStdString(), "H802");
    EXPECT_EQ(treeModel.data(treeModel.index(0, 2, atomIndex)).toString().toStdString(), "H");
    EXPECT_DOUBLE_EQ(treeModel.data(treeModel.index(0, 3, atomIndex)).toDouble(), 1.008000);

    // Test the bonds
    EXPECT_EQ(treeModel.data(treeModel.index(0, 0, bondIndex)).toString().toStdString(), "O801");
    EXPECT_EQ(treeModel.data(treeModel.index(0, 1, bondIndex)).toString().toStdString(), "C800");
    EXPECT_DOUBLE_EQ(treeModel.data(treeModel.index(0, 2, bondIndex)).toDouble(), 1.41);
    EXPECT_DOUBLE_EQ(treeModel.data(treeModel.index(0, 3, bondIndex)).toDouble(), 2677.76);

    // Test the angles
    EXPECT_EQ(treeModel.data(treeModel.index(0, 0, angleIndex)).toString().toStdString(), "O801");
    EXPECT_EQ(treeModel.data(treeModel.index(0, 1, angleIndex)).toString().toStdString(), "C800");
    EXPECT_EQ(treeModel.data(treeModel.index(0, 2, angleIndex)).toString().toStdString(), "H802");
    EXPECT_DOUBLE_EQ(treeModel.data(treeModel.index(0, 3, angleIndex)).toDouble(), 1.911136 * 180 / PI);
    EXPECT_DOUBLE_EQ(treeModel.data(treeModel.index(0, 4, angleIndex)).toDouble(), 292.88);

    // Test the torsions
    EXPECT_EQ(treeModel.data(treeModel.index(0, 0, torsionIndex)).toString().toStdString(), "H805");
    EXPECT_EQ(treeModel.data(treeModel.index(0, 1, torsionIndex)).toString().toStdString(), "O801");
    EXPECT_EQ(treeModel.data(treeModel.index(0, 2, torsionIndex)).toString().toStdString(), "C800");
    EXPECT_EQ(treeModel.data(treeModel.index(0, 3, torsionIndex)).toString().toStdString(), "H802");
    EXPECT_DOUBLE_EQ(treeModel.data(treeModel.index(0, 4, torsionIndex)).toDouble(), 0);
    EXPECT_DOUBLE_EQ(treeModel.data(treeModel.index(0, 6, torsionIndex)).toDouble(), 0.736384);

    // Test the impropers
    EXPECT_EQ(treeModel.data(treeModel.index(0, 0, improperIndex)).toString().toStdString(), "C800");
    EXPECT_EQ(treeModel.data(treeModel.index(0, 1, improperIndex)).toString().toStdString(), "O801");
    EXPECT_EQ(treeModel.data(treeModel.index(0, 2, improperIndex)).toString().toStdString(), "H802");
    EXPECT_EQ(treeModel.data(treeModel.index(0, 3, improperIndex)).toString().toStdString(), "H803");
    EXPECT_DOUBLE_EQ(treeModel.data(treeModel.index(0, 4, improperIndex)).toDouble(), 0);

    // Construct the actual forcefield
    ForcefieldLibrary::registerForcefield(treeModel.toForcefield());
    auto xmlFF = ForcefieldLibrary::forcefield("TestFF");
    ASSERT_TRUE(xmlFF);
    EXPECT_EQ(xmlFF->name(), "TestFF");

    // Pull the atoms
    auto oxygen = xmlFF->atomTypeByName("O801");
    ASSERT_TRUE(oxygen);
    auto carbon = xmlFF->atomTypeByName("C800");
    ASSERT_TRUE(carbon);
    auto hydrogen2 = xmlFF->atomTypeByName("H802");
    ASSERT_TRUE(hydrogen2);
    auto hydrogen3 = xmlFF->atomTypeByName("H803");
    ASSERT_TRUE(hydrogen3);
    auto hydrogen4 = xmlFF->atomTypeByName("H804");
    ASSERT_TRUE(hydrogen4);
    auto hydrogen5 = xmlFF->atomTypeByName("H805");
    ASSERT_TRUE(hydrogen5);
    auto nonexistent = xmlFF->atomTypeByName("Q800");
    ASSERT_FALSE(nonexistent);

    // Test the bonds
    auto bond = xmlFF->getBondTerm((*oxygen).get(), (*carbon).get());
    ASSERT_TRUE(bond);
    EXPECT_DOUBLE_EQ((*bond).get().parameters()[0], 2677.76000000);
    EXPECT_DOUBLE_EQ((*bond).get().parameters()[1], 1.41000);

    // Test the angles
    auto angle = xmlFF->getAngleTerm((*oxygen).get(), (*carbon).get(), (*hydrogen2).get());
    ASSERT_TRUE(angle);
    EXPECT_EQ((*angle).get().parameters()[0], 292.88000);
    EXPECT_EQ((*angle).get().parameters()[1], 1.911136 * 180 / PI);

    // Test the torsions
    auto torsion = xmlFF->getTorsionTerm((*hydrogen5).get(), (*oxygen).get(), (*carbon).get(), (*hydrogen2).get());
    ASSERT_TRUE(torsion);
    EXPECT_EQ((*torsion).get().parameters()[0], 0);
    EXPECT_EQ((*torsion).get().parameters()[2], 0.736384);

    // Test the impropers
    auto improper = xmlFF->getImproperTerm((*carbon).get(), (*oxygen).get(), (*hydrogen2).get(), (*hydrogen3).get());
    ASSERT_TRUE(improper);
    EXPECT_EQ((*improper).get().parameters()[0], 0);
}

} // namespace UnitTest
