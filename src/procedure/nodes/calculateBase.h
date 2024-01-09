// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "keywords/node.h"
#include "procedure/nodes/node.h"
#include <array>

// Forward Declarations
class SelectProcedureNode;

// Calculate Node Base
class CalculateProcedureNodeBase : public ProcedureNode
{
    public:
    CalculateProcedureNodeBase(ProcedureNode::NodeType nodeType, std::shared_ptr<SelectProcedureNode> site0 = nullptr,
                               std::shared_ptr<SelectProcedureNode> site1 = nullptr,
                               std::shared_ptr<SelectProcedureNode> site2 = nullptr,
                               std::shared_ptr<SelectProcedureNode> site3 = nullptr);
    ~CalculateProcedureNodeBase() override = default;

    /*
     * Observable Target
     */
    protected:
    // Sites (SelectProcedureNodes) to use for calculation of observable (retrieved from keywords)
    std::array<std::shared_ptr<const SelectProcedureNode>, 4> sites_;
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
    bool prepare(const ProcedureContext &procedureContext) override;
};
