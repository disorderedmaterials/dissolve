// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/nodes.h"
#include "procedure/procedure.h"
#include <gtest/gtest.h>
#include <string>

namespace UnitTest
{

TEST(CalculateExpressionTest, Basic)
{
    Procedure procedure(ProcedureNode::AnalysisContext);

    // No expression set
    auto expressionNode = procedure.createRootNode<CalculateExpression>("X");
    EXPECT_TRUE(procedure.rootSequence().check());
}

} // namespace UnitTest
