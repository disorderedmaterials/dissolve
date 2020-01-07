/*
	*** Procedure Nodes
	*** src/procedure/nodes/nodes.h
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

#ifndef DISSOLVE_PROCEDURENODE_NODETYPES_H
#define DISSOLVE_PROCEDURENODE_NODETYPES_H

/*
 * Include all procedure node types at once.
 */

#include "procedure/nodes/addspecies.h"
#include "procedure/nodes/box.h"
#include "procedure/nodes/calculateangle.h"
#include "procedure/nodes/calculatedistance.h"
#include "procedure/nodes/calculatevector.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/collect2d.h"
#include "procedure/nodes/collect3d.h"
#include "procedure/nodes/dynamicsite.h"
#include "procedure/nodes/exclude.h"
#include "procedure/nodes/fit1d.h"
#include "procedure/nodes/integrate1d.h"
#include "procedure/nodes/operatedivide.h"
#include "procedure/nodes/operateexpression.h"
#include "procedure/nodes/operategridnormalise.h"
#include "procedure/nodes/operatemultiply.h"
#include "procedure/nodes/operatenormalise.h"
#include "procedure/nodes/operatenumberdensitynormalise.h"
#include "procedure/nodes/operatesitepopulationnormalise.h"
#include "procedure/nodes/operatesphericalshellnormalise.h"
#include "procedure/nodes/parameters.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/process2d.h"
#include "procedure/nodes/process3d.h"
#include "procedure/nodes/select.h"
#include "procedure/nodes/sequence.h"
#include "procedure/nodes/sum1d.h"

#endif
