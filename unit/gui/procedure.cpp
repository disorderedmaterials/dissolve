// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "procedure/procedure.h"
#include "expression/variable.h"
#include "gui/models/procedureModel.h"
#include "main/dissolve.h"
#include "procedure/nodes/add.h"
#include "procedure/nodes/calculatedistance.h"
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
    procedure.addRootSequenceNode(calcAB);

    // Create a second set of data since we can't get the ParametersProcedureNode data non-const
    // Set up the model, supplying our mutable data and the parameters node as the parent
    ProcedureModel model(procedure);

    // Size
    EXPECT_EQ(model.columnCount(QModelIndex()), 1);
    EXPECT_EQ(model.rowCount(QModelIndex()), 1);
    EXPECT_EQ(model.columnCount(model.index(0, 0)), 1);
    EXPECT_EQ(model.rowCount(model.index(0, 0)), 2);

    // Data
    EXPECT_EQ(model.data(model.index(0, 0), Qt::DisplayRole).toString().toStdString(), "CalculateDistance");
    auto root = model.index(0, 0);
    EXPECT_EQ(model.parent(root), QModelIndex());
    EXPECT_NE(model.index(0, 0, root).internalPointer(), root.internalPointer());
    EXPECT_NE(model.parent(model.index(0, 0, root)), root);
    EXPECT_EQ(model.rowCount(model.index(0, 0, root)), 0);
    EXPECT_EQ(model.data(model.index(0, 0, root), Qt::DisplayRole).toString().toStdString(), "Select");
}

} // namespace UnitTest
