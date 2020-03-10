/*
	*** NETA Ring Node
	*** src/neta/ring.h
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

#ifndef DISSOLVE_NETA_RING_H
#define DISSOLVE_NETA_RING_H

#include <vector>
#include "neta/node.h"
#include "classes/speciesbond.h"
#include "classes/speciesring.h"

// Forward Declarations
class Element;
class ForcefieldAtomType;
class NETADefinition;

// NETA Ring Node
class NETARingNode : public NETANode
{
	public:
	// Constructor / Destructor
	NETARingNode(NETADefinition* parent);
	~NETARingNode();


	/*
	 * Modifiers
	 */
	private:
	// Repeat count value
	int repeatCount_;
	// Repeat count comparison operator
	NETANode::ComparisonOperator repeatCountOperator_;
	// Ring size value
	int sizeValue_;
	// Ring size value comparison operator
	NETANode::ComparisonOperator sizeValueOperator_;

	public:
	// Available modifiers
	enum NETARingModifier
	{
		SizeModifier,			/* 'size' - Specifies size of ring (default = -1) */
		RepeatRingModifier,		/* 'n' - Specifies the number of matches required (default = 1) */
		nRingModifiers
	};
	// Return enum options for NETARingModifiers
	static EnumOptions<NETARingNode::NETARingModifier> modifiers();
	// Return whether the specified modifier is valid for this node
	bool isValidModifier(const char* s) const;
	// Set value and comparator for specified modifier
	bool setModifier(const char* modifier, ComparisonOperator op, int value);


	/*
	 * Scoring
	 */
	private:
	// Locate rings in which the specified atom is involved
	void findRings(const SpeciesAtom* currentAtom, List<SpeciesRing>& rings, std::vector<const SpeciesAtom*>& path, const int minSize, const int maxSize) const;

	public:
	// Evaluate the node and return its score
	int score(const SpeciesAtom* i, RefList<const SpeciesAtom>& matchPath) const;
};

#endif
