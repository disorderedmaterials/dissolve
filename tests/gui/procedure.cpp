// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "expression/variable.h"
#include "generator/add.h"
#include "generator/generator.h"
#include "generator/select.h"
#include "gui/models/generatorModel.h"
#include "main/dissolve.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(GeneratorModelTest, Basic)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    // Create a simple generator with a parameters node
    Generator generator;
    auto selectA = generator.createRootNode<SelectGeneratorNode>("A");
    auto &forEachA = selectA->branch()->get();
    auto selectB = forEachA.create<SelectGeneratorNode>("B");
    auto &forEachB = selectB->branch()->get();
    auto selectC = forEachB.create<SelectGeneratorNode>("C");

    EXPECT_TRUE(generator.rootSequence().check());

    GeneratorModel model(generator);

    // Check out model root
    EXPECT_EQ(model.columnCount(QModelIndex()), 1);
    EXPECT_EQ(model.rowCount(QModelIndex()), 1);

    // Check out "SelectA" child in root sequence
    auto selectAChild = model.index(0, 0);
    EXPECT_EQ(model.columnCount(selectAChild), 1);
    EXPECT_EQ(model.rowCount(selectAChild), 1);
    EXPECT_EQ(model.data(selectAChild, Qt::DisplayRole).toString().toStdString(), "A (Select)");
    EXPECT_EQ(model.data(selectAChild, Qt::UserRole).value<NodeRef>(), selectA);

    // Check out "SelectB" child in SelectA's ForEach branch (via column 1)
    auto selectBChild = model.index(0, 0, selectAChild);
    EXPECT_EQ(model.columnCount(selectBChild), 1);
    EXPECT_EQ(model.rowCount(selectBChild), 1);
    EXPECT_EQ(model.data(selectBChild, Qt::DisplayRole).toString().toStdString(), "B (Select)");
    EXPECT_EQ(model.data(selectBChild, Qt::UserRole).value<NodeRef>(), selectB);

    // Check out "SelectC" child in SelectA's ForEach branch (via column 0)
    auto selectCChild = model.index(0, 0, selectBChild);
    EXPECT_EQ(model.columnCount(selectCChild), 1);
    EXPECT_EQ(model.rowCount(selectCChild), 0);
    EXPECT_EQ(model.data(selectCChild, Qt::DisplayRole).toString().toStdString(), "C (Select)");
    EXPECT_EQ(model.data(selectCChild, Qt::UserRole).value<NodeRef>(), selectC);
}

} // namespace UnitTest
