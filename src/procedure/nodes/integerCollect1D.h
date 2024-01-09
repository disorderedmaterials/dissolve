// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "math/integerHistogram1D.h"
#include "procedure/nodes/node.h"
#include "procedure/nodes/sequence.h"

// Forward Declarations
class CalculateProcedureNodeBase;
class LineParser;

// Procedure Node - Collect1D
class IntegerCollect1DProcedureNode : public ProcedureNode
{
    public:
    IntegerCollect1DProcedureNode(std::shared_ptr<CalculateProcedureNodeBase> observable = nullptr,
                                  ProcedureNode::NodeContext subCollectContext = ProcedureNode::AnalysisContext);
    ~IntegerCollect1DProcedureNode() override = default;

    /*
     * Data
     */
    private:
    // Observable (and associated index thereof) to bin along x
    std::pair<std::shared_ptr<const CalculateProcedureNodeBase>, int> xObservable_{nullptr, 0};
    // Histogram in which to accumulate data
    OptionalReferenceWrapper<IntegerHistogram1D> histogram_;
    // Range of the histogram for QuantityX
    std::optional<int> minimum_;
    std::optional<int> maximum_;

    public:
    // Return current data
    Data1D data() const;
    // Return accumulated data
    const Data1D &accumulatedData() const;

    /*
     * Branches
     */
    private:
    // Branch for subcollection, run if the target quantity is successfully binned
    ProcedureNodeSequence subCollectBranch_;

    public:
    // Return the branch from this node (if it has one)
    OptionalReferenceWrapper<ProcedureNodeSequence> branch() override;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
    // Finalise any necessary data after execution
    bool finalise(const ProcedureContext &procedureContext) override;
};
