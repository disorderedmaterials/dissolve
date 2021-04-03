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
    // Create keywords - store the pointers to the superclasses for later use
    siteKeywords_[0] = new NodeKeyword<SelectProcedureNode>(this, ProcedureNode::NodeType::Select, true, site0);
    keywords_.add("Control", siteKeywords_[0], "I", "Site that represents 'i' in the angle i-j-k");
    siteKeywords_[1] = new NodeKeyword<SelectProcedureNode>(this, ProcedureNode::NodeType::Select, true, site1);
    keywords_.add("Control", siteKeywords_[1], "J", "Site that represents 'j' in the angle i-j-k");
    siteKeywords_[2] = new NodeKeyword<SelectProcedureNode>(this, ProcedureNode::NodeType::Select, true, site2);
    keywords_.add("Control", siteKeywords_[2], "K", "Site that represents 'k' in the angle i-j-k");
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
