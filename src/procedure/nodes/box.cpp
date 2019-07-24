/*
	*** Procedure Node - Box
	*** src/procedure/nodes/box.cpp
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

#include "procedure/nodes/box.h"
#include "procedure/nodescopestack.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
BoxProcedureNode::BoxProcedureNode(Vec3<double> lengths, Vec3<double> angles, bool nonPeriodic) : ProcedureNode(ProcedureNode::BoxNode)
{
	lengthA_ = lengths.x;
	lengthB_ = lengths.y;
	lengthC_ = lengths.z;
	angleAlpha_ = angles.x;
	angleBeta_ = angles.y;
	angleGamma_ = angles.z;
	nonPeriodic_ = nonPeriodic;
}

// Destructor
BoxProcedureNode::~BoxProcedureNode()
{
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool BoxProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
	return (context == ProcedureNode::GenerationContext);
}

/*
 * Node Keywords
 */

// Return enum option info for BoxNodeKeyword
EnumOptions<BoxProcedureNode::BoxNodeKeyword> BoxProcedureNode::boxNodeKeywords()
{
	static EnumOptionsList BoxNodeTypeKeywords = EnumOptionsList() <<
		EnumOption(BoxProcedureNode::AnglesKeyword,		"Angles",	3) <<
		EnumOption(BoxProcedureNode::EndBoxKeyword,		"EndBox") <<
		EnumOption(BoxProcedureNode::LengthsKeyword,		"Lengths",	3) <<
		EnumOption(BoxProcedureNode::NonPeriodicKeyword,	"NonPeriodic",	1);

	static EnumOptions<BoxProcedureNode::BoxNodeKeyword> options("BoxNodeKeyword", BoxNodeTypeKeywords);

	return options;
}

/* 
 * Execute
 */

// Prepare any necessary data, ready for execution
bool BoxProcedureNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
{
	return true;
}

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult BoxProcedureNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// Create a Box in the target Configuration with our lengths and angles
	Vec3<double> lengths(lengthA_.asDouble(), lengthB_.asDouble(), lengthC_.asDouble());
	Vec3<double> angles(angleAlpha_.asDouble(), angleBeta_.asDouble(), angleGamma_.asDouble());
	if (!cfg->createBox(lengths, angles, nonPeriodic_)) return ProcedureNode::Failure;

	return ProcedureNode::Success;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool BoxProcedureNode::read(LineParser& parser, const CoreData& coreData, NodeScopeStack& scopeStack)
{
	// Read until we encounter the EndBox keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Do we recognise this argument and, if so, do we have the appropriate number of arguments?
		if (!boxNodeKeywords().isValid(parser.argc(0))) return boxNodeKeywords().errorAndPrintValid(parser.argc(0));
		BoxNodeKeyword nk = boxNodeKeywords().enumeration(parser.argc(0));
		if (!boxNodeKeywords().validNArgs(nk, parser.nArgs()-1)) return false;

		// All OK, so process it
		switch (nk)
		{
			case (BoxProcedureNode::AnglesKeyword):
				if (!angleAlpha_.set(parser.argc(1), scopeStack.parameterReferences())) return Messenger::error("Failed to parse expression '%s' for angle 'alpha'.\n", parser.argc(1));
				if (!angleBeta_.set(parser.argc(2), scopeStack.parameterReferences())) return Messenger::error("Failed to parse expression '%s' for angle 'beta'.\n", parser.argc(2));
				if (!angleGamma_.set(parser.argc(3), scopeStack.parameterReferences())) return Messenger::error("Failed to parse expression '%s' for angle 'gamma'.\n", parser.argc(3));				break;
			case (BoxProcedureNode::EndBoxKeyword):
				return true;
			case (BoxProcedureNode::LengthsKeyword):
				if (!lengthA_.set(parser.argc(1), scopeStack.parameterReferences())) return Messenger::error("Failed to parse expression '%s' for length 'A'.\n", parser.argc(1));
				if (!lengthB_.set(parser.argc(2), scopeStack.parameterReferences())) return Messenger::error("Failed to parse expression '%s' for length 'B'.\n", parser.argc(2));
				if (!lengthC_.set(parser.argc(3), scopeStack.parameterReferences())) return Messenger::error("Failed to parse expression '%s' for length 'C'.\n", parser.argc(3));
				break;
			case (BoxProcedureNode::NonPeriodicKeyword):
				nonPeriodic_ = parser.argb(1);
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the Box node keyword '%s'.\n", parser.argc(0));
		}
	}

	return true;
}

// Write structure to specified LineParser
bool BoxProcedureNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
