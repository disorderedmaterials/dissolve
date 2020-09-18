/*
    *** Procedure Node - Calculate Angle
    *** src/procedure/nodes/calculateangle.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "procedure/nodes/calculateangle.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "procedure/nodes/select.h"

CalculateAngleProcedureNode::CalculateAngleProcedureNode(SelectProcedureNode *site0, SelectProcedureNode *site1,
                                                         SelectProcedureNode *site2)
    : CalculateProcedureNodeBase(ProcedureNode::CalculateAngleNode, site0, site1, site2)
{
    // Create keywords - store the pointers to the superclasses for later use
    siteKeywords_[0] = new NodeKeyword<SelectProcedureNode>(this, ProcedureNode::SelectNode, true, site0);
    keywords_.add("Sites", siteKeywords_[0], "I", "Site that represents 'i' in the angle i-j-k");
    siteKeywords_[1] = new NodeKeyword<SelectProcedureNode>(this, ProcedureNode::SelectNode, true, site1);
    keywords_.add("Sites", siteKeywords_[1], "J", "Site that represents 'j' in the angle i-j-k");
    siteKeywords_[2] = new NodeKeyword<SelectProcedureNode>(this, ProcedureNode::SelectNode, true, site2);
    keywords_.add("Sites", siteKeywords_[2], "K", "Site that represents 'k' in the angle i-j-k");
}

CalculateAngleProcedureNode::~CalculateAngleProcedureNode() {}

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
ProcedureNode::NodeExecutionResult CalculateAngleProcedureNode::execute(ProcessPool &procPool, Configuration *cfg,
                                                                        std::string_view prefix, GenericList &targetList)
{
#ifdef CHECKS
    for (int n = 0; n < nSitesRequired(); ++n)
    {
        if (sites_[n]->currentSite() == nullptr)
        {
            Messenger::error("Observable {} has no current site.\n", n);
            return ProcedureNode::Failure;
        }
    }
#endif
    // Determine the value of the observable
    value_.x = cfg->box()->angleInDegrees(sites_[0]->currentSite()->origin(), sites_[1]->currentSite()->origin(),
                                          sites_[2]->currentSite()->origin());

    return ProcedureNode::Success;
}
