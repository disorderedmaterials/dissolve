// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/export/data1D.h"
#include "procedure/nodes/node.h"
#include "procedure/nodes/sequence.h"

// Forward Declarations
class Collect1DProcedureNode;
class Data1D;
class IntegerCollect1DProcedureNode;
class LineParser;

// Procedure Node - Process1D
class Process1DProcedureNode : public ProcedureNode
{
    public:
    Process1DProcedureNode(std::shared_ptr<Collect1DProcedureNode> target = nullptr,
                           ProcedureNode::NodeContext normalisationContext = ProcedureNode::OperateContext);
    Process1DProcedureNode(std::shared_ptr<IntegerCollect1DProcedureNode> intTarget,
                           ProcedureNode::NodeContext normalisationContext = ProcedureNode::OperateContext);
    ~Process1DProcedureNode() override = default;

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
    // Export file and format for processed data
    Data1DExportFileFormat exportFileAndFormat_;
    // Label for the value axis
    std::string labelValue_;
    // Label for the x axis
    std::string labelX_{"X"};
    // Pointer to processed data (stored in processing data list)
    Data1D *processedData_{nullptr};

    public:
    // Return whether processed data exists
    bool hasProcessedData() const;
    // Return processed data
    const Data1D &processedData() const;
    // Return export file and format for processed data
    Data1DExportFileFormat &exportFileAndFormat();
    // Return value label
    std::string valueLabel() const;
    // Return x axis label
    std::string xAxisLabel() const;

    /*
     * Branches
     */
    private:
    // Branch for normalisation of data
    ProcedureNodeSequence normalisationBranch_;

    public:
    // Return the branch from this node (if it has one)
    OptionalReferenceWrapper<ProcedureNodeSequence> branch() override;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Finalise any necessary data after execution
    bool finalise(const ProcedureContext &procedureContext) override;
};
