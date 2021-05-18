// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/species.h"
#include "gui/models/speciesAngleModel.h"
#include "gui/models/speciesAtomModel.h"
#include "gui/models/speciesBondModel.h"
#include "gui/models/speciesImproperModel.h"
#include "gui/models/speciesTorsionModel.h"
#include "main/dissolve.h"
#include <gtest/gtest.h>
#include <vector>

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

    dissolve.clear();
    dissolve.loadInput("restart/benzene.txt");
    auto &species = dissolve.species()[0];

    SpeciesAtomModel atom(species->atoms(), dissolve);

    // Test Atoms
    EXPECT_EQ(atom.columnCount(), 6);
    EXPECT_EQ(atom.rowCount(), 12);

    // Test Carbon atom
    for (auto role : roles)
    {
        EXPECT_EQ(atom.data(atom.index(0, 0), role).toString().toStdString(), "Carbon");
        EXPECT_EQ(atom.data(atom.index(0, 1), role).toString().toStdString(), "CA");
        EXPECT_DOUBLE_EQ(atom.data(atom.index(0, 2), role).toDouble(), -1.399);
        EXPECT_DOUBLE_EQ(atom.data(atom.index(0, 3), role).toDouble(), 0.1600);
        EXPECT_DOUBLE_EQ(atom.data(atom.index(0, 4), role).toDouble(), 0.000);
        EXPECT_DOUBLE_EQ(atom.data(atom.index(0, 5), role).toDouble(), -0.115);

        // Test Hydrogen Atom
        EXPECT_EQ(atom.data(atom.index(6, 0), role).toString().toStdString(), "Hydrogen");
        EXPECT_EQ(atom.data(atom.index(6, 1), role).toString().toStdString(), "HA");
        EXPECT_DOUBLE_EQ(atom.data(atom.index(6, 2), role).toDouble(), 1.483);
        EXPECT_DOUBLE_EQ(atom.data(atom.index(6, 3), role).toDouble(), 2.001);
        EXPECT_DOUBLE_EQ(atom.data(atom.index(6, 4), role).toDouble(), 0.000);
        EXPECT_DOUBLE_EQ(atom.data(atom.index(6, 5), role).toDouble(), 0.115);
    }

    // Mutate Hydrogen
    EXPECT_FALSE(atom.setData(atom.index(6, 0), "Carbon"));
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

    dissolve.clear();
    dissolve.loadInput("restart/benzene.txt");
    auto &species = dissolve.species()[0];

    SpeciesBondModel bond(species->bonds(), dissolve);

    // Test Bonds
    EXPECT_EQ(bond.columnCount(), 4);
    EXPECT_EQ(bond.rowCount(), 12);

    for (auto role : roles)
    {
        EXPECT_EQ(bond.data(bond.index(3, 0), role).toInt(), 4);
        EXPECT_EQ(bond.data(bond.index(3, 1), role).toInt(), 5);
        EXPECT_EQ(bond.data(bond.index(3, 2), role).toString().toStdString(), "@CA-CA");
        EXPECT_EQ(bond.data(bond.index(3, 3), role).toString().toStdString(), "3924.590000,1.400000");
    }

    // Mutate bond
    EXPECT_FALSE(bond.setData(bond.index(3, 0), 5));
    EXPECT_FALSE(bond.setData(bond.index(3, 1), 6));

    EXPECT_FALSE(bond.setData(bond.index(3, 3), "4.000000,5.000000"));

    EXPECT_FALSE(bond.setData(bond.index(3, 2), "Undefined"));
    EXPECT_TRUE(bond.setData(bond.index(3, 2), "Harmonic"));

    EXPECT_TRUE(bond.setData(bond.index(3, 3), "4.0,5.0"));
    EXPECT_EQ(bond.data(bond.index(3, 3)).toString().toStdString(), "4.000000,5.000000");

    EXPECT_TRUE(bond.setData(bond.index(3, 2), "@CA-CA"));
    EXPECT_EQ(bond.data(bond.index(3, 3)).toString().toStdString(), "3924.590000,1.400000");
}

TEST_F(SpeciesTabTest, Angles)
{

    CoreData coreData;
    Dissolve dissolve(coreData);

    dissolve.clear();
    dissolve.loadInput("restart/benzene.txt");
    auto &species = dissolve.species()[0];

    SpeciesAngleModel angle(species->angles(), dissolve);

    // Test Angles
    EXPECT_EQ(angle.columnCount(), 5);
    EXPECT_EQ(angle.rowCount(), 18);
    for (auto role : roles)
    {
        EXPECT_EQ(angle.data(angle.index(3, 0), role).toInt(), 4);
        EXPECT_EQ(angle.data(angle.index(3, 1), role).toInt(), 5);
        EXPECT_EQ(angle.data(angle.index(3, 2), role).toInt(), 6);
        EXPECT_EQ(angle.data(angle.index(3, 3), role).toString().toStdString(), "@CA-CA-CA");
        EXPECT_EQ(angle.data(angle.index(3, 4), role).toString().toStdString(), "527.184000,120.000000");
    }

    // Mutate angle
    EXPECT_FALSE(angle.setData(angle.index(3, 0), 5));
    EXPECT_FALSE(angle.setData(angle.index(3, 1), 6));
    EXPECT_FALSE(angle.setData(angle.index(3, 2), 7));
    EXPECT_FALSE(angle.setData(angle.index(3, 4), 6));

    EXPECT_FALSE(angle.setData(angle.index(3, 3), "Undefined"));
    EXPECT_TRUE(angle.setData(angle.index(3, 3), "Harmonic"));

    EXPECT_TRUE(angle.setData(angle.index(3, 4), "4.0,5.0"));
    EXPECT_EQ(angle.data(angle.index(3, 4)).toString().toStdString(), "4.000000,5.000000");

    EXPECT_TRUE(angle.setData(angle.index(3, 3), "@CA-CA-CA"));
    EXPECT_EQ(angle.data(angle.index(3, 4)).toString().toStdString(), "527.184000,120.000000");
}

TEST_F(SpeciesTabTest, Torsions)
{

    CoreData coreData;
    Dissolve dissolve(coreData);

    dissolve.clear();
    dissolve.loadInput("restart/benzene.txt");
    auto &species = dissolve.species()[0];

    SpeciesTorsionModel torsion(species->torsions(), dissolve);

    // Test Torsions
    EXPECT_EQ(torsion.columnCount(), 9);
    EXPECT_EQ(torsion.rowCount(), 24);
    for (auto role : roles)
    {
        EXPECT_EQ(torsion.data(torsion.index(3, 0), role).toInt(), 6);
        EXPECT_EQ(torsion.data(torsion.index(3, 1), role).toInt(), 1);
        EXPECT_EQ(torsion.data(torsion.index(3, 2), role).toInt(), 2);
        EXPECT_EQ(torsion.data(torsion.index(3, 3), role).toInt(), 3);
        EXPECT_EQ(torsion.data(torsion.index(3, 4), role).toString().toStdString(), "@CA-CA-CA-CA");
        EXPECT_EQ(torsion.data(torsion.index(3, 5), role).toDouble(), 0);
        EXPECT_DOUBLE_EQ(torsion.data(torsion.index(3, 6), role).toDouble(), 30.334);
        EXPECT_EQ(torsion.data(torsion.index(3, 7), role).toDouble(), 0);
        EXPECT_EQ(torsion.data(torsion.index(3, 8), role).toDouble(), 0);
    }

    // Mutate torsion
    EXPECT_FALSE(torsion.setData(torsion.index(3, 0), 5));
    EXPECT_FALSE(torsion.setData(torsion.index(3, 1), 6));
    EXPECT_FALSE(torsion.setData(torsion.index(3, 2), 7));
    EXPECT_FALSE(torsion.setData(torsion.index(3, 3), 8));
    for (auto i = 5; i < 9; ++i)
        EXPECT_FALSE(torsion.setData(torsion.index(3, i), 6));

    EXPECT_FALSE(torsion.setData(torsion.index(3, 4), "Undefined"));
    EXPECT_TRUE(torsion.setData(torsion.index(3, 4), "Cos3"));
    for (auto i = 5; i < 8; ++i)
    {
        EXPECT_TRUE(torsion.setData(torsion.index(3, i), i));
        EXPECT_DOUBLE_EQ(torsion.data(torsion.index(3, i)).toDouble(), i);
    }
    EXPECT_FALSE(torsion.setData(torsion.index(3, 8), 8));
    EXPECT_EQ(torsion.data(torsion.index(3, 8)).toDouble(), 0);
    EXPECT_TRUE(torsion.setData(torsion.index(3, 4), "@CA-CA-CA-CA"));
    EXPECT_EQ(torsion.data(torsion.index(3, 5)).toDouble(), 0);
    EXPECT_DOUBLE_EQ(torsion.data(torsion.index(3, 6)).toDouble(), 30.334);
    EXPECT_EQ(torsion.data(torsion.index(3, 7)).toDouble(), 0);
    EXPECT_EQ(torsion.data(torsion.index(3, 8)).toDouble(), 0);
}

TEST_F(SpeciesTabTest, Impropers)
{

    CoreData coreData;
    Dissolve dissolve(coreData);

    dissolve.clear();
    dissolve.loadInput("energyforce4/py5-ntf2.txt");
    auto &species = dissolve.species()[0];

    SpeciesImproperModel improper(species->impropers(), dissolve);

    // Test Torsions
    EXPECT_EQ(improper.columnCount(), 9);
    EXPECT_EQ(improper.rowCount(), 6);
    for (auto role : roles)
    {
        EXPECT_EQ(improper.data(improper.index(3, 0), role).toInt(), 4);
        EXPECT_EQ(improper.data(improper.index(3, 1), role).toInt(), 3);
        EXPECT_EQ(improper.data(improper.index(3, 2), role).toInt(), 5);
        EXPECT_EQ(improper.data(improper.index(3, 3), role).toInt(), 9);
        EXPECT_EQ(improper.data(improper.index(3, 4), role).toString().toStdString(), "@impgeneral");
        EXPECT_EQ(improper.data(improper.index(3, 5), role).toDouble(), 4.606);
        EXPECT_DOUBLE_EQ(improper.data(improper.index(3, 6), role).toDouble(), 2.0);
        EXPECT_DOUBLE_EQ(improper.data(improper.index(3, 7), role).toDouble(), 180.0);
        EXPECT_DOUBLE_EQ(improper.data(improper.index(3, 8), role).toDouble(), 1.00);
    }

    // Mutate improper
    EXPECT_FALSE(improper.setData(improper.index(3, 0), 5));
    EXPECT_FALSE(improper.setData(improper.index(3, 1), 6));
    EXPECT_FALSE(improper.setData(improper.index(3, 2), 7));
    EXPECT_FALSE(improper.setData(improper.index(3, 3), 8));
    for (auto i = 5; i < 9; ++i)
        EXPECT_FALSE(improper.setData(improper.index(3, i), 6));

    EXPECT_FALSE(improper.setData(improper.index(3, 4), "Undefined"));
    EXPECT_TRUE(improper.setData(improper.index(3, 4), "Cos3"));
    for (auto i = 5; i < 9; ++i)
    {
        EXPECT_TRUE(improper.setData(improper.index(3, i), i));
        EXPECT_DOUBLE_EQ(improper.data(improper.index(3, i)).toDouble(), i);
    }
    EXPECT_TRUE(improper.setData(improper.index(3, 4), "@impgeneral"));
    EXPECT_EQ(improper.data(improper.index(3, 5)).toDouble(), 4.606);
    EXPECT_DOUBLE_EQ(improper.data(improper.index(3, 6)).toDouble(), 2.0);
    EXPECT_DOUBLE_EQ(improper.data(improper.index(3, 7)).toDouble(), 180.0);
    EXPECT_DOUBLE_EQ(improper.data(improper.index(3, 8)).toDouble(), 1.00);
}

} // namespace UnitTest
