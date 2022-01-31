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
    auto selectA = std::make_shared<SelectProcedureNode>();
    auto selectB = std::make_shared<SelectProcedureNode>();
    selectA->setName("A");
    selectB->setName("B");
    auto calcAB = std::make_shared<CalculateDistanceProcedureNode>(selectA, selectB);
    auto collect = std::make_shared<Collect1DProcedureNode>(calcAB);

    collect->addSubCollectBranch(ProcedureNode::AnalysisContext);
    procedure.addRootSequenceNode(collect);

    ProcedureModel model(procedure);

    // Check out model root
    EXPECT_EQ(model.columnCount(QModelIndex()), 1);
    EXPECT_EQ(model.rowCount(QModelIndex()), 1);

    // Checkout out first child
    auto child = model.index(0, 0);
    EXPECT_EQ(model.columnCount(child), 1);
    EXPECT_EQ(model.rowCount(child), 1);
    EXPECT_EQ(model.data(child, Qt::DisplayRole).toString().toStdString(), "Collect1D");
    EXPECT_EQ(model.data(child, Qt::UserRole).value<NodeRef>(), collect);

    EXPECT_EQ(model.parent(child), QModelIndex());

    auto grandchild = model.index(0, 0, child);
    EXPECT_NE(grandchild.internalPointer(), child.internalPointer());
    EXPECT_NE(model.parent(grandchild), child);
    EXPECT_EQ(model.rowCount(grandchild), 0);
    EXPECT_EQ(model.data(grandchild, Qt::DisplayRole).toString().toStdString(), "Sequence");
}

} // namespace UnitTest
