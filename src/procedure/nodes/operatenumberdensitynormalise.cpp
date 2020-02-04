/*
	*** Procedure Node - Operate Number Density Normalise
	*** src/procedure/nodes/operatenumberdensitynormalise.cpp
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

#include "procedure/nodes/operatenumberdensitynormalise.h"
#include "procedure/nodes/select.h"
#include "keywords/types.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
OperateNumberDensityNormaliseProcedureNode::OperateNumberDensityNormaliseProcedureNode() : OperateProcedureNodeBase(ProcedureNode::OperateNumberDensityNormaliseNode)
{
	// Create keywords - store the pointers to the superclasses for later use
	keywords_.add("Sites", new NodeRefListKeyword<const SelectProcedureNode>(this, ProcedureNode::SelectNode, false, selectNodes_), "Site", "Site(s) by which to normalise data based on their population");
}

OperateNumberDensityNormaliseProcedureNode::OperateNumberDensityNormaliseProcedureNode(RefList<const SelectProcedureNode> selectNodes) : OperateProcedureNodeBase(ProcedureNode::OperateNumberDensityNormaliseNode)
{
	// Create keywords - store the pointers to the superclasses for later use
	keywords_.add("Sites", new NodeRefListKeyword<const SelectProcedureNode>(this, ProcedureNode::SelectNode, false, selectNodes_), "Site", "Site(s) by which to normalise data based on their population");

	selectNodes_ = selectNodes;
}

// Destructor
OperateNumberDensityNormaliseProcedureNode::~OperateNumberDensityNormaliseProcedureNode()
{
}

/*
 * Data Target (implements virtuals in OperateProcedureNodeBase)
 */

// Operate on Data1D target
bool OperateNumberDensityNormaliseProcedureNode::operateData1D(ProcessPool& procPool, Configuration* cfg)
{
	for (const SelectProcedureNode* selectNode : selectNodes_) (*targetData1D_) /= (selectNode->nAverageSites() / cfg->box()->volume());

	return true;
}

// Operate on Data2D target
bool OperateNumberDensityNormaliseProcedureNode::operateData2D(ProcessPool& procPool, Configuration* cfg)
{
	for (const SelectProcedureNode* selectNode : selectNodes_) (*targetData2D_) /= (selectNode->nAverageSites() / cfg->box()->volume());

	return true;
}

// Operate on Data3D target
bool OperateNumberDensityNormaliseProcedureNode::operateData3D(ProcessPool& procPool, Configuration* cfg)
{
	for (const SelectProcedureNode* selectNode : selectNodes_) (*targetData3D_) /= (selectNode->nAverageSites() / cfg->box()->volume());

	return true;
}
