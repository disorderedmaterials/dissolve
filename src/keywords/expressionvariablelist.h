/*
	*** Keyword - ExpressionVariable List
	*** src/keywords/expressionvariablelist.h
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

#ifndef DISSOLVE_KEYWORD_EXPRESSIONVARIABLELIST_H
#define DISSOLVE_KEYWORD_EXPRESSIONVARIABLELIST_H

#include "keywords/data.h"
#include "keywords/base.h"
#include "expression/node.h"
#include "templates/list.h"

// Forward Declarations
class ProcedureNode;

// Keyword with ExpressionVariable List
class ExpressionVariableListKeyword : public KeywordData< List<ExpressionNode>& >
{
	public:
	// Constructor
	ExpressionVariableListKeyword(ProcedureNode* parentNode, List<ExpressionNode>& variables, ExpressionValue::ValueType variableType);
	// Destructor
	~ExpressionVariableListKeyword();


	/*
	 * Parent Node
	 */
	private:
	// Parent ProcedureNode
	ProcedureNode* parentNode_;

	public:
	// Parent ProcedureNode
	const ProcedureNode* parentNode() const;


	/*
	 * Variable Type
	 */
	private:
	// Assumed type for variables in the list
	ExpressionValue::ValueType variableType_;

	public:
	// Return assumed type for variables in the list
	ExpressionValue::ValueType variableType() const;


	/*
	 * Data
	 */
	protected:
	// Determine whether current data is actually 'set'
	bool currentDataIsSet() const;


	/*
	 * Arguments
	 */
	public:
	// Return minimum number of arguments accepted
	int minArguments();
	// Return maximum number of arguments accepted
	int maxArguments();
	// Parse arguments from supplied LineParser, starting at given argument offset
	bool read(LineParser& parser, int startArg, const CoreData& coreData);
	// Write keyword data to specified LineParser
	bool write(LineParser& parser, const char* prefix);


	/*
	 * Object Management
	 */
	protected:
};

#endif

