// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "io/export/data1d.h"
#include "procedure/nodes/node.h"
#include "templates/reflist.h"

// Forward Declarations
class Collect1DProcedureNode;
class Data1D;
class LineParser;
class NodeScopeStack;

// Procedure Node - Process1D
class Process1DProcedureNode : public ProcedureNode
{
    public:
    Process1DProcedureNode(const Collect1DProcedureNode *target = nullptr);
    ~Process1DProcedureNode();

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context);

    /*
     * Data
     */
    private:
    // Collect1D node that we are processing (retrieved from keyword 'SourceData')
    const Collect1DProcedureNode *collectNode_;
    // Pointer to processed data (stored in processing data list)
    Data1D *processedData_;
    // Export file and format for saving data
    Data1DExportFileFormat exportFileAndFormat_;

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
    bool hasBranch() const;
    // Return SequenceNode for the branch (if it exists)
    SequenceProcedureNode *branch();

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList);
    // Execute node, targetting the supplied Configuration
    ProcedureNode::NodeExecutionResult execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                               GenericList &targetList);
    // Finalise any necessary data after execution
    bool finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList);
};
