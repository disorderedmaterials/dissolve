// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "procedure/nodes/operateexpression.h"
#include "expression/variable.h"
#include "keywords/expression.h"
#include "math/data1d.h"
#include "math/data2d.h"
#include "math/data3d.h"
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
    xDelta_ = std::make_shared<ExpressionVariable>("xDelta");
    variables_.emplace_back(xDelta_);
    yDelta_ = std::make_shared<ExpressionVariable>("yDelta");
    variables_.emplace_back(yDelta_);
    zDelta_ = std::make_shared<ExpressionVariable>("zDelta");
    variables_.emplace_back(zDelta_);
    value_ = std::make_shared<ExpressionVariable>("value");
    variables_.emplace_back(value_);

    expression_.create(expressionText, variables_);

    keywords_.add<ExpressionKeyword>("Control", "Expression", "Expression to apply to values", expression_, variables_);
}

// Set the expression
bool OperateExpressionProcedureNode::setExpression(std::string_view expressionText)
{
    return expression_.create(expressionText, variables_);
}

// Zero all variables
void OperateExpressionProcedureNode::zeroVariables()
{
    for (auto &v : variables_)
        v->setValue(0.0);
}

/*
 * Data Target (implements virtuals in OperateProcedureNodeBase)
 */

// Operate on Data1D target
bool OperateExpressionProcedureNode::operateData1D(const ProcessPool &procPool, Configuration *cfg)
{
    zeroVariables();

    const auto &x = targetData1D_->xAxis();
    auto &values = targetData1D_->values();

    // Set data-related quantities
    if (x.size() > 1)
        xDelta_->setValue(x[1] - x[0]);
    else
        Messenger::warn("OperateExpression: Not enough data to calculate x delta ({}).\n", targetData1D_->tag());

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
bool OperateExpressionProcedureNode::operateData2D(const ProcessPool &procPool, Configuration *cfg)
{
    zeroVariables();

    const auto &x = targetData2D_->xAxis();
    const auto &y = targetData2D_->yAxis();
    auto &values = targetData2D_->values();

    // Set data-related quantities
    if (x.size() > 1)
        xDelta_->setValue(x[1] - x[0]);
    else
        Messenger::warn("OperateExpression: Not enough data to calculate x delta ({}).\n", targetData2D_->tag());
    if (y.size() > 1)
        yDelta_->setValue(y[1] - y[0]);
    else
        Messenger::warn("OperateExpression: Not enough data to calculate y delta ({}).\n", targetData2D_->tag());

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
bool OperateExpressionProcedureNode::operateData3D(const ProcessPool &procPool, Configuration *cfg)
{
    zeroVariables();

    const auto &x = targetData3D_->xAxis();
    const auto &y = targetData3D_->yAxis();
    const auto &z = targetData3D_->zAxis();
    auto &values = targetData3D_->values();

    // Set data-related quantities
    if (x.size() > 1)
        xDelta_->setValue(x[1] - x[0]);
    else
        Messenger::warn("OperateExpression: Not enough data to calculate x delta ({}).\n", targetData3D_->tag());
    if (y.size() > 1)
        yDelta_->setValue(y[1] - y[0]);
    else
        Messenger::warn("OperateExpression: Not enough data to calculate y delta ({}).\n", targetData3D_->tag());
    if (z.size() > 1)
        zDelta_->setValue(y[1] - y[0]);
    else
        Messenger::warn("OperateExpression: Not enough data to calculate z delta ({}).\n", targetData3D_->tag());

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
