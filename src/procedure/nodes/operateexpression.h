// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
    ~OperateExpressionProcedureNode() override = default;

    /*
     * Expression and Variables
     */
    private:
    // Expression
    Expression expression_;
    // Vector of variables accessible by the transform equation
    std::vector<std::shared_ptr<ExpressionVariable>> variables_;
    // Variables accessible by the transform equation
    std::shared_ptr<ExpressionVariable> x_, y_, z_, value_;

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
