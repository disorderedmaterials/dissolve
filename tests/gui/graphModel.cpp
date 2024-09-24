// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/nodeGraph/graphModel.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <qnamespace.h>

namespace UnitTest
{

TEST(GraphModelTest, GraphModel)
{
  GraphModel model;

  auto& edges = *model.edges();
  auto& nodes = *model.nodes();

  EXPECT_EQ(nodes.rowCount(), 2);
  EXPECT_EQ(edges.rowCount(), 1);

  EXPECT_EQ(edges.data(nodes.index(0, 0), Qt::UserRole).toInt(),
            0);
  EXPECT_EQ(edges.data(nodes.index(0, 0), Qt::UserRole+1).toInt(),
            0);
  EXPECT_EQ(edges.data(nodes.index(0, 0), Qt::UserRole+2).toInt(),
            1);
  EXPECT_EQ(edges.data(nodes.index(0, 0), Qt::UserRole+3).toInt(),
            0);

  EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole).toString(),
            "Source");
  EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 1).toInt(),
            100);
  EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 2).toInt(),
            300);
  EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 3).toString(),
            "number");
  EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 4).toString(),
            "file:/home/adam/Code/dissolve/src/gui/icons/open.svg");
  EXPECT_EQ(nodes.data(nodes.index(0, 0), Qt::UserRole + 5).toDouble(),
            7.5);

  EXPECT_EQ(nodes.data(nodes.index(1, 0), Qt::UserRole).toString(),
            "Destination");
  EXPECT_EQ(nodes.data(nodes.index(1, 0), Qt::UserRole + 1).toInt(),
            600);
  EXPECT_EQ(nodes.data(nodes.index(1, 0), Qt::UserRole + 2).toInt(),
            400);
  EXPECT_EQ(nodes.data(nodes.index(1, 0), Qt::UserRole + 3).toString(),
            "number");
  EXPECT_EQ(nodes.data(nodes.index(1, 0), Qt::UserRole + 4).toString(),
            "file:/home/adam/Code/dissolve/src/gui/icons/open.svg");
  EXPECT_EQ(nodes.data(nodes.index(1, 0), Qt::UserRole + 5).toDouble(),
            12.5);
  
}

}
