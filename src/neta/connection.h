/*
	*** NETA Connection Node
	*** src/neta/connectivity.h
	Copyright T. Youngs 2019

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

#ifndef DISSOLVE_NETA_CONNECTION_H
#define DISSOLVE_NETA_CONNECTION_H

#include "neta/node.h"
#include "classes/speciesbond.h"
#include "templates/pointerarray.h"

// Forward Declarations
class Element;
class ForcefieldAtomType;
class NETADefinition;

// NETA Connection Node
class NETAConnectionNode : public NETANode
{
	public:
	// Constructor / Destructor
	NETAConnectionNode(NETADefinition* parent, PointerArray<Element> targetElements, PointerArray<ForcefieldAtomType> targetAtomTypes, SpeciesBond::BondType bt = SpeciesBond::nBondTypes);
	~NETAConnectionNode();

	private:
	// Array of elements that the current context atom may be
	PointerArray<Element> allowedElements_;
	// Array of ForcefieldAtomTypes that the current context atom may be
	PointerArray<ForcefieldAtomType> allowedAtomTypes_;
	// Type of required connection
	SpeciesBond::BondType bondType_;

	public:
	// Set node data
	void set(Array<int> elementsAndTypes, NETANode* innerneta, SpeciesBond::BondType bondtype);


	/*
	 * Modifiers
	 */
	private:
	// Repeat count value
	int repeatCount_;
	// Repeat count comparison operator
	NETANode::ComparisonOperator repeatCountOperator_;

	public:
	// Available modifiers
	enum NETAConnectionModifier
	{
		RepeatConnectionModifier,	/* 'n' - specifies the number of matches required (default = 1) */
		nConnectionModifiers
	};
	// Return enum options for NETAConnectionModifiers
	static EnumOptions<NETAConnectionNode::NETAConnectionModifier> modifiers();
	// Return whether the specified modifier is valid for this node
	bool isValidModifier(const char* s);
	// Set value and comparator for specified modifier
	bool setModifier(const char* modifier, ComparisonOperator op, int value);


	/*
	 * Scoring
	 */
	public:
	// Evaluate the node and return its score
	int score(const SpeciesAtom* i, RefList<const SpeciesAtom>& matchPath) const;
};

#endif
