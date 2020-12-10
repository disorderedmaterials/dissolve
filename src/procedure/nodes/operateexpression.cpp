// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "procedure/nodes/operateexpression.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "expression/variable.h"
#include "keywords/types.h"
#include "math/data1d.h"
#include "math/integrator.h"

OperateExpressionProcedureNode::OperateExpressionProcedureNode(std::string_view expressionText)
    : OperateProcedureNodeBase(ProcedureNode::OperateExpressionNode)
{
    // Set up persistent variables and initial expression value
    x_ = expression_.createDoubleVariable("x", true);
    y_ = expression_.createDoubleVariable("y", true);
    z_ = expression_.createDoubleVariable("z", true);
    value_ = expression_.createDoubleVariable("value", true);
    expression_.set(expressionText);

    keywords_.add("Expression", new ExpressionKeyword(expression_), "Expression", "Expression to apply to values");
}

OperateExpressionProcedureNode::~OperateExpressionProcedureNode() {}

/*
 * Data Target (implements virtuals in OperateProcedureNodeBase)
 */

// Operate on Data1D target
bool OperateExpressionProcedureNode::operateData1D(ProcessPool &procPool, Configuration *cfg)
{
    const auto &x = targetData1D_->xAxis();
    auto &values = targetData1D_->values();

    y_->set(0.0);
    z_->set(0.0);

    // Evaluate the expression over all values
    for (auto i = 0; i < x.size(); ++i)
    {
        // Set variables in expression
        x_->set(x[i]);
        value_->set(values.at(i));

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
    Array2D<double> &values = targetData2D_->values();

    z_->set(0.0);

    // Evaluate the expression over all values
    for (auto i = 0; i < x.size(); ++i)
    {
        // Set x value in expression
        x_->set(x[i]);

        for (auto j = 0; j < y.size(); ++j)
        {
            // Set y and value in expression
            y_->set(y[j]);
            value_->set(values[{i, j}]);

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
    Array3D<double> &values = targetData3D_->values();

    z_->set(0.0);

    // Evaluate the expression over all values
    for (auto i = 0; i < x.size(); ++i)
    {
        // Set x value in expression
        x_->set(x[i]);

        for (auto j = 0; j < y.size(); ++j)
        {
            // Set y value in expression
            y_->set(y[j]);

            for (auto k = 0; k < z.size(); ++k)
            {
                // Set z and  value in expression
                z_->set(z[k]);
                // TODO: Convert to a single loop when we have the
                // iterator combiner
                value_->set(values[{i, j, k}]);

                // Evaluate and store new value
                values[{i, j, k}] = expression_.asDouble();
            }
        }
    }

    return true;
}
