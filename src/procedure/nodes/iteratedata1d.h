// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "io/export/data1D.h"
#include "procedure/nodes/node.h"
#include "procedure/nodes/sequence.h"

// Forward Declarations
class Collect1DProcedureNode;
class Data1D;
class IntegerCollect1DProcedureNode;
class LineParser;

// Procedure Node - IterateData1D
class IterateData1DProcedureNode : public ProcedureNode
{
    public:
    IterateData1DProcedureNode(std::shared_ptr<Collect1DProcedureNode> target = nullptr,
                               ProcedureNode::NodeContext normalisationContext = ProcedureNode::OperateContext);
    IterateData1DProcedureNode(std::shared_ptr<IntegerCollect1DProcedureNode> intTarget,
                               ProcedureNode::NodeContext normalisationContext = ProcedureNode::OperateContext);
    ~IterateData1DProcedureNode() override = default;

    private:
    // Set up keywords for node
    void setUpKeywords();

    /*
     * Data
     */
    // Whether to use only the current binned data of the histogram, rather than the accumulated average
    bool instantaneous_{false};
    // Collect1D node that we are processing
    std::shared_ptr<const Collect1DProcedureNode> sourceData_;
    std::shared_ptr<const IntegerCollect1DProcedureNode> sourceIntegerData_;
    // Pointer to processed data (stored in processing data list)
    Data1D *processedData_{nullptr};

    public:
    // Return whether processed data exists
    bool hasProcessedData() const;
    // Return processed data
    const Data1D &processedData() const;

    /*
     * Branches
     */
    private:
    // Branch for normalisation of data
    ProcedureNodeSequence normalisationBranch_;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Finalise any necessary data after execution
    bool finalise(const ProcedureContext &procedureContext) override;
};
