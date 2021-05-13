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

TEST_F(SpeciesTabTest, DataManger)
{

    CoreData coreData;
    Dissolve dissolve(coreData);

    dissolve.clear();
    dissolve.loadInput("restart/benzene.txt");
    auto &species = dissolve.species()[0];

    SpeciesAtomModel atom(species->atoms(), dissolve);
    SpeciesBondModel bond(species->bonds(), dissolve);
    SpeciesAngleModel angle(species->angles());
    SpeciesTorsionModel torsion(species->torsions());
    SpeciesImproperModel improper(species->impropers());

    // Test Atoms
    EXPECT_EQ(atom.columnCount(), 6);
    EXPECT_EQ(atom.rowCount(), 12);

    // Test Carbon atom
    EXPECT_EQ(atom.data(atom.index(0, 0)).toString().toStdString(), "Carbon");
    EXPECT_EQ(atom.data(atom.index(0, 1)).toString().toStdString(), "CA");
    EXPECT_EQ(atom.data(atom.index(0, 2)).toDouble(), -1.399);
    EXPECT_EQ(atom.data(atom.index(0, 3)).toDouble(), 0.1600);
    EXPECT_EQ(atom.data(atom.index(0, 4)).toDouble(), 0.000);
    EXPECT_EQ(atom.data(atom.index(0, 5)).toDouble(), -0.115);

    // Test Hydrogen Atom
    EXPECT_EQ(atom.data(atom.index(6, 0)).toString().toStdString(), "Hydrogen");
    EXPECT_EQ(atom.data(atom.index(6, 1)).toString().toStdString(), "HA");
    EXPECT_EQ(atom.data(atom.index(6, 2)).toDouble(), 1.483);
    EXPECT_EQ(atom.data(atom.index(6, 3)).toDouble(), 2.001);
    EXPECT_EQ(atom.data(atom.index(6, 4)).toDouble(), 0.000);
    EXPECT_EQ(atom.data(atom.index(6, 5)).toDouble(), 0.115);

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

    EXPECT_EQ(bond.columnCount(), 7);
    EXPECT_EQ(bond.rowCount(), 12);

    EXPECT_EQ(angle.columnCount(), 8);
    EXPECT_EQ(angle.rowCount(), 18);

    EXPECT_EQ(torsion.columnCount(), 9);
    EXPECT_EQ(torsion.rowCount(), 24);

    // EXPECT_EQ(model.rowCount(), 1);
    // EXPECT_EQ(model.data(model.index(0, 0)).toString().toStdString(), "benzene");
    // EXPECT_EQ(model.data(model.index(0, 1)).toString().toStdString(), "restart/benzene.txt.restart.test");
}

} // namespace UnitTest
