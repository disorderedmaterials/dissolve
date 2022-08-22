// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "math/histogram2d.h"
#include "procedure/nodes/node.h"
#include "procedure/nodes/sequence.h"

// Forward Declarations
class CalculateProcedureNodeBase;
class LineParser;

// Procedure Node - Collect2D
class Collect2DProcedureNode : public ProcedureNode
{
    public:
    explicit Collect2DProcedureNode(std::shared_ptr<CalculateProcedureNodeBase> xObservable = nullptr,
                                    std::shared_ptr<CalculateProcedureNodeBase> yObservable = nullptr,
                                    ProcedureNode::NodeContext subCollectContext = ProcedureNode::AnalysisContext,
                                    double xMin = 0.0, double xMax = 10.0, double xBinWidth = 0.05, double yMin = 0.0,
                                    double yMax = 10.0, double yBinWidth = 0.05);
    ~Collect2DProcedureNode() override = default;

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context) override;

    /*
     * Data
     */
    private:
    // Observable (and associated index thereof) to bin along x
    std::pair<std::shared_ptr<const CalculateProcedureNodeBase>, int> xObservable_{nullptr, 0};
    // Observable (and associated index thereof) to bin along y
    std::pair<std::shared_ptr<const CalculateProcedureNodeBase>, int> yObservable_{nullptr, 0};
    // Histogram in which to accumulate data
    OptionalReferenceWrapper<Histogram2D> histogram_;
    // Range and binwidth of the histogram for QuantityX
    Vec3<double> rangeX_{0.0, 10.0, 0.05};
    // Range and binwidth of the histogram for QuantityY
    Vec3<double> rangeY_{0.0, 10.0, 0.05};

    public:
    // Return accumulated data
    const Data2D &accumulatedData() const;

    /*
     * Branches
     */
    private:
    // Branch for subcollection, run if the target quantity is successfully binned
    ProcedureNodeSequence subCollectBranch_;

    public:
    // Return the branch from this node (if it has one)
    OptionalReferenceWrapper<ProcedureNodeSequence> branch() override;
    // Find the nodes owned by this node
    std::vector<ConstNodeRef> children() const override;

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
