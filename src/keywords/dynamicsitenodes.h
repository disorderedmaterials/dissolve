// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "expression/node.h"
#include "keywords/base.h"

// Forward Declarations
class DynamicSiteProcedureNode;
class SelectProcedureNode;

// Keyword with vector of dynamic site references
class DynamicSiteNodesKeyword : public KeywordBase
{
    public:
    DynamicSiteNodesKeyword(std::vector<std::shared_ptr<DynamicSiteProcedureNode>> &data, SelectProcedureNode *parentNode,
                            bool axesRequired = false);
    ~DynamicSiteNodesKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    std::vector<std::shared_ptr<DynamicSiteProcedureNode>> &data_;
    // Parent SelectProcedureNode
    SelectProcedureNode *parentNode_;
    // Whether sites in the list must have a defined orientation
    bool axesRequired_;

    protected:
    // Determine whether current data is 'empty', and should be considered as 'not set'
    bool isDataEmpty() const override;

    public:
    // Return reference to data
    std::vector<std::shared_ptr<DynamicSiteProcedureNode>> &data();
    const std::vector<std::shared_ptr<DynamicSiteProcedureNode>> &data() const;
    // Return parent SelectProcedureNode
    const SelectProcedureNode *parentNode() const;

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const override;
    // Return maximum number of arguments accepted
    int maxArguments() const override;
    // Parse arguments from supplied LineParser, starting at given argument offset
    bool read(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Write keyword data to specified LineParser
    bool write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
};
