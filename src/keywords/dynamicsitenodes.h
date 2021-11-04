// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "expression/node.h"
#include "keywords/data.h"
#include "templates/list.h"

// Forward Declarations
class DynamicSiteProcedureNode;
class SelectProcedureNode;

// Keyword with DynamicSiteProcedureNode (Ref)List
class DynamicSiteNodesKeyword : public KeywordData<std::vector<std::shared_ptr<DynamicSiteProcedureNode>> &>
{
    public:
    DynamicSiteNodesKeyword(std::shared_ptr<SelectProcedureNode> parentNode, std::vector<std::shared_ptr<DynamicSiteProcedureNode>> &nodes,
                            bool axesRequired = false);
    ~DynamicSiteNodesKeyword() override;

    /*
     * Parent Node
     */
    private:
    // Parent SelectProcedureNode
    std::shared_ptr<SelectProcedureNode> parentNode_;

    public:
    // Return parent SelectProcedureNode
    std::shared_ptr<const SelectProcedureNode> parentNode() const;

    /*
     * Specification
     */
    private:
    // Whether sites in the list must have a defined orientation
    bool axesRequired_;

    /*
     * Data
     */
    protected:
    // Determine whether current data is 'empty', and should be considered as 'not set'
    bool isDataEmpty() const override;

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

    /*
     * Object Management
     */
    protected:
};
