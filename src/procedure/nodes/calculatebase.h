// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/node.h"
#include "procedure/nodes/node.h"
#include "templates/array.h"

// Forward Declarations
class SelectProcedureNode;

// Calculate Node Base
class CalculateProcedureNodeBase : public ProcedureNode
{
    public:
    CalculateProcedureNodeBase(ProcedureNode::NodeType nodeType, SelectProcedureNode *site0 = nullptr,
                               SelectProcedureNode *site1 = nullptr, SelectProcedureNode *site2 = nullptr,
                               SelectProcedureNode *site3 = nullptr);
    virtual ~CalculateProcedureNodeBase() override = default;

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context) override;

    /*
     * Keywords
     */
    protected:
    // Pointers to available site node keywords
    NodeKeyword<SelectProcedureNode> *siteKeywords_[4];

    /*
     * Observable Target
     */
    protected:
    // Sites (SelectProcedureNodes) to use for calculation of observable (retrieved from keywords)
    SelectProcedureNode *sites_[4];
    // Last calculate value(s) of observable (as Vec3)
    Vec3<double> value_;

    public:
    // Return last calculated value of observable
    double value(int id) const;
    // Return last calculated value as vector
    Vec3<double> values() const;
    // Return number of sites required to calculate observable
    virtual int nSitesRequired() const = 0;
    // Return dimensionality of calculated observable
    virtual int dimensionality() const = 0;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};
