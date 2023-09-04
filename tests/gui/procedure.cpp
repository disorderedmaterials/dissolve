// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/procedure.h"
#include "expression/variable.h"
#include "gui/models/procedureModel.h"
#include "main/dissolve.h"
#include "procedure/nodes/add.h"
#include "procedure/nodes/calculateDistance.h"
#include "procedure/nodes/collect1D.h"
#include "procedure/nodes/select.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(ProcedureModelTest, Basic)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    // Create a simple procedure with a parameters node
    Procedure procedure(ProcedureNode::AnalysisContext);
    auto selectA = procedure.createRootNode<SelectProcedureNode>("A");
    auto &forEachA = selectA->branch()->get();
    auto selectB = forEachA.create<SelectProcedureNode>("B");
    auto &forEachB = selectB->branch()->get();
    auto selectC = forEachB.create<SelectProcedureNode>("C");
    auto calcAB = procedure.createRootNode<CalculateDistanceProcedureNode>({}, selectA, selectB);
    auto collect = procedure.createRootNode<Collect1DProcedureNode>({}, calcAB);
    EXPECT_TRUE(procedure.rootSequence().check());

    ProcedureModel model(procedure);

    // Check out model root
    EXPECT_EQ(model.columnCount(QModelIndex()), 2);
    EXPECT_EQ(model.rowCount(QModelIndex()), 3);

    // Check out "SelectA" child in root sequence
    auto selectAChild = model.index(0, 0);
    EXPECT_EQ(model.columnCount(selectAChild), 2);
    EXPECT_EQ(model.rowCount(selectAChild), 1);
    EXPECT_EQ(model.data(selectAChild, Qt::DisplayRole).toString().toStdString(), "A (Select)");
    EXPECT_EQ(model.data(selectAChild, Qt::UserRole).value<NodeRef>(), selectA);

    // Check out "SelectB" child in SelectA's ForEach branch (via column 1)
    auto selectBChild = model.index(0, 1, selectAChild);
    EXPECT_EQ(model.columnCount(selectBChild), 2);
    EXPECT_EQ(model.rowCount(selectBChild), 1);
    EXPECT_EQ(model.data(selectBChild, Qt::DisplayRole).toString().toStdString(), "Analysis (ForEach branch in Select)");
    EXPECT_EQ(model.data(selectBChild, Qt::UserRole).value<NodeRef>(), selectB);

    // Check out "SelectC" child in SelectA's ForEach branch (via column 1)
    auto selectCChild = model.index(0, 0, selectBChild);
    EXPECT_EQ(model.columnCount(selectCChild), 2);
    EXPECT_EQ(model.rowCount(selectCChild), 0);
    EXPECT_EQ(model.data(selectCChild, Qt::DisplayRole).toString().toStdString(), "C (Select)");
    EXPECT_EQ(model.data(selectCChild, Qt::UserRole).value<NodeRef>(), selectC);

    // Check out "Collect1D" child in root sequence
    auto collectChild = model.index(2, 0);
    EXPECT_EQ(model.columnCount(collectChild), 2);
    EXPECT_EQ(model.rowCount(collectChild), 0);
    EXPECT_EQ(model.data(collectChild, Qt::DisplayRole).toString().toStdString(), "Collect1D01 (Collect1D)");
    EXPECT_EQ(model.data(collectChild, Qt::UserRole).value<NodeRef>(), collect);

    EXPECT_EQ(model.parent(collectChild).isValid(), QModelIndex().isValid());
}

} // namespace UnitTest
