// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "procedure/nodes/calculateexpression.h"
#include "procedure/nodes/select.h"

CalculateExpression::CalculateExpression()
    : CalculateProcedureNodeBase(ProcedureNode::NodeType::CalculateDistance, site0, site1)
{
    keywords_.add<NodeValueKeyword>("Expression", "Expression to evaluate", );
}

/*
 * Observable Target
 */

// Return number of sites required to calculate observable
int CalculateExpression::nSitesRequired() const { return 0; }

// Return dimensionality of calculated observable
int CalculateExpression::dimensionality() const { return 1; }

/*
 * Execute
 */

// Execute node
double CalculateExpression::execute() { return (asDouble()); }
