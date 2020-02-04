/*
	*** Procedure Node - Operate Normalise
	*** src/procedure/nodes/operatenormalise.cpp
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

#include "procedure/nodes/operatenormalise.h"
#include "keywords/types.h"
#include "math/data1d.h"
#include "math/integrator.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructors
OperateNormaliseProcedureNode::OperateNormaliseProcedureNode(double value) : OperateProcedureNodeBase(ProcedureNode::OperateNormaliseNode)
{
	keywords_.add("Normal", new NodeValueKeyword(this, value), "Value", "Value expression to normalise to");
	keywords_.add("Normal", new BoolKeyword(true), "Absolute", "Normalise absolute sum of values rather than direct sum");
}
OperateNormaliseProcedureNode::OperateNormaliseProcedureNode(int value) : OperateProcedureNodeBase(ProcedureNode::OperateNormaliseNode)
{
	keywords_.add("Normal", new NodeValueKeyword(this, value), "Value", "Value expression to normalise to");
	keywords_.add("Normal", new BoolKeyword(true), "Absolute", "Normalise absolute sum of values rather than direct sum");
}

// Destructor
OperateNormaliseProcedureNode::~OperateNormaliseProcedureNode()
{
}

/*
 * Data Target (implements virtuals in OperateProcedureNodeBase)
 */

// Operate on Data1D target
bool OperateNormaliseProcedureNode::operateData1D(ProcessPool& procPool, Configuration* cfg)
{
	// Evaluate the expression to get the value
	const double value = keywords_.asDouble("Value");
	const bool absolute = keywords_.asBool("Absolute");

	// Get sum of absolute values
	double sum = absolute ? Integrator::absSum(*targetData1D_) : Integrator::sum(*targetData1D_);
	(*targetData1D_) /= sum;
	(*targetData1D_) *= value;

	return true;
}

// Operate on Data2D target
bool OperateNormaliseProcedureNode::operateData2D(ProcessPool& procPool, Configuration* cfg)
{
	// Evaluate the expression to get the value
	const double value = keywords_.asDouble("Value");
	const bool absolute = keywords_.asBool("Absolute");

	// Get sum of absolute values
	double sum = absolute ? Integrator::absSum(*targetData2D_) : Integrator::sum(*targetData2D_);
	(*targetData2D_) /= sum;
	(*targetData2D_) *= value;

	return true;
}

// Operate on Data3D target
bool OperateNormaliseProcedureNode::operateData3D(ProcessPool& procPool, Configuration* cfg)
{
	// Evaluate the expression to get the value
	const double value = keywords_.asDouble("Value");
	const bool absolute = keywords_.asBool("Absolute");

	// Get sum of absolute values
	double sum = absolute ? Integrator::absSum(*targetData3D_) : Integrator::sum(*targetData3D_);
	(*targetData3D_) /= sum;
	(*targetData3D_) *= value;

	return true;
}
