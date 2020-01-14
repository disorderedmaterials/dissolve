/*
	*** Procedure Node - Calculate AxisAngle
	*** src/procedure/nodes/calculatevector.cpp
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

#include "procedure/nodes/calculateaxisangle.h"
#include "procedure/nodes/select.h"
#include "keywords/bool.h"
#include "keywords/integer.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
CalculateAxisAngleProcedureNode::CalculateAxisAngleProcedureNode(SelectProcedureNode* site0, int axis0, SelectProcedureNode* site1, int axis1) : CalculateProcedureNodeBase(ProcedureNode::CalculateAxisAngleNode, site0, site1)
{
	// Create keywords - store the pointers to the superclasses for later use
	siteKeywords_[0] = new NodeKeyword<SelectProcedureNode>(this, ProcedureNode::SelectNode, true, site0);
	keywords_.add("Sites", siteKeywords_[0], "I", "Site that contains the first set of axes");
	keywords_.add("Sites", new IntegerKeyword(axis0, 1, 3), "AxisI", "Axis to use from site I");
	siteKeywords_[1] = new NodeKeyword<SelectProcedureNode>(this, ProcedureNode::SelectNode, true, site1);
	keywords_.add("Sites", siteKeywords_[1], "J", "Site that contains the second set of axes");
	keywords_.add("Sites", new IntegerKeyword(axis1, 1, 3), "AxisJ", "Axis to use from site J");
}

// Destructor
CalculateAxisAngleProcedureNode::~CalculateAxisAngleProcedureNode()
{
}

/*
 * Observable Target
 */

// Return number of sites required to calculate observable
int CalculateAxisAngleProcedureNode::nSitesRequired() const
{
	return 2;
}

// Return dimensionality of calculated observable
int CalculateAxisAngleProcedureNode::dimensionality() const
{
	return 1;
}

/* 
 * Execute
 */

// Prepare any necessary data, ready for execution
bool CalculateAxisAngleProcedureNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// Get orientation flag
	axisI_ = keywords_.asInt("AxisI") - 1;
	axisJ_ = keywords_.asInt("AxisJ") - 1;
}

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult CalculateAxisAngleProcedureNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
#ifdef CHECKS
	for (int n=0; n<nSitesRequired(); ++n)
	{
		if (sites_[n]->currentSite() == NULL)
		{
			Messenger::error("Observable %i has no current site.\n", n);
			return ProcedureNode::Failure;
		}
	}
#endif
	value_ = Box::angleInDegrees(sites_[0]->currentSite()->axes().columnAsVec3(axisI_), sites_[1]->currentSite()->axes().columnAsVec3(axisJ_));

	return ProcedureNode::Success;
}
