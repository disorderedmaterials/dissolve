/*
	*** Procedure Node - Operate Divide
	*** src/procedure/nodes/operatedivide.cpp
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

#include "procedure/nodes/operatedivide.h"
#include "keywords/types.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
OperateDivideProcedureNode::OperateDivideProcedureNode() : OperateProcedureNodeBase(ProcedureNode::OperateDivideNode)
{
	keywords_.add("Divisor", new NodeValueKeyword(this, 1.0), "Value", "Value expression to divide by");
}

// Destructor
OperateDivideProcedureNode::~OperateDivideProcedureNode()
{
}

/*
 * Data Target (implements virtuals in OperateProcedureNodeBase)
 */

// Operate on Data1D target
bool OperateDivideProcedureNode::operateData1D(ProcessPool& procPool, Configuration* cfg)
{
	// Evaluate the expression to get the divisor
	const double divisor = keywords_.asDouble("Value");
	(*targetData1D_) /= divisor;

	return true;
}

// Operate on Data2D target
bool OperateDivideProcedureNode::operateData2D(ProcessPool& procPool, Configuration* cfg)
{
	// Evaluate the expression to get the divisor
	const double divisor = keywords_.asDouble("Value");
	(*targetData2D_) /= divisor;

	return true;
}

// Operate on Data3D target
bool OperateDivideProcedureNode::operateData3D(ProcessPool& procPool, Configuration* cfg)
{
	// Evaluate the expression to get the divisor
	const double divisor = keywords_.asDouble("Value");
	(*targetData3D_) /= divisor;

	return true;
}
