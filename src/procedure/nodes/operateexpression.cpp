// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/operateexpression.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "expression/variable.h"
#include "keywords/types.h"
#include "math/data1d.h"
#include "math/integrator.h"

OperateExpressionProcedureNode::OperateExpressionProcedureNode(std::string_view expressionText)
    : OperateProcedureNodeBase(ProcedureNode::NodeType::OperateExpression)
{
    // Create variables, and add them to the vector
    x_ = std::make_shared<ExpressionVariable>("x");
    variables_.emplace_back(x_);
    y_ = std::make_shared<ExpressionVariable>("y");
    variables_.emplace_back(y_);
    z_ = std::make_shared<ExpressionVariable>("z");
    variables_.emplace_back(z_);
    value_ = std::make_shared<ExpressionVariable>("value");
    variables_.emplace_back(value_);

    expression_.create(expressionText, variables_);

    keywords_.add("Control", new ExpressionKeyword(expression_, variables_), "Expression", "Expression to apply to values");
}

/*
 * Data Target (implements virtuals in OperateProcedureNodeBase)
 */

// Operate on Data1D target
bool OperateExpressionProcedureNode::operateData1D(ProcessPool &procPool, Configuration *cfg)
{
    const auto &x = targetData1D_->xAxis();
    auto &values = targetData1D_->values();

    y_->setValue(0.0);
    z_->setValue(0.0);

    // Evaluate the expression over all values
    for (auto i = 0; i < x.size(); ++i)
    {
        // Set variables in expression
        x_->setValue(x[i]);
        value_->setValue(values.at(i));

        // Evaluate and store new value
        values.at(i) = expression_.asDouble();
    }

    return true;
}

// Operate on Data2D target
bool OperateExpressionProcedureNode::operateData2D(ProcessPool &procPool, Configuration *cfg)
{
    const auto &x = targetData2D_->xAxis();
    const auto &y = targetData2D_->yAxis();
    auto &values = targetData2D_->values();

    z_->setValue(0.0);

    // Evaluate the expression over all values
    for (auto i = 0; i < x.size(); ++i)
    {
        // Set x value in expression
        x_->setValue(x[i]);

        for (auto j = 0; j < y.size(); ++j)
        {
            // Set y and value in expression
            y_->setValue(y[j]);
            value_->setValue(values[{i, j}]);

            // Evaluate and store new value
            values[{i, j}] = expression_.asDouble();
        }
    }

    return true;
}

// Operate on Data3D target
bool OperateExpressionProcedureNode::operateData3D(ProcessPool &procPool, Configuration *cfg)
{
    const auto &x = targetData3D_->xAxis();
    const auto &y = targetData3D_->yAxis();
    const auto &z = targetData3D_->zAxis();
    auto &values = targetData3D_->values();

    z_->setValue(0.0);

    // Evaluate the expression over all values
    for (auto i = 0; i < x.size(); ++i)
    {
        // Set x value in expression
        x_->setValue(x[i]);

        for (auto j = 0; j < y.size(); ++j)
        {
            // Set y value in expression
            y_->setValue(y[j]);

            for (auto k = 0; k < z.size(); ++k)
            {
                // Set z and  value in expression
                z_->setValue(z[k]);
                // TODO: Convert to a single loop when we have the
                // iterator combiner
                value_->setValue(values[{i, j, k}]);

                // Evaluate and store new value
                values[{i, j, k}] = expression_.asDouble();
            }
        }
    }

    return true;
}
