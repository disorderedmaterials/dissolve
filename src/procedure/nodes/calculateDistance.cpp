// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/calculateDistance.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "procedure/nodes/select.h"

CalculateDistanceProcedureNode::CalculateDistanceProcedureNode(std::shared_ptr<SelectProcedureNode> site0,
                                                               std::shared_ptr<SelectProcedureNode> site1)
    : CalculateProcedureNodeBase(ProcedureNode::NodeType::CalculateDistance, site0, site1)
{
    keywords_.setOrganisation("Options", "Sites");
    keywords_.add<NodeKeyword<SelectProcedureNode>>("I", "Site that represents 'i' in the distance i-j", sites_[0], this,
                                                    ProcedureNode::NodeType::Select, true);
    keywords_.add<NodeKeyword<SelectProcedureNode>>("J", "Site that represents 'j' in the distance i-j", sites_[1], this,
                                                    ProcedureNode::NodeType::Select, true);
}

/*
 * Observable Target
 */

// Return number of sites required to calculate observable
int CalculateDistanceProcedureNode::nSitesRequired() const { return 2; }

// Return dimensionality of calculated observable
int CalculateDistanceProcedureNode::dimensionality() const { return 1; }

/*
 * Execute
 */

// Execute node
bool CalculateDistanceProcedureNode::execute(const ProcedureContext &procedureContext)
{
    assert(sites_[0] && sites_[0]->currentSite());
    assert(sites_[1] && sites_[1]->currentSite());

    // Determine the value of the observable
    value_.x = procedureContext.configuration()->box()->minimumDistance(sites_[0]->currentSite()->get().origin(),
                                                                        sites_[1]->currentSite()->get().origin());

    return true;
}
