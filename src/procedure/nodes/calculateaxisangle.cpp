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

CalculateAxisAngleProcedureNode::CalculateAxisAngleProcedureNode(std::shared_ptr<SelectProcedureNode> site0, OrientedSite::SiteAxis axis0,
                                                                 std::shared_ptr<SelectProcedureNode> site1, OrientedSite::SiteAxis axis1)
    : CalculateProcedureNodeBase(ProcedureNode::NodeType::CalculateAxisAngle, site0, site1), axes_{axis0, axis1}
{
    keywords_.add<NodeKeyword<SelectProcedureNode>>("Control", "I", "Site that contains the first set of axes", sites_[0], this,
                                                    ProcedureNode::NodeType::Select, true);
    keywords_.add<NodeKeyword<SelectProcedureNode>>("Control", "J", "Site that contains the second set of axes", sites_[1],
                                                    this, ProcedureNode::NodeType::Select, true);
    keywords_.add<EnumOptionsKeyword<OrientedSite::SiteAxis>>("Control", "AxisI", "Axis to use from site I", axes_[0],
                                                              OrientedSite::siteAxis());
    keywords_.add<EnumOptionsKeyword<OrientedSite::SiteAxis>>("Control", "AxisJ", "Axis to use from site J", axes_[1],
                                                              OrientedSite::siteAxis());
}

/*
 * Data
 */

// Return axis specified
OrientedSite::SiteAxis &CalculateAxisAngleProcedureNode::axis(int n)
{
    assert(n >= 0 && n < 2);
    return axes_[n];
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

    return true;
}

// Execute node, targetting the supplied Configuration
bool CalculateAxisAngleProcedureNode::execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                              GenericList &targetList)
{
    assert(sites_[0] && sites_[0]->currentSite());
    assert(sites_[1] && sites_[1]->currentSite());

    value_ = Box::angleInDegrees(sites_[0]->currentSite()->axes().columnAsVec3(axes_[0]),
                                 sites_[1]->currentSite()->axes().columnAsVec3(axes_[1]));

    return true;
}
