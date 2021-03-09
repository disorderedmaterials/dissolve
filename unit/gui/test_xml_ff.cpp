// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "data/elements.h"
#include "data/ff/library.h"
#include "data/ff/xml/base.h"
#include "main/dissolve.h"
#include "models/xmlAngleModel.h"
#include "models/xmlAtomModel.h"
#include "models/xmlBondModel.h"
#include "models/xmlImproperModel.h"
#include "models/xmlTorsionModel.h"
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
    void SetUp() override { ASSERT_TRUE(doc.load_file("ff/methanol.xml")); }

    pugi::xml_document doc;
};

TEST_F(XmlFFTest, XmlBond)
{
    XmlBondModel bonds;

    bonds.readFile(doc.root());

    ASSERT_EQ(bonds.columnCount(), 4);
    ASSERT_EQ(bonds.rowCount(), 5);

    std::vector<XmlBondData> reference = {{"O801", "C800", 0.141000, 267776.000000},
                                          {"H802", "C800", 0.109000, 284512.000000},
                                          {"H803", "C800", 0.109000, 284512.000000},
                                          {"H804", "C800", 0.109000, 284512.000000},
                                          {"H805", "O801", 0.094500, 462750.400000}};

    int row = 0;
    for (auto b : reference)
    {
        ASSERT_EQ(bonds.data(bonds.index(row, 0)).toString().toStdString(), std::get<0>(b));
        ASSERT_EQ(bonds.data(bonds.index(row, 1)).toString().toStdString(), std::get<1>(b));
        ASSERT_EQ(bonds.data(bonds.index(row, 2)).toDouble(), std::get<2>(b));
        ASSERT_EQ(bonds.data(bonds.index(row, 3)).toDouble(), std::get<3>(b));
        ++row;
    }
}

TEST_F(XmlFFTest, XmlAngle)
{
    XmlAngleModel angles;

    angles.readFile(doc.root());

    ASSERT_EQ(angles.columnCount(), 5);
    ASSERT_EQ(angles.rowCount(), 7);

    std::vector<XmlAngleData> reference = {
        {"O801", "C800", "H802", 1.911136, 292.880000}, {"O801", "C800", "H803", 1.911136, 292.880000},
        {"O801", "C800", "H804", 1.911136, 292.880000}, {"C800", "O801", "H805", 1.893682, 460.240000},
        {"H802", "C800", "H803", 1.881465, 276.144000}, {"H802", "C800", "H804", 1.881465, 276.144000},
        {"H803", "C800", "H804", 1.881465, 276.144000}};

    int row = 0;
    for (auto b : reference)
    {
        ASSERT_EQ(angles.data(angles.index(row, 0)).toString().toStdString(), std::get<0>(b));
        ASSERT_EQ(angles.data(angles.index(row, 1)).toString().toStdString(), std::get<1>(b));
        ASSERT_EQ(angles.data(angles.index(row, 2)).toString().toStdString(), std::get<2>(b));
        ASSERT_EQ(angles.data(angles.index(row, 3)).toDouble(), std::get<3>(b));
        ASSERT_EQ(angles.data(angles.index(row, 4)).toDouble(), std::get<4>(b));
        ++row;
    }
}

TEST_F(XmlFFTest, XmlTorsion)
{
    XmlTorsionModel torsions;

    torsions.readFile(doc.root());

    ASSERT_EQ(torsions.columnCount(), 16);
    ASSERT_EQ(torsions.rowCount(), 3);

    std::vector<XmlTorsionData> reference = {{"H805", "O801", "C800", "H802", 0.000000, 0.000000, 0.736384, 0.000000, 1, 2, 3,
                                              4, 0.00, 3.141592653589793, 0.00, 3.141592653589793},
                                             {"H805", "O801", "C800", "H803", 0.000000, 0.000000, 0.736384, 0.000000, 1, 2, 3,
                                              4, 0.00, 3.141592653589793, 0.00, 3.141592653589793},
                                             {"H805", "O801", "C800", "H804", 0.000000, 0.000000, 0.736384, 0.000000, 1, 2, 3,
                                              4, 0.00, 3.141592653589793, 0.00, 3.141592653589793}};

    int row = 0;
    for (auto b : reference)
    {
        ASSERT_EQ(torsions.data(torsions.index(row, 0)).toString().toStdString(), std::get<0>(b));
        ASSERT_EQ(torsions.data(torsions.index(row, 1)).toString().toStdString(), std::get<1>(b));
        ASSERT_EQ(torsions.data(torsions.index(row, 2)).toString().toStdString(), std::get<2>(b));
        ASSERT_EQ(torsions.data(torsions.index(row, 3)).toString().toStdString(), std::get<3>(b));

        ASSERT_EQ(torsions.data(torsions.index(row, 4)).toDouble(), std::get<4>(b));
        ASSERT_EQ(torsions.data(torsions.index(row, 5)).toDouble(), std::get<5>(b));
        ASSERT_EQ(torsions.data(torsions.index(row, 6)).toDouble(), std::get<6>(b));
        ASSERT_EQ(torsions.data(torsions.index(row, 7)).toDouble(), std::get<7>(b));
        ++row;
    }
}

TEST_F(XmlFFTest, XmlImproper)
{
    XmlImproperModel impropers;

    impropers.readFile(doc.root());

    ASSERT_EQ(impropers.columnCount(), 16);
    ASSERT_EQ(impropers.rowCount(), 2);

    std::vector<XmlImproperData> reference = {{"C800", "O801", "H802", "H803", 0.000000, 0.000000, 0.000000, 0.000000, 1, 2, 3,
                                               4, 0.00, 3.141592653589793, 0.00, 3.141592653589793},
                                              {"C800", "O801", "H802", "H804", 0.000000, 0.000000, 0.000000, 0.000000, 1, 2, 3,
                                               4, 0.00, 3.141592653589793, 0.00, 3.141592653589793}};

    int row = 0;
    for (auto b : reference)
    {
        ASSERT_EQ(impropers.data(impropers.index(row, 0)).toString().toStdString(), std::get<0>(b));
        ASSERT_EQ(impropers.data(impropers.index(row, 1)).toString().toStdString(), std::get<1>(b));
        ASSERT_EQ(impropers.data(impropers.index(row, 2)).toString().toStdString(), std::get<2>(b));
        ASSERT_EQ(impropers.data(impropers.index(row, 3)).toString().toStdString(), std::get<3>(b));

        ASSERT_EQ(impropers.data(impropers.index(row, 4)).toDouble(), std::get<4>(b));
        ASSERT_EQ(impropers.data(impropers.index(row, 5)).toDouble(), std::get<5>(b));
        ASSERT_EQ(impropers.data(impropers.index(row, 6)).toDouble(), std::get<6>(b));
        ASSERT_EQ(impropers.data(impropers.index(row, 7)).toDouble(), std::get<7>(b));
        ++row;
    }
}

TEST_F(XmlFFTest, XmlAtom)
{
    CoreData coreData;
    Dissolve dissolve(coreData);
    XmlAtomModel atoms(dissolve);

    atoms.readFile(doc.root());

    ASSERT_EQ(atoms.columnCount(), 5);
    ASSERT_EQ(atoms.rowCount(), 6);

    std::vector<XmlAtomData> reference = {{"opls_802", "H802", "H", 1.008000, -1},  {"opls_804", "H804", "H", 1.008000, -1},
                                          {"opls_801", "O801", "O", 15.999000, -1}, {"opls_803", "H803", "H", 1.008000, -1},
                                          {"opls_800", "C800", "C", 12.011000, -1}, {"opls_805", "H805", "H", 1.008000, -1}};

    int row = 0;
    for (auto b : reference)
    {
        ASSERT_EQ(atoms.data(atoms.index(row, 0)).toString().toStdString(), std::get<0>(b));
        ASSERT_EQ(atoms.data(atoms.index(row, 1)).toString().toStdString(), std::get<1>(b));
        ASSERT_EQ(atoms.data(atoms.index(row, 2)).toString().toStdString(), std::get<2>(b));
        ASSERT_EQ(atoms.data(atoms.index(row, 3)).toDouble(), std::get<3>(b));
        ASSERT_EQ(atoms.data(atoms.index(row, 4)).toString().toStdString(), "Missing");
        ++row;
    }

    // Cannot set atom type because we haven't defined this type yet.
    ASSERT_FALSE(atoms.setData(atoms.index(0, 4), "H"));
    ASSERT_EQ(atoms.data(atoms.index(0, 4)).toString().toStdString(), "Missing");

    auto type = dissolve.addAtomType(Elements::H);
    ASSERT_EQ(type->name(), "H");

    // Now we can add the atom type because it exists;
    ASSERT_TRUE(atoms.setData(atoms.index(0, 4), "H"));
    ASSERT_EQ(atoms.data(atoms.index(0, 4)).toString().toStdString(), "H");
}

TEST_F(XmlFFTest, XmlAll)
{
    CoreData coreData;
    Dissolve dissolve(coreData);
    XmlAtomModel atomModel(dissolve);
    XmlBondModel bondModel;
    XmlAngleModel angleModel;
    XmlTorsionModel torsionModel;
    XmlImproperModel improperModel;

    dissolve.addAtomType(Elements::H);
    dissolve.addAtomType(Elements::C);
    dissolve.addAtomType(Elements::O);

    atomModel.readFile(doc.root());
    bondModel.readFile(doc.root());
    angleModel.readFile(doc.root());
    torsionModel.readFile(doc.root());
    improperModel.readFile(doc.root());
    auto map = atomModel.toMap();
    std::vector<ForcefieldAtomType> atoms = atomModel.toVector();
    std::vector<ForcefieldBondTerm> bonds = bondModel.toVector(map);
    std::vector<ForcefieldAngleTerm> angles = angleModel.toVector(map);
    std::vector<ForcefieldTorsionTerm> torsions = torsionModel.toVector(map);
    std::vector<ForcefieldImproperTerm> impropers = improperModel.toVector(map);
    ASSERT_EQ(atoms.size(), 6);
    ASSERT_EQ(bonds.size(), 5);
    ASSERT_EQ(angles.size(), 7);
    ASSERT_EQ(torsions.size(), 3);
    ASSERT_EQ(impropers.size(), 2);

    // auto xmlFF = std::make_shared<Forcefield_XML>();
    auto xmlFF = std::make_shared<Forcefield_XML>(atoms, bonds, angles, torsions, impropers);
    ForcefieldLibrary::registerForcefield(std::static_pointer_cast<Forcefield>(xmlFF));
}

} // namespace UnitTest
