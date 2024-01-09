// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/calculateAngle.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "keywords/bool.h"
#include "procedure/nodes/select.h"

CalculateAngleProcedureNode::CalculateAngleProcedureNode(std::shared_ptr<SelectProcedureNode> site0,
                                                         std::shared_ptr<SelectProcedureNode> site1,
                                                         std::shared_ptr<SelectProcedureNode> site2)
    : CalculateProcedureNodeBase(ProcedureNode::NodeType::CalculateAngle, site0, site1, site2)
{
    keywords_.setOrganisation("Options", "Sites");
    keywords_.add<NodeKeyword<SelectProcedureNode>>("I", "Site that represents 'i' in the angle i-j-k", sites_[0], this,
                                                    ProcedureNode::NodeType::Select, true);
    keywords_.add<NodeKeyword<SelectProcedureNode>>("J", "Site that represents 'j' in the angle i-j-k", sites_[1], this,
                                                    ProcedureNode::NodeType::Select, true);
    keywords_.add<NodeKeyword<SelectProcedureNode>>("K", "Site that represents 'k' in the angle i-j-k", sites_[2], this,
                                                    ProcedureNode::NodeType::Select, true);

    keywords_.setOrganisation("Options", "Control");
    keywords_.add<BoolKeyword>(
        "Symmetric", "Whether to consider angles as symmetric about 90, mapping all angles to the range 0 - 90", symmetric_);
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

// Execute node
bool CalculateAngleProcedureNode::execute(const ProcedureContext &procedureContext)
{
    assert(sites_[0] && sites_[0]->currentSite());
    assert(sites_[1] && sites_[1]->currentSite());
    assert(sites_[2] && sites_[2]->currentSite());

    // Determine the value of the observable
    value_.x = procedureContext.configuration()->box()->angleInDegrees(sites_[0]->currentSite()->get().origin(),
                                                                       sites_[1]->currentSite()->get().origin(),
                                                                       sites_[2]->currentSite()->get().origin());

    if (symmetric_ && value_.x > 90.0)
        value_.x = 180.0 - value_.x;

    return true;
}
