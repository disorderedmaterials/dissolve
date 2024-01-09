// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/calculateExpression.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "keywords/nodeValue.h"
#include "procedure/nodes/select.h"

CalculateExpressionProcedureNode::CalculateExpressionProcedureNode()
    : CalculateProcedureNodeBase(ProcedureNode::NodeType::CalculateExpression)
{
    keywords_.add<NodeValueKeyword>("Expression", "Expression to evaluate", expression_, this);
}

/*
 * Observable Target
 */

// Set expression to evaluate
bool CalculateExpressionProcedureNode::setExpression(std::string_view expression)
{
    auto vars = getParameters();

    return expression_.set(expression, vars);
}

// Return number of sites required to calculate observable
int CalculateExpressionProcedureNode::nSitesRequired() const { return 0; }

// Return dimensionality of calculated observable
int CalculateExpressionProcedureNode::dimensionality() const { return 1; }

/*
 * Execute
 */

// Execute node
bool CalculateExpressionProcedureNode::execute(const ProcedureContext &procedureContext)
{
    value_.x = expression_.asDouble();
    return true;
}
