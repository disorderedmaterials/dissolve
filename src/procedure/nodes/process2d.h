// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "io/export/data2d.h"
#include "procedure/nodes/node.h"

// Forward Declarations
class Collect2DProcedureNode;
class Data2D;
class LineParser;
class NodeScopeStack;

// Procedure Node - Process2D
class Process2DProcedureNode : public ProcedureNode
{
    public:
    Process2DProcedureNode(Collect2DProcedureNode *target = nullptr);
    ~Process2DProcedureNode() override = default;

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
    // Collect2D node that we are processing
    const Collect2DProcedureNode *sourceData_{nullptr};
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
    // Branch for normalisation of data (if defined)
    SequenceProcedureNode *normalisationBranch_;

    public:
    // Add and return normalisation sequence branch
    SequenceProcedureNode *addNormalisationBranch();
    // Return whether this node has a branch
    bool hasBranch() const override;
    // Return SequenceNode for the branch (if it exists)
    SequenceProcedureNode *branch() override;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
    // Finalise any necessary data after execution
    bool finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};
