/*
	*** Keyword - DynamicSiteProcedureNode (Ref)List
	*** src/keywords/dynamicsitenodes.cpp
	Copyright T. Youngs 2012-2020

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

#include "keywords/dynamicsitenodes.h"
#include "classes/coredata.h"
#include "expression/variable.h"
#include "procedure/nodes/dynamicsite.h"
#include "base/lineparser.h"

// Constructor
DynamicSiteNodesKeyword::DynamicSiteNodesKeyword(SelectProcedureNode* parentNode, RefList<DynamicSiteProcedureNode>& nodes, bool axesRequired) : KeywordData< RefList<DynamicSiteProcedureNode>& >(KeywordBase::DynamicSiteNodesData, nodes)
{
	parentNode_ = parentNode;
	axesRequired_ = axesRequired;
}

// Destructor
DynamicSiteNodesKeyword::~DynamicSiteNodesKeyword()
{
}

/*
 * Parent Node
 */

// Return parent SelectProcedureNode
const SelectProcedureNode* DynamicSiteNodesKeyword::parentNode() const
{
	return parentNode_;
}

/*
 * Data
 */

// Determine whether current data is 'empty', and should be considered as 'not set'
bool DynamicSiteNodesKeyword::isDataEmpty() const
{
	return data_.nItems() > 0;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int DynamicSiteNodesKeyword::minArguments() const
{
	return 0;
}

// Return maximum number of arguments accepted
int DynamicSiteNodesKeyword::maxArguments() const
{
	return 0;
}

// Parse arguments from supplied LineParser, starting at given argument offset
bool DynamicSiteNodesKeyword::read(LineParser& parser, int startArg, const CoreData& coreData)
{
	if (!parentNode_) return Messenger::error("Parent ProcedureNode not set, so can't read DynamicSiteNode data.\n");

	// Create a new DynamicSite and add it to our data RefList
	DynamicSiteProcedureNode* dynamicSite = new DynamicSiteProcedureNode(parentNode_);
	data_.append(dynamicSite);

	// Attempt to read the DynamicSite data
	if (!dynamicSite->read(parser, coreData)) return false;

	// Check for required axes?
	if (axesRequired_ && (!dynamicSite->hasAxes())) return Messenger::error("Dynamic sites defined for keyword '%s' must have axes defined.\n", name());

	set_ = true;

	return true;
}

// Write keyword data to specified LineParser
bool DynamicSiteNodesKeyword::write(LineParser& parser, const char* keywordName, const char* prefix)
{
	// Loop over list of dynamic sites in the RefList
	for(auto dynamicSite : data_) if (!dynamicSite->write(parser, prefix)) return false;

	return true;
}

/*
 * Object Management
 */
