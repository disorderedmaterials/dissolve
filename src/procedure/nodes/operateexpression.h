/*
    *** Procedure Node - Operate Expression
    *** src/procedure/nodes/operateexpression.h
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

#pragma once

#include "expression/expression.h"
#include "procedure/nodes/operatebase.h"

// Forward Declarations
/* none */

// Operate Expression Node
class OperateExpressionProcedureNode : public OperateProcedureNodeBase
{
    public:
    OperateExpressionProcedureNode(std::string_view expressionText = "");
    ~OperateExpressionProcedureNode();

    /*
     * Expression and Variables
     */
    private:
    // Normalisation expression
    Expression expression_;
    // X variable in equation
    ExpressionVariable *x_;
    // Y variable in equation
    ExpressionVariable *y_;
    // Z variable in equation
    ExpressionVariable *z_;
    // Value variable in equation
    ExpressionVariable *value_;

    /*
     * Data Target (implements virtuals in OperateProcedureNodeBase)
     */
    public:
    // Operate on Data1D target
    bool operateData1D(ProcessPool &procPool, Configuration *cfg);
    // Operate on Data2D target
    bool operateData2D(ProcessPool &procPool, Configuration *cfg);
    // Operate on Data3D target
    bool operateData3D(ProcessPool &procPool, Configuration *cfg);
};
