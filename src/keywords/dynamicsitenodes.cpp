// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/dynamicsitenodes.h"
#include "base/lineparser.h"
#include "classes/coredata.h"
#include "expression/variable.h"
#include "procedure/nodes/dynamicsite.h"
#include "procedure/nodes/select.h"

DynamicSiteNodesKeyword::DynamicSiteNodesKeyword(std::vector<std::shared_ptr<DynamicSiteProcedureNode>> &data,
                                                 SelectProcedureNode *parentNode, bool axesRequired)
    : KeywordBase(typeid(this)), data_(data), parentNode_(parentNode), axesRequired_(axesRequired)
{
}

/*
 * Data
 */

// Determine whether current data is 'empty', and should be considered as 'not set'
bool DynamicSiteNodesKeyword::isDataEmpty() const { return data_.empty(); }

// Return reference to data
dissolve::any_ptr DynamicSiteNodesKeyword::data() const { return data_; }

// Return parent SelectProcedureNode
const SelectProcedureNode *DynamicSiteNodesKeyword::parentNode() const { return parentNode_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int DynamicSiteNodesKeyword::minArguments() const { return 0; }

// Return maximum number of arguments accepted
std::optional<int> DynamicSiteNodesKeyword::maxArguments() const { return std::nullopt; }

// Deserialise from supplied LineParser, starting at given argument offset
bool DynamicSiteNodesKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (!parentNode_)
        return Messenger::error("Parent ProcedureNode not set, so can't read DynamicSiteNode data.\n");

    // Create a new DynamicSite and add it to our data RefList
    auto dynamicSite = std::make_shared<DynamicSiteProcedureNode>(
        std::dynamic_pointer_cast<SelectProcedureNode>(parentNode_->shared_from_this()));
    data_.push_back(dynamicSite);

    // Attempt to read the DynamicSite data
    if (!dynamicSite->deserialise(parser, coreData))
        return false;

    // Check for required axes?
    if (axesRequired_ && (!dynamicSite->hasAxes()))
        return Messenger::error("Dynamic sites defined for keyword '{}' must have axes defined.\n", name());

    set_ = true;

    return true;
}

// Serialise data to specified LineParser
bool DynamicSiteNodesKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    for (auto dynamicSite : data_)
        if (!dynamicSite->write(parser, prefix))
            return false;

    return true;
}
