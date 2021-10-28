// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/calculateangle.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "procedure/nodes/select.h"

CalculateAngleProcedureNode::CalculateAngleProcedureNode(SelectProcedureNode *site0, SelectProcedureNode *site1,
                                                         SelectProcedureNode *site2)
    : CalculateProcedureNodeBase(ProcedureNode::NodeType::CalculateAngle, site0, site1, site2)
{
    keywords_.add<NodeKeyword<SelectProcedureNode>>("Control", "I", "Site that represents 'i' in the angle i-j-k", sites_[0],
                                                    this, ProcedureNode::NodeType::Select, true);
    keywords_.add<NodeKeyword<SelectProcedureNode>>("Control", "J", "Site that represents 'j' in the angle i-j-k", sites_[1],
                                                    this, ProcedureNode::NodeType::Select, true);
    keywords_.add<NodeKeyword<SelectProcedureNode>>("Control", "K", "Site that represents 'k' in the angle i-j-k", sites_[2],
                                                    this, ProcedureNode::NodeType::Select, true);
}

/*
 * Observable Target
 */

// Return number of sites required to calculate observable
int CalculateAngleProcedureNode::nSitesRequired() const { return 3; }

// Return dimensionality of calculated observable
int CalculateAngleProcedureNode::dimensionality() const { return 1; }

/*
 * Execute
 */

// Execute node, targetting the supplied Configuration
bool CalculateAngleProcedureNode::execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                          GenericList &targetList)
{
    assert(sites_[0] && sites_[0]->currentSite());
    assert(sites_[1] && sites_[1]->currentSite());
    assert(sites_[2] && sites_[2]->currentSite());

    // Determine the value of the observable
    value_.x = cfg->box()->angleInDegrees(sites_[0]->currentSite()->origin(), sites_[1]->currentSite()->origin(),
                                          sites_[2]->currentSite()->origin());

    return true;
}
