// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/dissolveModel.h"
#include "gui/models/nodeGraph/exampleGraphModel.h"
#include "gui/models/nodeGraph/generatorGraphModel.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <qnamespace.h>

namespace UnitTest
{

TEST(GraphModelTest, ExampleGraphModel)
{
    ExampleGraphModel model;

    GraphEdgeModel &edges = *model.edges();
    auto &nodes = *model.nodes();

    EXPECT_EQ(nodes.rowCount(), 0);
    EXPECT_EQ(edges.rowCount(), 0);

    model.emplace_back(100, 300, 7.5);
    model.items().back().rawValue().name = "First";
    model.emplace_back(600, 400, {});
    model.items().back().rawValue().name = "Last";
    model.connect("First", 0, "Last", 0);

    EXPECT_EQ(std::get<double>(model.items()[0].rawValue().value), 7.5);

    EXPECT_EQ(nodes.rowCount(), 2);
    EXPECT_EQ(model.nEdges(), 1);
    EXPECT_EQ(edges.rowCount(), 1);

    EXPECT_EQ(model.disconnect("First", 0, "Last", 0), true);
    EXPECT_EQ(model.nEdges(), 0);
    EXPECT_EQ(edges.rowCount(), 0);
    EXPECT_EQ(nodes.rowCount(), 2);
    model.connect("First", 0, "Last", 0);

    EXPECT_EQ(edges.data(nodes.index(0, 0), Qt::UserRole).toString(), "First");
    EXPECT_EQ(edges.data(nodes.index(0, 0), Qt::UserRole + 1).toInt(), 0);
    EXPECT_EQ(edges.data(nodes.index(0, 0), Qt::UserRole + 2).toString(), "Last");
    EXPECT_EQ(edges.data(nodes.index(0, 0), Qt::UserRole + 3).toInt(), 0);

    EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 1).toInt(), 100);
    EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 2).toInt(), 300);
    EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 3).toString(), "number");
    EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 4).toString(), "qrc:/Dissolve/icons/open.svg");
    EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 5).toDouble(), 7.5);

    EXPECT_EQ(nodes.data(nodes.index(1, 0), Qt::UserRole + 1).toInt(), 600);
    EXPECT_EQ(nodes.data(nodes.index(1, 0), Qt::UserRole + 2).toInt(), 400);
    EXPECT_EQ(nodes.data(nodes.index(1, 0), Qt::UserRole + 3).toString(), "ptr");
    EXPECT_EQ(nodes.data(nodes.index(1, 0), Qt::UserRole + 4).toString(), "qrc:/Dissolve/icons/cross.svg");
    EXPECT_EQ(nodes.data(nodes.index(1, 0), Qt::UserRole + 5).toDouble(), 7.5);

    model.deleteNode("Last");
    EXPECT_EQ(nodes.rowCount(), 1);
    EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 1).toInt(), 100);
    EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 2).toInt(), 300);
    EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 3).toString(), "number");
    EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 4).toString(), "qrc:/Dissolve/icons/open.svg");
    EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 5).toDouble(), 7.5);
}

TEST(GraphModelTest, GeneratorGraphModel)
{
    DissolveModel dissolveModel;
    CoreData coreData;
    Dissolve dissolve(coreData);
    dissolveModel.setDissolve(dissolve);

    GeneratorGraphModel ggm;
    ggm.setWorld(&dissolveModel);

    dissolveModel.loadInput(QUrl("file:dissolve/input/broadening-argon.txt"));
    ggm.handleReset();

    EXPECT_EQ(ggm.nEdges(), 4);

    auto nodes = ggm.nodes();
    EXPECT_EQ(nodes->rowCount(), 5);

    // Flip the hash to get the roles by name
    QHash<QByteArray, int> flipped;
    for (auto [k, v] : nodes->roleNames().asKeyValueRange())
        flipped.insert(v, k);

    // Ensure that necessary roeslare present
    ASSERT_NE(flipped["type"], 0);
    ASSERT_NE(flipped["name"], 0);
    ASSERT_NE(flipped["icon"], 0);
    ASSERT_NE(flipped["temperature"], 0);
    ASSERT_NE(flipped["atomicDensity"], 0);

    // Bulk Configuration
    EXPECT_EQ(nodes->data(nodes->index(0, 0), flipped["type"]).toString().toStdString(), std::string("Configuration"));
    EXPECT_EQ(nodes->data(nodes->index(0, 0), flipped["name"]).toString().toStdString(), std::string("liquid"));
    EXPECT_EQ(nodes->data(nodes->index(0, 0), flipped["temperature"]).toDouble(), 300.0);
    EXPECT_EQ(nodes->data(nodes->index(0, 0), flipped["atomicDensity"]).toDouble(), 0.0);

    // Main Generator
    EXPECT_EQ(nodes->data(nodes->index(1, 0), flipped["type"]).toString().toStdString(), std::string("Generator"));

    // Box Node
    EXPECT_EQ(nodes->data(nodes->index(2, 0), flipped["type"]).toString().toStdString(), std::string("GeneratorNode"));
    EXPECT_EQ(nodes->data(nodes->index(2, 0), flipped["icon"]).toString().toStdString(), "qrc:/Dissolve/icons/nodes/Box.svg");

    // Add Node
    EXPECT_EQ(nodes->data(nodes->index(3, 0), flipped["type"]).toString().toStdString(), std::string("GeneratorNode"));
    EXPECT_EQ(nodes->data(nodes->index(3, 0), flipped["icon"]).toString().toStdString(), "qrc:/Dissolve/icons/nodes/Add.svg");

    // Import Node
    EXPECT_EQ(nodes->data(nodes->index(4, 0), flipped["type"]).toString().toStdString(), std::string("GeneratorNode"));
    EXPECT_EQ(nodes->data(nodes->index(4, 0), flipped["icon"]).toString().toStdString(),
              "qrc:/Dissolve/icons/nodes/ImportCoordinates.svg");
}

} // namespace UnitTest
