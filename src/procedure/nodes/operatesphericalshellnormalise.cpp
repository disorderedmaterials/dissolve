/*
	*** Procedure Node - Operate Spherical Shell Normalise
	*** src/procedure/nodes/operatesphericalshellnormalise.cpp
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

#include "procedure/nodes/operatesphericalshellnormalise.h"
#include "procedure/nodes/select.h"
#include "keywords/types.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
OperateSphericalShellNormaliseProcedureNode::OperateSphericalShellNormaliseProcedureNode() : OperateProcedureNodeBase(ProcedureNode::OperateSphericalShellNormaliseNode)
{
}

// Destructor
OperateSphericalShellNormaliseProcedureNode::~OperateSphericalShellNormaliseProcedureNode()
{
}

/*
 * Data Target (implements virtuals in OperateProcedureNodeBase)
 */

// Operate on Data1D target
bool OperateSphericalShellNormaliseProcedureNode::operateData1D(ProcessPool& procPool, Configuration* cfg)
{
	// We expect x values to be centre-bin values, and regularly spaced
	const Array<double>& xAxis = targetData1D_->constXAxis();
	Array<double>& values = targetData1D_->values();
	if (xAxis.nItems() < 2) return true;

	// Derive first left-bin boundary from the delta betwen points 0 and 1
	double leftBin = xAxis.constAt(0) - (xAxis.constAt(1) - xAxis.constAt(0)) * 0.5, rightBin, divisor;
	double r1Cubed = pow(leftBin, 3), r2Cubed;
	for (int n = 0; n < xAxis.nItems(); ++n)
	{
		// Get new right-bin from existing left bin boundary and current bin centre
		rightBin = leftBin + 2*(xAxis.constAt(n) - leftBin);
		r2Cubed = pow(rightBin, 3);
		divisor = (4.0/3.0) * PI * (r2Cubed - r1Cubed);
		values[n] /= divisor;
		if (targetData1D_->valuesHaveErrors()) targetData1D_->error(n) /= divisor;

		// Overwrite old values
		r1Cubed = r2Cubed;
		leftBin = rightBin;
	}

	return true;
}
