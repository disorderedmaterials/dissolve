// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "math/range.h"
#include "math/sampledDouble.h"
#include "procedure/nodes/node.h"

// Forward Declarations
class Process1DProcedureNode;
class LineParser;

// Procedure Node - Integrate1D
class Integrate1DProcedureNode : public ProcedureNode
{
    public:
    Integrate1DProcedureNode(std::shared_ptr<Process1DProcedureNode> target = nullptr);
    ~Integrate1DProcedureNode() override = default;

    /*
     * Data
     */
    private:
    // Process1D node that we are targetting
    std::shared_ptr<const Process1DProcedureNode> sourceData_;
    // Ranges for integration regions
    Range range_[3] = {{0.0, 3.0}, {3.0, 6.0}, {6.0, 9.0}};
    // Calculated integral (stored in processing data list)
    SampledDouble integral_[3];

    public:
    // Return calculated integral specified
    const SampledDouble &integral(int index) const;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Finalise any necessary data after execution
    bool finalise(const ProcedureContext &procedureContext) override;
};
