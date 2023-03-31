// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/calculatedistanceexpression.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "procedure/nodes/select.h"

CalculateDistanceExpression::CalculateDistanceExpression()
    : CalculateProcedureNodeBase(ProcedureNode::NodeType::CalculateDistance, site0, site1)
{
    keywords_.add<NodeValueKeyword>("Distance Expression", "Expression that represents the distance between 2 sites", );
}

/*
 * Observable Target
 */

// Return number of sites required to calculate observable
int CalculateDistanceExpression::nSitesRequired() const { return 2; }

// Return dimensionality of calculated observable
int CalculateDistanceExpression::dimensionality() const { return 1; }

/*
 * Execute
 */

// Execute node
bool CalculateDistanceExpression::execute(const ProcedureContext &procedureContext)
{

    assert(sites_[0] && sites_[0]->currentSite());
    assert(sites_[1] && sites_[1]->currentSite());

    // Determine the value of the observable
    value_.x = procedureContext.configuration()->box()->minimumDistance(sites_[0]->currentSite()->origin(),
                                                                        sites_[1]->currentSite()->origin());

    return true;
}
