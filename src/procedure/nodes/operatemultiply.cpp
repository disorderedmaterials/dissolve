/*
	*** Procedure Node - Operate Multiply
	*** src/procedure/nodes/operatemultiply.cpp
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

#include "procedure/nodes/operatemultiply.h"
#include "math/data1d.h"
#include "keywords/types.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
OperateMultiplyProcedureNode::OperateMultiplyProcedureNode() : OperateProcedureNodeBase(ProcedureNode::OperateMultiplyNode)
{
	keywords_.add("Multiplier", new NodeValueKeyword(this, 1.0), "Value", "Value expression to multiply by");
}

// Destructor
OperateMultiplyProcedureNode::~OperateMultiplyProcedureNode()
{
}

/*
 * Data Target (implements virtuals in OperateProcedureNodeBase)
 */

// Operate on Data1D target
bool OperateMultiplyProcedureNode::operateData1D(ProcessPool& procPool, Configuration* cfg)
{
	// Evaluate the expression to get the multiplier
	const double multiplier = keywords_.asDouble("Value");
	(*targetData1D_) *= multiplier;

	return true;
} 

// Operate on Data2D target
bool OperateMultiplyProcedureNode::operateData2D(ProcessPool& procPool, Configuration* cfg)
{
	// Evaluate the expression to get the multiplier
	const double multiplier = keywords_.asDouble("Value");
	(*targetData2D_) *= multiplier;

	return true;
}

// Operate on Data3D target
bool OperateMultiplyProcedureNode::operateData3D(ProcessPool& procPool, Configuration* cfg)
{
	// Evaluate the expression to get the multiplier
	const double multiplier = keywords_.asDouble("Value");
	(*targetData3D_) *= multiplier;

	return true;
}
