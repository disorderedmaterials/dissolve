 /*
	*** NETA Node
	*** src/neta/node.h
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

#ifndef DISSOLVE_NETA_NODE_H
#define DISSOLVE_NETA_NODE_H

#include <vector>
#include "base/enumoptions.h"
#include "templates/listitem.h"
#include "templates/list.h"
#include "templates/reflist.h"

// Forward Declarations
class Element;
class ForcefieldAtomType;
class NETADefinition;
class NETAPresenceNode;
class NETAConnectionNode;
class NETARingNode;
class SpeciesAtom;

// NETA Node
class NETANode : public ListItem<NETANode>
{
	public:
	// Node types
	enum NodeType { BasicNode, ConnectionNode, LogicNode, PresenceNode, RingNode, RootNode, nNETANodeTypes };
	// Value Comparison Operators
	enum ComparisonOperator { EqualTo, NotEqualTo, GreaterThan, LessThan, GreaterThanEqualTo, LessThanEqualTo };
	// Return enum options for Comparison Operators
	static EnumOptions<NETANode::ComparisonOperator> comparisonOperators();
	// Special Scoring Results
	enum NETAResult { NoMatch = -1 };
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
	 * Branching and Node Generation
	 */
	protected:
	// Branch of nodes
	List<NETANode> branch_;

	public:
	// Clear all nodes
	void clear();
	// Return last node of branch
	NETANode* lastBranchNode();
	// Return number of nodes defined in branch
	int nBranchNodes() const;
	// Create connectivity node in the branch
	NETAConnectionNode* createConnectionNode(std::vector<Element*> targetElements, std::vector<ForcefieldAtomType*> targetAtomTypes);
	// Create presence node in the branch
	NETAPresenceNode* createPresenceNode(std::vector<Element*> targetElements, std::vector<ForcefieldAtomType*> targetAtomTypes);
	// Create ring node in the branch
	NETARingNode* createRingNode();


	/*
	 * Modifiers
	 */
	public:
	// Return whether the specified modifier is valid for this node
	virtual bool isValidModifier(const char* s) const;
	// Set value and comparator for specified modifier
	virtual bool setModifier(const char* modifier, ComparisonOperator op, int value);


	/*
	 * Flags
	 */
	public:
	// Return whether the specified flag is valid for this node
	virtual bool isValidFlag(const char* s) const;
	// Set specified flag
	virtual bool setFlag(const char* flag, bool state);


	/*
	 * Value Comparison
	 */
	public:
	// Return result of comparison between values provided
	static bool compareValues(int lhsValue, ComparisonOperator op, int rhsValue);


	/*
	 * Scoring
	 */
	protected:
	// Whether to use reverse logic when returning the final value
	bool reverseLogic_;

	public:
	// Set node to use reverse logic
	void setReverseLogic();
	// Evaluate the node and return its score
	virtual int score(const SpeciesAtom* i, RefList<const SpeciesAtom>& atomData) const;
};

#endif
