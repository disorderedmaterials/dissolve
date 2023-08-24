// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "math/histogram3D.h"
#include "procedure/nodes/node.h"
#include "procedure/nodes/sequence.h"

// Forward Declarations
class CalculateProcedureNodeBase;
class LineParser;

// Procedure Node - Collect3D
class Collect3DProcedureNode : public ProcedureNode
{
    public:
    explicit Collect3DProcedureNode(std::shared_ptr<CalculateProcedureNodeBase> xObservable = nullptr,
                                    std::shared_ptr<CalculateProcedureNodeBase> yObservable = nullptr,
                                    std::shared_ptr<CalculateProcedureNodeBase> zObservable = nullptr,
                                    ProcedureNode::NodeContext subCollectContext = ProcedureNode::AnalysisContext,
                                    Vec3<double> xMinMaxBin = {0.0, 10.0, 0.05}, Vec3<double> yMinMaxBin = {0.0, 10.0, 0.05},
                                    Vec3<double> zMinMaxBin = {0.0, 10.0, 0.05});
    explicit Collect3DProcedureNode(std::shared_ptr<CalculateProcedureNodeBase> xyzObservable,
                                    ProcedureNode::NodeContext subCollectContext = ProcedureNode::AnalysisContext,
                                    Vec3<double> xMinMaxBin = {0.0, 10.0, 0.05}, Vec3<double> yMinMaxBin = {0.0, 10.0, 0.05},
                                    Vec3<double> zMinMaxBin = {0.0, 10.0, 0.05});
    ~Collect3DProcedureNode() override = default;

    /*
     * Data
     */
    private:
    // Observable (and associated index thereof) to bin along x
    std::pair<std::shared_ptr<const CalculateProcedureNodeBase>, int> xObservable_{nullptr, 0};
    // Observable (and associated index thereof) to bin along y
    std::pair<std::shared_ptr<const CalculateProcedureNodeBase>, int> yObservable_{nullptr, 0};
    // Observable (and associated index thereof) to bin along z
    std::pair<std::shared_ptr<const CalculateProcedureNodeBase>, int> zObservable_{nullptr, 0};
    // Histogram in which to accumulate data
    OptionalReferenceWrapper<Histogram3D> histogram_;
    // Range and binwidth of the histogram for QuantityX
    Vec3<double> rangeX_{0.0, 10.0, 0.05};
    // Range and binwidth of the histogram for QuantityY
    Vec3<double> rangeY_{0.0, 10.0, 0.05};
    // Range and binwidth of the histogram for QuantityZ
    Vec3<double> rangeZ_{0.0, 10.0, 0.05};

    public:
    // Return accumulated data
    const Data3D &accumulatedData() const;

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
