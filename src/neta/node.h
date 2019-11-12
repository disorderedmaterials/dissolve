 /*
	*** NETA Node
	*** src/neta/node.h
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

#ifndef DISSOLVE_NETA_NODE_H
#define DISSOLVE_NETA_NODE_H

#include "templates/listitem.h"
#include "templates/reflist.h"

// Forward Declarations
class NETADefinition;
class SpeciesAtom;

// NETA Node
class NETANode : public ListItem<NETANode>
{
	public:
	// Node types
	enum NodeType { ConnectionNode, LogicNode, nNETANodeTypes };
	// Constructor / Destructor
	NETANode(NETADefinition* parent, NodeType type);
	virtual ~NETANode();


	/*
	 * Node Type and Parent
	 */
	protected:
	// Type of node
	NodeType nodeType_;
	// Pointer to parent definition
	NETADefinition* parent_;

	public:
	// Return node type
	NodeType nodeType() const;
	// Return parent definition
	NETADefinition* parent() const;


	/*
	 * Scoring
	 */
	protected:
	// Whether to use reverse logic when returning the final value
	bool reverseLogic_;

	public:
	// Special Results
	enum NETAResult { NoMatch = -1 };
	// Set node to use reverse logic
	void setReverseLogic();
	// Evaluate the node and return its score
	virtual int score(const SpeciesAtom* i, RefList<const SpeciesAtom>& matchPath) const = 0;


// 
// 	public:
// 	// Return node type
// 	NETANodeType nodeType() const;
// 	// Return parent NETA structure
// 	NETADefinition* parent() const;
// 	// Set parent NETA definition
// 	void setParent(NETADefinition* neta);
// 	// Validation function
// 	virtual int score(SpeciesAtom* target, RefList<SpeciesAtom>* nbrs, RefList<Ring>* rings, NETAContextNode* context, RefList<SpeciesAtom>& path, int level) = 0;
// 	// Print node contents
// 	virtual void nodePrint(int offset, const char* prefix) = 0;
// 	// Print (append) NETA representation of node contents
// 	virtual void netaPrint(CharString& neta) = 0;
// 	// Print contextual score
// 	static void printScore(int level, const char* fmt, ...);
// 
// 
// 	/*
// 	 * Value Comparison
// 	 */
// 	public:
// 	// Value Comparison Operators
// 	enum ValueComparison { EqualTo, NotEqualTo, GreaterThan, LessThan, GreaterThanEqualTo, LessThanEqualTo, nNETAValueComparisons };
// 	static const char* netaValueComparison(ValueComparison op);
// 
// 	public:
// 	// Return result of comparison between values provided
// 	static bool valueComparison(int lhsValue, ValueComparison op, int rhsValue);
};

#endif
