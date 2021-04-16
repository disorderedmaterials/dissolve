// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/calculateaxisangle.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "keywords/bool.h"
#include "keywords/enumoptions.h"
#include "procedure/nodes/select.h"

CalculateAxisAngleProcedureNode::CalculateAxisAngleProcedureNode(SelectProcedureNode *site0, OrientedSite::SiteAxis axis0,
                                                                 SelectProcedureNode *site1, OrientedSite::SiteAxis axis1)
    : CalculateProcedureNodeBase(ProcedureNode::NodeType::CalculateAxisAngle, site0, site1)
{
    // Create keywords - store the pointers to the superclasses for later use
    siteKeywords_[0] = new NodeKeyword<SelectProcedureNode>(this, ProcedureNode::NodeType::Select, true, site0);
    keywords_.add("Control", siteKeywords_[0], "I", "Site that contains the first set of axes");
    keywords_.add("Control", new EnumOptionsKeyword<OrientedSite::SiteAxis>(OrientedSite::siteAxis() = axis0), "AxisI",
                  "Axis to use from site I");
    siteKeywords_[1] = new NodeKeyword<SelectProcedureNode>(this, ProcedureNode::NodeType::Select, true, site1);
    keywords_.add("Control", siteKeywords_[1], "J", "Site that contains the second set of axes");
    keywords_.add("Control", new EnumOptionsKeyword<OrientedSite::SiteAxis>(OrientedSite::siteAxis() = axis1), "AxisJ",
                  "Axis to use from site J");
}

/*
 * Observable Target
 */

// Return number of sites required to calculate observable
int CalculateAxisAngleProcedureNode::nSitesRequired() const { return 2; }

// Return dimensionality of calculated observable
int CalculateAxisAngleProcedureNode::dimensionality() const { return 1; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool CalculateAxisAngleProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // Call the base class function
    if (!CalculateProcedureNodeBase::prepare(cfg, prefix, targetList))
        return false;

    // Get orientation flag
    axisI_ = keywords_.enumeration<OrientedSite::SiteAxis>("AxisI");
    axisJ_ = keywords_.enumeration<OrientedSite::SiteAxis>("AxisJ");

    return true;
}

// Execute node, targetting the supplied Configuration
bool CalculateAxisAngleProcedureNode::execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                              GenericList &targetList)
{
    assert(sites_[0] && sites_[0]->currentSite());
    assert(sites_[1] && sites_[1]->currentSite());

    value_ = Box::angleInDegrees(sites_[0]->currentSite()->axes().columnAsVec3(axisI_),
                                 sites_[1]->currentSite()->axes().columnAsVec3(axisJ_));

    return true;
}
