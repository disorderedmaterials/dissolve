// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "gui/models/speciesModel.h"
#include "data/elements.h"
#include "gui/models/nodeGraph/graphModel.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(SpeciesModel, Basic)
{
    SpeciesModel model;
    model.setName("Water");
    model.atoms()->addAtom("O", 0.156743, -0.152502, 0.317676, -0.82);
    model.atoms()->addAtom("H", -0.257130, 0.637822, 0.198948, 0.41);
    model.atoms()->addAtom("H", 0.100387, -0.485320, -0.516624, 0.41);

    Graph graph(nullptr);
    GraphModel graphModel;
    graphModel.setGraph(&graph);
    model.create(QVariant::fromValue(&graphModel));

    ASSERT_EQ(graph.nodes().size(), 3);

    auto &node = graph.nodes().at("Water");

    ASSERT_NE(node, nullptr);

    auto species = node->findOutput("Species")->get<const Species *>();

    ASSERT_NE(species, nullptr);
    EXPECT_EQ(species->name(), "Water");
    EXPECT_EQ(species->atom(0).Z(), Elements::Element::O);
    EXPECT_EQ(species->atom(1).Z(), Elements::Element::H);
}
} // namespace UnitTest
