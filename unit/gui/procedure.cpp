// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "procedure/procedure.h"
#include "expression/variable.h"
#include "gui/models/procedureModel.h"
#include "main/dissolve.h"
#include "procedure/nodes/add.h"
#include "procedure/nodes/parameters.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(ProcedureModelTest, Basic)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    // Create a simple procedure with a parameters node
    Procedure procedure(ProcedureNode::GenerationContext);
    auto adder = std::make_shared<AddProcedureNode>();
    auto parameters = std::make_shared<ParametersProcedureNode>();
    adder->scope()->addNode(parameters);
    procedure.addRootSequenceNode(adder);

    // Create a second set of data since we can't get the ParametersProcedureNode data non-const
    // Set up the model, supplying our mutable data and the parameters node as the parent
    ProcedureModel model(procedure);

    // Size
    EXPECT_EQ(model.columnCount(QModelIndex()), 1);
    EXPECT_EQ(model.rowCount(QModelIndex()), 1);
    EXPECT_EQ(model.columnCount(model.index(0, 0)), 1);
    EXPECT_EQ(model.rowCount(model.index(0, 0)), 1);

    // Data
    EXPECT_EQ(model.data(model.index(0, 0), Qt::DisplayRole).toString().toStdString(), "Add");
    // auto root = model.index(0, 0);
    // EXPECT_NE(model.index(0, 0, root).internalPointer(), root.internalPointer());
    // EXPECT_EQ(model.rowCount(model.index(0, 0, root)), 0);
    // EXPECT_EQ(model.data(model.index(0, 0, root), Qt::DisplayRole).toString().toStdString(), "Foo");
}

} // namespace UnitTest
