/*
	*** Procedure Node - Operate Multiply
	*** src/procedure/nodes/operatemultiply.h
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

#ifndef DISSOLVE_PROCEDURENODE_OPERATEMULTIPLY_H
#define DISSOLVE_PROCEDURENODE_OPERATEMULTIPLY_H

#include "procedure/nodes/operatebase.h"

// Forward Declarations
class SelectProcedureNode;

// Operate Number Density Normalise Node
class OperateMultiplyProcedureNode : public OperateProcedureNodeBase
{
	public:
	// Constructor
	OperateMultiplyProcedureNode();
	// Destructor
	~OperateMultiplyProcedureNode();


	/*
	 * Data Target (implements virtuals in OperateProcedureNodeBase)
	 */
	public:
	// Operate on Data1D target
	bool operateData1D(ProcessPool& procPool, Configuration* cfg);
	// Operate on Data2D target
	bool operateData2D(ProcessPool& procPool, Configuration* cfg);
	// Operate on Data3D target
	bool operateData3D(ProcessPool& procPool, Configuration* cfg);
};

#endif
