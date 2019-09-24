/*
	*** Procedure Node - Calculate Vector
	*** src/procedure/nodes/calculatevector.cpp
	Copyright T. Youngs 2012-2019

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

#include "procedure/nodes/calculatevector.h"
#include "procedure/nodes/select.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
CalculateVectorProcedureNode::CalculateVectorProcedureNode(SelectProcedureNode* site0, SelectProcedureNode* site1, SelectProcedureNode* site2, SelectProcedureNode* site3) : CalculateProcedureNodeBase(ProcedureNode::CalculateVectorNode, site0, site1, site2, site3)
{

}

// Destructor
CalculateVectorProcedureNode::~CalculateVectorProcedureNode()
{
}

/*
 * Observable Target
 */

// Return number of sites required to calculate observable
int CalculateVectorProcedureNode::nSitesRequired() const
{
	return 2;
}

// Return dimensionality of calculated observable
int CalculateVectorProcedureNode::dimensionality() const
{
	return 3;
}

/* 
 * Execute
 */

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult CalculateVectorProcedureNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
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
	// Determine the value of the observable
	value_ = cfg->box()->minimumVector(sites_[0]->currentSite()->origin(), sites_[1]->currentSite()->origin());

	return ProcedureNode::Success;
}
