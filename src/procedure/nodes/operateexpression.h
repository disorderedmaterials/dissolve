// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

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
