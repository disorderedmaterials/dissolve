/*
	*** NETA Logic Node
	*** src/neta/logic.h
	Copyright T. Youngs 2019-2020

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

#ifndef DISSOLVE_NETA_LOGIC_H
#define DISSOLVE_NETA_LOGIC_H

#include "neta/node.h"

// Forward Declarations
class NETADefinition;

// NETA Logic Node
class NETALogicNode : public NETANode
{
	public:
	// Node logic types
	enum LogicType { AndLogic, OrLogic, AndNotLogic };
	// Constructor / Destructor
	NETALogicNode(NETADefinition* parent, LogicType logic, NETANode* arg1, NETANode* arg2);
	~NETALogicNode();
	
	private:
	// Logic type
	LogicType logic_;
	// Node targets
	NETANode* argument1_, *argument2_;


	/*
	 * Scoring
	 */
	public:
	// Evaluate the node and return its score
	int score(const SpeciesAtom* i, RefList<const SpeciesAtom>& matchPath) const;
};

#endif
