/*
    *** Procedure Node - Calculate Base Node
    *** src/procedure/nodes/calculatebase.cpp
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

#include "procedure/nodes/calculatebase.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

CalculateProcedureNodeBase::CalculateProcedureNodeBase(ProcedureNode::NodeType nodeType, SelectProcedureNode *site0,
                                                       SelectProcedureNode *site1, SelectProcedureNode *site2,
                                                       SelectProcedureNode *site3)
    : ProcedureNode(nodeType)
{
    sites_[0] = site0;
    sites_[1] = site1;
    sites_[2] = site2;
    sites_[3] = site3;
    siteKeywords_[0] = nullptr;
    siteKeywords_[1] = nullptr;
    siteKeywords_[2] = nullptr;
    siteKeywords_[3] = nullptr;
    value_ = 0.0;
}

CalculateProcedureNodeBase::~CalculateProcedureNodeBase() {}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool CalculateProcedureNodeBase::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::AnalysisContext);
}

/*
 * Observable Target
 */

// Return last calculated value of observable
double CalculateProcedureNodeBase::value(int id) const
{
#ifdef CHECKS
    if ((id < 0) || (id >= dimensionality()))
    {
        Messenger::error("Observable value index {} is out of range for this observable which has a dimensionality of {}.\n",
                         id, dimensionality());
        return 0.0;
    }
#endif
    return value_.get(id);
}

// Return last calculated value of observable
Vec3<double> CalculateProcedureNodeBase::values() const { return value_; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool CalculateProcedureNodeBase::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // Check that the sites have been properly defined
    for (int n = 0; n < nSitesRequired(); ++n)
    {
        sites_[n] = siteKeywords_[n] ? siteKeywords_[n]->node() : nullptr;
        if (!sites_[n])
            return Messenger::error("Observable site {} is not set.\n", n);
    }

    return true;
}
