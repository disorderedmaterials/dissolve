// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/procedure.h"
#include "expression/variable.h"
#include "gui/models/procedureModel.h"
#include "main/dissolve.h"
#include "procedure/nodes/add.h"
#include "procedure/nodes/select.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(ProcedureModelTest, Basic)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    // Create a simple procedure with a parameters node
    Procedure procedure;
    auto selectA = procedure.createRootNode<SelectProcedureNode>("A");
    auto &forEachA = selectA->branch()->get();
    auto selectB = forEachA.create<SelectProcedureNode>("B");
    auto &forEachB = selectB->branch()->get();
    auto selectC = forEachB.create<SelectProcedureNode>("C");

    EXPECT_TRUE(procedure.rootSequence().check());

    ProcedureModel model(procedure);

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
