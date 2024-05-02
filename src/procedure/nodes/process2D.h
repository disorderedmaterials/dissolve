// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/export/data2D.h"
#include "procedure/nodes/node.h"
#include "procedure/nodes/sequence.h"

// Forward Declarations
class Collect2DProcedureNode;
class Data2D;
class LineParser;

// Procedure Node - Process2D
class Process2DProcedureNode : public ProcedureNode
{
    public:
    Process2DProcedureNode(std::shared_ptr<Collect2DProcedureNode> target = nullptr,
                           ProcedureNode::NodeContext forEachContext = ProcedureNode::OperateContext);
    ~Process2DProcedureNode() override = default;

    /*
     * Data
     */
    private:
    // Collect2D node that we are processing
    std::shared_ptr<const Collect2DProcedureNode> sourceData_;
    // Export file and format for processed data
    Data2DExportFileFormat exportFileAndFormat_;
    // Label for the value axis
    std::string labelValue_{"Counts"};
    // Label for the x axis
    std::string labelX_{"X"};
    // Label for the y axis
    std::string labelY_{"Y"};
    // Pointer to processed data (stored in processing data list)
    Data2D *processedData_{nullptr};

    public:
    // Return processed data
    const Data2D &processedData() const;
    // Return export file and format for processed data
    Data2DExportFileFormat &exportFileAndFormat();
    // Return value label
    std::string valueLabel() const;
    // Return x axis label
    std::string xAxisLabel() const;
    // Return y axis label
    std::string yAxisLabel() const;

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
