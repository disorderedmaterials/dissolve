// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/nodes.h"
#include "procedure/procedure.h"
#include "src/math/constants.h"
#include <gtest/gtest.h>
#include <string>

namespace UnitTest
{

TEST(CalculateExpressionTest, Basic)
{
    Procedure procedure(ProcedureNode::AnalysisContext);

    // No expression set
    auto expressionNode = procedure.createRootNode<CalculateExpressionProcedureNode>("X");
    EXPECT_TRUE(procedure.rootSequence().check());

    // Simple number
    expressionNode->keywords().set("Expression", NodeValue(4));
    expressionNode->execute();
    EXPECT_DOUBLE_EQ(expressionNode->value(0), 4.0);

    // Expresssions
    expressionNode->keywords().set("Expression", NodeValue("3.8 * sin(1.2)"));
    expressionNode->execute();
    EXPECT_DOUBLE_EQ(expressionNode->value(0), 3.8 * sin(1 / DEGRAD * 1.2));
}

} // namespace UnitTest
