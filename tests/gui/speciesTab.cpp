// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/species.h"
#include "gui/models/speciesAngleModel.h"
#include "gui/models/speciesAtomModel.h"
#include "gui/models/speciesBondModel.h"
#include "gui/models/speciesImproperModel.h"
#include "gui/models/speciesIsoModel.h"
#include "gui/models/speciesTorsionModel.h"
#include "main/dissolve.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>

Q_DECLARE_METATYPE(Sears91::Isotope);

namespace UnitTest
{

class SpeciesTabTest : public ::testing::Test
{
    public:
    SpeciesTabTest() = default;

    protected:
    void SetUp() override {}
};

std::vector<Qt::ItemDataRole> roles = {Qt::DisplayRole, Qt::EditRole};

TEST_F(SpeciesTabTest, Atoms)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    ASSERT_TRUE(dissolve.loadInput("dissolve/input/full-benzene.txt"));
    auto &species = coreData.species()[0];

    SpeciesAtomModel atom(*species, coreData);

    // Test Atoms
    EXPECT_EQ(atom.columnCount(), 6);
    EXPECT_EQ(atom.rowCount(), 12);

    // Test Carbon atom
    for (auto role : roles)
    {
        EXPECT_EQ(atom.data(atom.index(0, 0), role).toString().toStdString(), "C");
        EXPECT_EQ(atom.data(atom.index(0, 1), role).toString().toStdString(), "CA");
        EXPECT_DOUBLE_EQ(atom.data(atom.index(0, 2), role).toDouble(), -1.399);
        EXPECT_DOUBLE_EQ(atom.data(atom.index(0, 3), role).toDouble(), 0.1600);
        EXPECT_DOUBLE_EQ(atom.data(atom.index(0, 4), role).toDouble(), 0.000);
        EXPECT_DOUBLE_EQ(atom.data(atom.index(0, 5), role).toDouble(), -0.115);

        // Test Hydrogen Atom
        EXPECT_EQ(atom.data(atom.index(6, 0), role).toString().toStdString(), "H");
        EXPECT_EQ(atom.data(atom.index(6, 1), role).toString().toStdString(), "HA");
        EXPECT_DOUBLE_EQ(atom.data(atom.index(6, 2), role).toDouble(), 1.483);
        EXPECT_DOUBLE_EQ(atom.data(atom.index(6, 3), role).toDouble(), 2.001);
        EXPECT_DOUBLE_EQ(atom.data(atom.index(6, 4), role).toDouble(), 0.000);
        EXPECT_DOUBLE_EQ(atom.data(atom.index(6, 5), role).toDouble(), 0.115);
    }

    // Mutate Hydrogen
    EXPECT_FALSE(atom.setData(atom.index(6, 0), "C"));
    EXPECT_TRUE(atom.setData(atom.index(6, 1), "HA"));
    EXPECT_FALSE(atom.setData(atom.index(6, 1), "Unreal"));
    EXPECT_TRUE(atom.setData(atom.index(6, 2), "CA"));
    EXPECT_TRUE(atom.setData(atom.index(6, 3), 3.0));
    EXPECT_TRUE(atom.setData(atom.index(6, 4), 4.0));
    EXPECT_TRUE(atom.setData(atom.index(6, 5), 5.0));
    for (auto i = 3; i < 6; ++i)
        EXPECT_EQ(atom.data(atom.index(6, i)).toDouble(), i);
}

TEST_F(SpeciesTabTest, Bonds)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    ASSERT_TRUE(dissolve.loadInput("dissolve/input/full-benzene.txt"));
    auto &species = coreData.species()[0];

    SpeciesBondModel bond(species->bonds(), coreData);

    // Test Bonds
    EXPECT_EQ(bond.columnCount(), 4);
    EXPECT_EQ(bond.rowCount(), 12);

    for (auto role : roles)
    {
        EXPECT_EQ(bond.data(bond.index(3, 0), role).toInt(), 4);
        EXPECT_EQ(bond.data(bond.index(3, 1), role).toInt(), 5);
        EXPECT_EQ(bond.data(bond.index(3, 2), role).toString().toStdString(), "@CA-CA");
        EXPECT_EQ(bond.data(bond.index(3, 3), role).toString().toStdString(), "k=3924.59 eq=1.4");
    }

    // Mutate bond
    EXPECT_FALSE(bond.setData(bond.index(3, 0), 5));
    EXPECT_FALSE(bond.setData(bond.index(3, 1), 6));

    EXPECT_FALSE(bond.setData(bond.index(3, 3), "4, 5"));

    EXPECT_FALSE(bond.setData(bond.index(3, 2), "Undefined"));
    EXPECT_TRUE(bond.setData(bond.index(3, 2), "Harmonic"));

    EXPECT_TRUE(bond.setData(bond.index(3, 3), "4.0 5.0"));
    EXPECT_THAT(bond.data(bond.index(3, 3)).toString().toStdString(),
                testing::AnyOf(testing::Eq("k=4.0 eq=5.0"), testing::Eq("k=4 eq=5")));

    EXPECT_TRUE(bond.setData(bond.index(3, 2), "@CA-CA"));
    EXPECT_EQ(bond.data(bond.index(3, 3)).toString().toStdString(), "k=3924.59 eq=1.4");
}

TEST_F(SpeciesTabTest, Angles)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    ASSERT_TRUE(dissolve.loadInput("dissolve/input/full-benzene.txt"));
    auto &species = coreData.species()[0];

    SpeciesAngleModel angle(species->angles(), coreData);

    // Test Angles
    EXPECT_EQ(angle.columnCount(), 5);
    EXPECT_EQ(angle.rowCount(), 18);
    for (auto role : roles)
    {
        EXPECT_EQ(angle.data(angle.index(3, 0), role).toInt(), 4);
        EXPECT_EQ(angle.data(angle.index(3, 1), role).toInt(), 5);
        EXPECT_EQ(angle.data(angle.index(3, 2), role).toInt(), 6);
        EXPECT_EQ(angle.data(angle.index(3, 3), role).toString().toStdString(), "@CA-CA-CA");
        EXPECT_THAT(angle.data(angle.index(3, 4), role).toString().toStdString(),
                    testing::AnyOf(testing::Eq("k=527.184 eq=120.0"), testing::Eq("k=527.184 eq=120")));
    }

    // Mutate angle
    EXPECT_FALSE(angle.setData(angle.index(3, 0), 5));
    EXPECT_FALSE(angle.setData(angle.index(3, 1), 6));
    EXPECT_FALSE(angle.setData(angle.index(3, 2), 7));
    EXPECT_FALSE(angle.setData(angle.index(3, 4), 6));

    EXPECT_FALSE(angle.setData(angle.index(3, 3), "Undefined"));
    EXPECT_TRUE(angle.setData(angle.index(3, 3), "Harmonic"));

    EXPECT_TRUE(angle.setData(angle.index(3, 4), "4.0 5.0"));
    EXPECT_THAT(angle.data(angle.index(3, 4)).toString().toStdString(),
                testing::AnyOf(testing::Eq("k=4.0 eq=5.0"), testing::Eq("k=4 eq=5")));

    EXPECT_TRUE(angle.setData(angle.index(3, 3), "@CA-CA-CA"));
    EXPECT_THAT(angle.data(angle.index(3, 4)).toString().toStdString(),
                testing::AnyOf(testing::Eq("k=527.184 eq=120.0"), testing::Eq("k=527.184 eq=120")));
}

TEST_F(SpeciesTabTest, Torsions)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    ASSERT_TRUE(dissolve.loadInput("dissolve/input/full-benzene.txt"));
    auto &species = coreData.species()[0];

    SpeciesTorsionModel torsion(species->torsions(), coreData);

    // Test Torsions
    EXPECT_EQ(torsion.columnCount(), 8);
    EXPECT_EQ(torsion.rowCount(), 24);
    for (auto role : roles)
    {
        EXPECT_EQ(torsion.data(torsion.index(3, 0), role).toInt(), 6);
        EXPECT_EQ(torsion.data(torsion.index(3, 1), role).toInt(), 1);
        EXPECT_EQ(torsion.data(torsion.index(3, 2), role).toInt(), 2);
        EXPECT_EQ(torsion.data(torsion.index(3, 3), role).toInt(), 3);
        EXPECT_EQ(torsion.data(torsion.index(3, 4), role).toString().toStdString(), "@CA-CA-CA-CA");
        EXPECT_THAT(torsion.data(torsion.index(3, 5), role).toString().toStdString(),
                    testing::AnyOf(testing::Eq("k1=0.0 k2=30.334 k3=0.0"), testing::Eq("k1=0 k2=30.334 k3=0")));
        EXPECT_DOUBLE_EQ(torsion.data(torsion.index(3, 6), role).toDouble(), 0.5);
        EXPECT_DOUBLE_EQ(torsion.data(torsion.index(3, 7), role).toDouble(), 0.5);
    }

    // Mutate torsion
    EXPECT_FALSE(torsion.setData(torsion.index(3, 0), 5));
    EXPECT_FALSE(torsion.setData(torsion.index(3, 1), 6));
    EXPECT_FALSE(torsion.setData(torsion.index(3, 2), 7));
    EXPECT_FALSE(torsion.setData(torsion.index(3, 3), 8));

    EXPECT_FALSE(torsion.setData(torsion.index(3, 5), "4.0, 5.0, 6.0"));

    EXPECT_FALSE(torsion.setData(torsion.index(3, 4), "Undefined"));
    EXPECT_TRUE(torsion.setData(torsion.index(3, 4), "Cos3"));

    EXPECT_FALSE(torsion.setData(torsion.index(3, 5), "4.0 5.0"));
    EXPECT_TRUE(torsion.setData(torsion.index(3, 5), "4.0 5.0 6.0"));
    EXPECT_THAT(torsion.data(torsion.index(3, 5)).toString().toStdString(),
                testing::AnyOf(testing::Eq("k1=4.0 k2=5.0 k3=6.0"), testing::Eq("k1=4 k2=5 k3=6")));

    EXPECT_TRUE(torsion.setData(torsion.index(3, 6), 0.83));
    EXPECT_DOUBLE_EQ(torsion.data(torsion.index(3, 6)).toDouble(), 0.83);
    EXPECT_TRUE(torsion.setData(torsion.index(3, 7), 0.0));
    EXPECT_DOUBLE_EQ(torsion.data(torsion.index(3, 7)).toDouble(), 0.0);

    EXPECT_FALSE(torsion.setData(torsion.index(3, 8), 8));
    EXPECT_EQ(torsion.data(torsion.index(3, 8)).toDouble(), 0);
    EXPECT_TRUE(torsion.setData(torsion.index(3, 4), "@CA-CA-CA-CA"));
    EXPECT_THAT(torsion.data(torsion.index(3, 5)).toString().toStdString(),
                testing::AnyOf(testing::Eq("k1=0.0 k2=30.334 k3=0.0"), testing::Eq("k1=0 k2=30.334 k3=0")));

    EXPECT_FALSE(torsion.setData(torsion.index(3, 6), 0.83));
    EXPECT_FALSE(torsion.setData(torsion.index(3, 7), 0.0));
}

TEST_F(SpeciesTabTest, Impropers)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    ASSERT_TRUE(dissolve.loadInput("dissolve/input/py5-ntf2.txt"));
    auto &species = coreData.species()[0];

    SpeciesImproperModel improper(species->impropers(), coreData);

    // Test Torsions
    EXPECT_EQ(improper.columnCount(), 6);
    EXPECT_EQ(improper.rowCount(), 6);
    for (auto role : roles)
    {
        EXPECT_EQ(improper.data(improper.index(3, 0), role).toInt(), 4);
        EXPECT_EQ(improper.data(improper.index(3, 1), role).toInt(), 3);
        EXPECT_EQ(improper.data(improper.index(3, 2), role).toInt(), 5);
        EXPECT_EQ(improper.data(improper.index(3, 3), role).toInt(), 9);
        EXPECT_EQ(improper.data(improper.index(3, 4), role).toString().toStdString(), "@impgeneral");
        EXPECT_THAT(improper.data(improper.index(3, 5)).toString().toStdString(),
                    testing::AnyOf(testing::Eq("k=4.606 n=2.0 eq=180.0 s=1.0"), testing::Eq("k=4.606 n=2 eq=180 s=1")));
    }

    // Mutate improper
    EXPECT_FALSE(improper.setData(improper.index(3, 0), 5));
    EXPECT_FALSE(improper.setData(improper.index(3, 1), 6));
    EXPECT_FALSE(improper.setData(improper.index(3, 2), 7));
    EXPECT_FALSE(improper.setData(improper.index(3, 3), 8));

    EXPECT_FALSE(improper.setData(improper.index(3, 5), "3.0, 4.0 5.0 6.0"));

    EXPECT_FALSE(improper.setData(improper.index(3, 4), "Undefined"));
    EXPECT_TRUE(improper.setData(improper.index(3, 4), "Cos3"));

    EXPECT_FALSE(improper.setData(improper.index(3, 5), "3.0 4.0 5.0 6.0"));
    EXPECT_FALSE(improper.setData(improper.index(3, 5), "3.0 4.0 5.0 6.0 7.0 8.0 8.0"));
    EXPECT_TRUE(improper.setData(improper.index(3, 5), "3.0 4.0 5.0"));
    EXPECT_THAT(improper.data(improper.index(3, 5)).toString().toStdString(),
                testing::AnyOf(testing::Eq("k1=3.0 k2=4.0 k3=5.0"), testing::Eq("k1=3 k2=4 k3=5")));

    EXPECT_TRUE(improper.setData(improper.index(3, 4), "@impgeneral"));
    EXPECT_THAT(improper.data(improper.index(3, 5)).toString().toStdString(),
                testing::AnyOf(testing::Eq("k=4.606 n=2.0 eq=180.0 s=1.0"), testing::Eq("k=4.606 n=2 eq=180 s=1")));
}

TEST_F(SpeciesTabTest, Isotopologues)
{

    CoreData coreData;
    Dissolve dissolve(coreData);

    ASSERT_TRUE(dissolve.loadInput("dissolve/input/water.txt"));
    auto &species = coreData.species()[0];

    SpeciesIsoModel isos(*species);

    // Test the top level branches of the tree
    EXPECT_EQ(isos.columnCount(), 3);
    EXPECT_EQ(isos.rowCount(), 1);
    EXPECT_EQ(isos.data(isos.index(0, 1)), QVariant());
    auto naturalIndex = isos.index(0, 0);
    EXPECT_EQ(isos.data(naturalIndex).toString().toStdString(), "Deuteriated");
    EXPECT_TRUE(isos.setData(naturalIndex, "Example"));
    EXPECT_EQ(isos.data(naturalIndex).toString().toStdString(), "Example");
    EXPECT_TRUE(isos.hasChildren(naturalIndex));
    EXPECT_EQ(naturalIndex.internalId(), 0);
    EXPECT_EQ(isos.rowCount(naturalIndex), 2);
    EXPECT_EQ(isos.index(0, 1, naturalIndex).internalId(), 1);
    EXPECT_EQ(isos.index(1, 2, naturalIndex).internalId(), 1);
    EXPECT_EQ(isos.parent(isos.index(1, 2, naturalIndex)), naturalIndex);

    // Check Display
    EXPECT_EQ(isos.data(isos.index(0, 1, naturalIndex)).toString().toStdString(), "OW");
    EXPECT_EQ(isos.data(isos.index(0, 2, naturalIndex)).toString().toStdString(), "Natural (bc = 5.803 fm)");
    EXPECT_EQ(isos.data(isos.index(1, 1, naturalIndex)).toString().toStdString(), "HW");
    EXPECT_EQ(isos.data(isos.index(1, 2, naturalIndex)).toString().toStdString(), "2 (bc = 6.671 fm)");

    // Update Isotope
    EXPECT_TRUE(isos.setData(isos.index(1, 2, naturalIndex), QVariant::fromValue(Sears91::isotope(Elements::Element(1), 0)),
                             Qt::UserRole));
    EXPECT_EQ(isos.data(isos.index(1, 2, naturalIndex)).toString().toStdString(), "Natural (bc = -3.739 fm)");
    EXPECT_FALSE(isos.setData(isos.index(1, 2, naturalIndex), QVariant::fromValue(Sears91::isotope(Elements::Element(16), 33)),
                              Qt::UserRole));
    EXPECT_EQ(isos.data(isos.index(1, 2, naturalIndex)).toString().toStdString(), "Natural (bc = -3.739 fm)");
}

} // namespace UnitTest
