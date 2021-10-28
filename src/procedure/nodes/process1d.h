// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "io/export/data1d.h"
#include "procedure/nodes/node.h"

// Forward Declarations
class Collect1DProcedureNode;
class Data1D;
class LineParser;
class NodeScopeStack;

// Procedure Node - Process1D
class Process1DProcedureNode : public ProcedureNode
{
    public:
    Process1DProcedureNode(Collect1DProcedureNode *target = nullptr);
    ~Process1DProcedureNode() override = default;

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
    // Whether to use only the current binned data of the histogram, rather than the accumulated average
    bool currentDataOnly_{false};
    // Collect1D node that we are processingg
    const Collect1DProcedureNode *sourceData_;
    // Export file and format for processed data
    Data1DExportFileFormat exportFileAndFormat_;
    // Label for the value axis
    std::string labelValue_{"Y"};
    // Label for the x axis
    std::string labelX_{"X"};
    // Pointer to processed data (stored in processing data list)
    Data1D *processedData_;

    public:
    // Return whether processed data exists
    bool hasProcessedData() const;
    // Return processed data
    const Data1D &processedData() const;
    // Return value label
    std::string valueLabel() const;
    // Return x axis label
    std::string xAxisLabel() const;

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
