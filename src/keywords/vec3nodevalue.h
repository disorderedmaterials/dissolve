/*
	*** Keyword - NodeValue Triplet
	*** src/keywords/vec3nodevalue.h
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

#ifndef DISSOLVE_KEYWORD_VEC3NODEVALUE_H
#define DISSOLVE_KEYWORD_VEC3NODEVALUE_H

#include "keywords/data.h"
#include "keywords/vec3labels.h"
#include "procedure/nodevalue.h"

// Forward Declarations
class ProcedureNode;

// Keyword with NodeValue Triplet Data
class Vec3NodeValueKeyword : public KeywordData< Vec3<NodeValue> >
{
	public:
	// Constructors
	Vec3NodeValueKeyword(ProcedureNode* parentNode, Vec3<double> value, Vec3Labels::LabelType labelType = Vec3Labels::NoLabels);
	// Destructor
	~Vec3NodeValueKeyword();


	/*
	 * Parent Node
	 */
	private:
	// Parent ProcedureNode
	ProcedureNode* parentNode_;


	/*
	 * Label Type
	 */
	private:
	// Label type to display in GUI
	Vec3Labels::LabelType labelType_;

	public:
	// Label type to display in GUI
	Vec3Labels::LabelType labelType() const;


	/*
	 * Arguments
	 */
	public:
	// Return minimum number of arguments accepted
	int minArguments() const;
	// Return maximum number of arguments accepted
	int maxArguments() const;
	// Parse arguments from supplied LineParser, starting at given argument offset
	bool read(LineParser& parser, int startArg, const CoreData& coreData);
	// Write keyword data to specified LineParser
	bool write(LineParser& parser, const char* prefix);


	/*
	 * Set
	 */
	public:
	// Set the specified value from supplied expression text
	bool setValue(int index, const char* expressionText);


	/*
	 * Conversion
	 */
	public:
	// Return value (as Vec3<int>)
	Vec3<int> asVec3Int();
	// Return value (as Vec3<double>)
	Vec3<double> asVec3Double();
};

#endif

