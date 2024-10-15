// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/nodeGraph/exampleGraphModel.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <qnamespace.h>

namespace UnitTest
{

TEST(GraphModelTest, GraphModel)
{
    ExampleGraphModel model;

    GraphEdgeModel &edges = *model.edges();
    auto &nodes = *model.nodes();

    EXPECT_EQ(nodes.rowCount(), 0);
    EXPECT_EQ(edges.rowCount(), 0);

    model.emplace_back(100, 300, 7.5);
    model.emplace_back(600, 400, {});
    model.connect(0, 0, 1, 0);

    EXPECT_EQ(std::get<double>(model.items[0].rawValue().value), 7.5);

    EXPECT_EQ(nodes.rowCount(), 2);
    EXPECT_EQ(model.nEdges(), 1);
    EXPECT_EQ(edges.rowCount(), 1);

    EXPECT_EQ(model.disconnect(0, 0, 1, 0), true);
    EXPECT_EQ(model.nEdges(), 0);
    EXPECT_EQ(edges.rowCount(), 0);
    EXPECT_EQ(nodes.rowCount(), 2);
    model.connect(0, 0, 1, 0);

    EXPECT_EQ(edges.data(nodes.index(0, 0), Qt::UserRole).toInt(), 0);
    EXPECT_EQ(edges.data(nodes.index(0, 0), Qt::UserRole + 1).toInt(), 0);
    EXPECT_EQ(edges.data(nodes.index(0, 0), Qt::UserRole + 2).toInt(), 1);
    EXPECT_EQ(edges.data(nodes.index(0, 0), Qt::UserRole + 3).toInt(), 0);

    EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 1).toInt(), 100);
    EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 2).toInt(), 300);
    EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 3).toString(), "number");
    EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 4).toString(),
              "file:/home/adam/Code/dissolve/src/gui/icons/open.svg");
    EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 5).toDouble(), 7.5);

    EXPECT_EQ(nodes.data(nodes.index(1, 0), Qt::UserRole + 1).toInt(), 600);
    EXPECT_EQ(nodes.data(nodes.index(1, 0), Qt::UserRole + 2).toInt(), 400);
    EXPECT_EQ(nodes.data(nodes.index(1, 0), Qt::UserRole + 3).toString(), "ptr");
    EXPECT_EQ(nodes.data(nodes.index(1, 0), Qt::UserRole + 4).toString(),
              "file:/home/adam/Code/dissolve/src/gui/icons/open.svg");
    EXPECT_EQ(nodes.data(nodes.index(1, 0), Qt::UserRole + 5).toDouble(), 7.5);

    model.deleteNode(1);
    EXPECT_EQ(nodes.rowCount(), 1);
    EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 1).toInt(), 100);
    EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 2).toInt(), 300);
    EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 3).toString(), "number");
    EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 4).toString(),
              "file:/home/adam/Code/dissolve/src/gui/icons/open.svg");
    EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 5).toDouble(), 7.5);
}

} // namespace UnitTest
