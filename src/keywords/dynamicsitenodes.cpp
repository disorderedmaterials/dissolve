// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/dynamicsitenodes.h"
#include "base/lineparser.h"
#include "classes/coredata.h"
#include "expression/variable.h"
#include "procedure/nodes/dynamicsite.h"

DynamicSiteNodesKeyword::DynamicSiteNodesKeyword(SelectProcedureNode *parentNode, RefList<DynamicSiteProcedureNode> &nodes,
                                                 bool axesRequired)
    : KeywordData<RefList<DynamicSiteProcedureNode> &>(KeywordBase::DynamicSiteNodesData, nodes)
{
    parentNode_ = parentNode;
    axesRequired_ = axesRequired;
}

DynamicSiteNodesKeyword::~DynamicSiteNodesKeyword() {}

/*
 * Parent Node
 */

// Return parent SelectProcedureNode
const SelectProcedureNode *DynamicSiteNodesKeyword::parentNode() const { return parentNode_; }

/*
 * Data
 */

// Determine whether current data is 'empty', and should be considered as 'not set'
bool DynamicSiteNodesKeyword::isDataEmpty() const { return data_.nItems() == 0; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int DynamicSiteNodesKeyword::minArguments() const { return 0; }

// Return maximum number of arguments accepted
int DynamicSiteNodesKeyword::maxArguments() const { return 0; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool DynamicSiteNodesKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (!parentNode_)
        return Messenger::error("Parent ProcedureNode not set, so can't read DynamicSiteNode data.\n");

    // Create a new DynamicSite and add it to our data RefList
    auto *dynamicSite = new DynamicSiteProcedureNode(parentNode_);
    data_.append(dynamicSite);

    // Attempt to read the DynamicSite data
    if (!dynamicSite->deserialise(parser, coreData))
        return false;

    // Check for required axes?
    if (axesRequired_ && (!dynamicSite->hasAxes()))
        return Messenger::error("Dynamic sites defined for keyword '{}' must have axes defined.\n", name());

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool DynamicSiteNodesKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    // Loop over list of dynamic sites in the RefList
    for (auto dynamicSite : data_)
        if (!dynamicSite->write(parser, prefix))
            return false;

    return true;
}

/*
 * Object Management
 */
