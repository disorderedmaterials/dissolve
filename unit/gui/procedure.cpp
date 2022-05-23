// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "procedure/procedure.h"
#include "expression/variable.h"
#include "gui/models/procedureModel.h"
#include "main/dissolve.h"
#include "procedure/nodes/add.h"
#include "procedure/nodes/calculatedistance.h"
#include "procedure/nodes/collect1d.h"
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
    auto selectB = procedure.createRootNode<SelectProcedureNode>("B");
    auto calcAB = procedure.createRootNode<CalculateDistanceProcedureNode>({}, selectA, selectB);
    auto collect = procedure.createRootNode<Collect1DProcedureNode>({}, calcAB);
    collect->addSubCollectBranch(ProcedureNode::AnalysisContext);

    ProcedureModel model(procedure);

    // Check out model root
    EXPECT_EQ(model.columnCount(QModelIndex()), 1);
    EXPECT_EQ(model.rowCount(QModelIndex()), 4);

    // Check out first child
    auto child = model.index(3, 0);
    EXPECT_EQ(model.columnCount(child), 1);
    EXPECT_EQ(model.rowCount(child), 1);
    EXPECT_EQ(model.data(child, Qt::DisplayRole).toString().toStdString(), "Collect1D (Collect1D01)");
    EXPECT_EQ(model.data(child, Qt::UserRole).value<NodeRef>(), collect);

    EXPECT_EQ(model.parent(child), QModelIndex());

    auto grandchild = model.index(0, 0, child);
    EXPECT_NE(grandchild.internalPointer(), child.internalPointer());
    EXPECT_NE(model.parent(grandchild), child);
    EXPECT_EQ(model.rowCount(grandchild), 0);
    EXPECT_EQ(model.data(grandchild, Qt::DisplayRole).toString().toStdString(), "Sequence");
}

} // namespace UnitTest
