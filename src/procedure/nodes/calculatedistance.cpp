/*
    *** Procedure Node - Calculate Distance
    *** src/procedure/nodes/calculatedistance.cpp
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

#include "procedure/nodes/calculatedistance.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "procedure/nodes/select.h"

CalculateDistanceProcedureNode::CalculateDistanceProcedureNode(SelectProcedureNode *site0, SelectProcedureNode *site1)
    : CalculateProcedureNodeBase(ProcedureNode::CalculateDistanceNode, site0, site1)
{
    // Create keywords - store the pointers to the superclasses for later use
    siteKeywords_[0] = new NodeKeyword<SelectProcedureNode>(this, ProcedureNode::SelectNode, true, site0);
    keywords_.add("Sites", siteKeywords_[0], "I", "Site that represents 'i' in the distance i-j");
    siteKeywords_[1] = new NodeKeyword<SelectProcedureNode>(this, ProcedureNode::SelectNode, true, site1);
    keywords_.add("Sites", siteKeywords_[1], "J", "Site that represents 'j' in the distance i-j");
}

CalculateDistanceProcedureNode::~CalculateDistanceProcedureNode() {}

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

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult CalculateDistanceProcedureNode::execute(ProcessPool &procPool, Configuration *cfg,
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
    value_.x = cfg->box()->minimumDistance(sites_[0]->currentSite()->origin(), sites_[1]->currentSite()->origin());

    return ProcedureNode::Success;
}
