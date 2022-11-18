// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

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

// Return reference to data
std::vector<std::shared_ptr<DynamicSiteProcedureNode>> &DynamicSiteNodesKeyword::data() { return data_; }
const std::vector<std::shared_ptr<DynamicSiteProcedureNode>> &DynamicSiteNodesKeyword::data() const { return data_; }

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

    // Create a new DynamicSite and add it to our data vector
    auto dynamicSite = std::make_shared<DynamicSiteProcedureNode>(
        std::dynamic_pointer_cast<SelectProcedureNode>(parentNode_->shared_from_this()));
    data_.push_back(dynamicSite);

    // Attempt to read the DynamicSite data
    if (!dynamicSite->deserialise(parser, coreData))
        return false;

    // Check for required axes?
    if (axesRequired_ && (!dynamicSite->hasAxes()))
        return Messenger::error("Dynamic sites defined for keyword '{}' must have axes defined.\n", name());

    return true;
}

// Serialise data to specified LineParser
bool DynamicSiteNodesKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    for (auto dynamicSite : data_)
        if (!dynamicSite->serialise(parser, prefix))
            return false;

    return true;
}

// Express as a serialisable value
SerialisedValue DynamicSiteNodesKeyword::serialise() const
{
    return fromVector(data_, [](const auto &item) -> SerialisedValue { return *item; });
}

// Read values from a serialisable value
void DynamicSiteNodesKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    for (auto n : node.as_array())
    {
        auto dynamicSite = std::make_shared<DynamicSiteProcedureNode>(
            std::dynamic_pointer_cast<SelectProcedureNode>(parentNode_->shared_from_this()));
        dynamicSite->deserialise(node, coreData);
        data_.push_back(dynamicSite);
    }
}
// Has not changed from initial value
bool DynamicSiteNodesKeyword::isDefault() const { return data_.empty(); }
