// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/export/data3D.h"
#include "procedure/nodes/node.h"
#include "procedure/nodes/nodeReference.h"
#include "procedure/nodes/sequence.h"

// Forward Declarations
class Collect3DProcedureNode;
class Data3D;
class LineParser;

// Procedure Node - Process3D
class Process3DProcedureNode : public ProcedureNode
{
    public:
    Process3DProcedureNode(std::shared_ptr<Collect3DProcedureNode> target = nullptr,
                           ProcedureNode::NodeContext forEachContext = ProcedureNode::OperateContext);
    ~Process3DProcedureNode() override = default;

    /*
     * Data
     */
    private:
    // Collect3D node that we are processing
    std::shared_ptr<const Collect3DProcedureNode> sourceData_;
    // Export file and format
    Data3DExportFileFormat exportFileAndFormat_;
    // Label for the value axis
    std::string labelValue_{"Counts"};
    // Label for the x axis
    std::string labelX_{"X"};
    // Label for the y axis
    std::string labelY_{"Y"};
    // Label for the z axis
    std::string labelZ_{"Z"};
    // Pointer to processed data (stored in processing data list)
    Data3D *processedData_;

    public:
    // Return processed data
    const Data3D &processedData() const;
    // Return export file and format for processed data
    Data3DExportFileFormat &exportFileAndFormat();
    // Return value label
    std::string valueLabel() const;
    // Return x axis label
    std::string xAxisLabel() const;
    // Return y axis label
    std::string yAxisLabel() const;
    // Return z axis label
    std::string zAxisLabel() const;

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
