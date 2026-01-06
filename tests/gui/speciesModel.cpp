// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui/models/speciesModel.h"
#include "classes/bondFunctions.h"
#include "data/elements.h"
#include "gui/models/nodeGraph/graphModel.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(SpeciesModel, Basic)
{

    // Stage 1: Simulate GUI interactions to create species
    Graph graph(nullptr);
    GraphModel graphModel;
    graphModel.setGraph(&graph);

    SpeciesModel model;
    model.assignModel(&graphModel);

    model.setName("Water");
    model.atoms()->addAtom("O", 0.156743, -0.152502, 0.317676, -0.82);
    model.atoms()->addAtom("H", -0.257130, 0.637822, 0.198948, 0.41);
    model.atoms()->addAtom("H", 0.100387, -0.485320, -0.516624, 0.41);

    model.addBond(1, 2);
    auto bonds = model.bonds();
    EXPECT_FALSE(bonds->setData(bonds->index(0, SpeciesBondModel::DataType::Form), "Harmonix"));
    EXPECT_TRUE(bonds->setData(bonds->index(0, SpeciesBondModel::DataType::Form), "Harmonic"));
    EXPECT_FALSE(bonds->setData(bonds->index(0, SpeciesBondModel::DataType::Parameters), "k="));
    EXPECT_TRUE(bonds->setData(bonds->index(0, SpeciesBondModel::DataType::Parameters), "k=4431.53 eq=1.012"));
    model.addBond(1, 3);
    EXPECT_TRUE(bonds->setData(bonds->index(1, SpeciesBondModel::DataType::Form), "Harmonic"));
    EXPECT_TRUE(bonds->setData(bonds->index(1, SpeciesBondModel::DataType::Parameters), "k=4431.53 eq=1.012"));
    model.addAngle(2, 1, 3);

    auto angles = model.angles();
    EXPECT_TRUE(angles->setData(angles->index(0, SpeciesAngleModel::DataType::Form), "Harmonic"));
    EXPECT_TRUE(angles->setData(angles->index(0, SpeciesAngleModel::DataType::Parameters), "k=317.5656 eq=113.24"));

    model.create();

    ASSERT_EQ(graph.nodes().size(), 3);

    auto &node = graph.nodes().at("Water");

    ASSERT_NE(node, nullptr);

    auto species = node->findOutput("Species")->get<const Species *>();

    // Stage 3: Confirm that species was created correctly
    ASSERT_NE(species, nullptr);
    EXPECT_EQ(species->name(), "Water");
    EXPECT_EQ(species->atom(0).Z(), Elements::Element::O);
    EXPECT_EQ(species->atom(1).Z(), Elements::Element::H);

    EXPECT_EQ(species->bonds()[0].interactionForm(), BondFunctions::Form::Harmonic);
    EXPECT_EQ(species->bonds()[1].interactionParameters()[1], 1.012);
    EXPECT_EQ(species->angles()[0].interactionForm(), AngleFunctions::Form::Harmonic);
}
} // namespace UnitTest
