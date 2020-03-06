/*
	*** NETA Connection Node
	*** src/neta/connection.h
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

#ifndef DISSOLVE_NETA_CONNECTION_H
#define DISSOLVE_NETA_CONNECTION_H

#include <vector>
#include "neta/node.h"
#include "classes/speciesbond.h"

// Forward Declarations
class Element;
class ForcefieldAtomType;
class NETADefinition;

// NETA Connection Node
class NETAConnectionNode : public NETANode
{
	public:
	// Constructor / Destructor
	NETAConnectionNode(NETADefinition* parent, std::vector<Element*> targetElements, std::vector<ForcefieldAtomType*> targetAtomTypes, SpeciesBond::BondType bt = SpeciesBond::nBondTypes);
	~NETAConnectionNode();

	private:
	// Array of elements that the current context atom may be
	std::vector<Element*> allowedElements_;
	// Array of ForcefieldAtomTypes that the current context atom may be
	std::vector<ForcefieldAtomType*> allowedAtomTypes_;
	// Type of required connection
	SpeciesBond::BondType bondType_;


	/*
	 * Modifiers
	 */
	private:
	// Repeat count value
	int repeatCount_;
	// Repeat count comparison operator
	NETANode::ComparisonOperator repeatCountOperator_;
	// Number of bonds value
	int nBondsValue_;
	// Numbe of bonds value comparison operator
	NETANode::ComparisonOperator nBondsValueOperator_;
	// Number of hydrogens value
	int nHydrogensValue_;
	// Numbe of hydrogens value comparison operator
	NETANode::ComparisonOperator nHydrogensValueOperator_;

	public:
	// Available modifiers
	enum NETAConnectionModifier
	{
		NBondsModifier,			/* 'nbonds' - Specifies number of bonds (default = -1) */
		NHydrogensModifier,		/* 'nh' - Specifies number of hydrogens (default = -1) */
		RepeatConnectionModifier,	/* 'n' - Specifies the number of matches required (default = 1) */
		nConnectionModifiers
	};
	// Return enum options for NETAConnectionModifiers
	static EnumOptions<NETAConnectionNode::NETAConnectionModifier> modifiers();
	// Return whether the specified modifier is valid for this node
	bool isValidModifier(const char* s) const;
	// Set value and comparator for specified modifier
	bool setModifier(const char* modifier, ComparisonOperator op, int value);


	/*
	 * Flags
	 */
	private:
	// Whether to allow the root atom of the path to be re-matched within this node
	bool allowRootMatch_;

	public:
	// Available flags
	enum NETAConnectionFlag
	{
		RootFlag,			/* 'root' - Specifies that the root atom of the current path may be re-matched */
		nConnectionFlags
	};
	// Return enum options for NETAConnectionFlags
	static EnumOptions<NETAConnectionNode::NETAConnectionFlag> flags();
	// Return whether the specified flag is valid for this node
	bool isValidFlag(const char* s) const;
	// Set specified flag
	bool setFlag(const char* flag, bool state);


	/*
	 * Scoring
	 */
	public:
	// Evaluate the node and return its score
	int score(const SpeciesAtom* i, RefList<const SpeciesAtom>& matchPath) const;
};

#endif
