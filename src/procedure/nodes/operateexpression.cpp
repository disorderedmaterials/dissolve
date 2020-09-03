/*
    *** Procedure Node - Operate Expression
    *** src/procedure/nodes/operateexpression.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    const auto &x = targetData1D_->constXAxis();
    Array<double> &values = targetData1D_->values();

    y_->set(0.0);
    z_->set(0.0);

    // Evaluate the expression over all values
    for (int i = 0; i < x.nItems(); ++i)
    {
        // Set variables in expression
        x_->set(x.constAt(i));
        value_->set(values.at(i));

        // Evaluate and store new value
        values.at(i) = expression_.asDouble();
    }

    return true;
}

// Operate on Data2D target
bool OperateExpressionProcedureNode::operateData2D(ProcessPool &procPool, Configuration *cfg)
{
    const auto &x = targetData2D_->constXAxis();
    const auto &y = targetData2D_->constYAxis();
    Array2D<double> &values = targetData2D_->values();

    z_->set(0.0);

    // Evaluate the expression over all values
    for (int i = 0; i < x.nItems(); ++i)
    {
        // Set x value in expression
        x_->set(x.constAt(i));

        for (int j = 0; j < y.nItems(); ++j)
        {
            // Set y and value in expression
            y_->set(y.constAt(j));
            value_->set(values.at(i, j));

            // Evaluate and store new value
            values.at(i, j) = expression_.asDouble();
        }
    }

    return true;
}

// Operate on Data3D target
bool OperateExpressionProcedureNode::operateData3D(ProcessPool &procPool, Configuration *cfg)
{
    const auto &x = targetData3D_->constXAxis();
    const auto &y = targetData3D_->constYAxis();
    const auto &z = targetData3D_->constZAxis();
    Array3D<double> &values = targetData3D_->values();

    z_->set(0.0);

    // Evaluate the expression over all values
    for (int i = 0; i < x.nItems(); ++i)
    {
        // Set x value in expression
        x_->set(x.constAt(i));

        for (int j = 0; j < y.nItems(); ++j)
        {
            // Set y value in expression
            y_->set(y.constAt(j));

            for (int k = 0; k < z.nItems(); ++k)
            {
                // Set z and  value in expression
                z_->set(z.constAt(k));
                value_->set(values.at(i, j, k));

                // Evaluate and store new value
                values.at(i, j, k) = expression_.asDouble();
            }
        }
    }

    return true;
}
