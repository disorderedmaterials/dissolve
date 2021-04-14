// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "io/export/data3d.h"
#include "procedure/nodes/node.h"
#include "procedure/nodes/nodereference.h"
#include "templates/reflist.h"

// Forward Declarations
class Collect3DProcedureNode;
class Data3D;
class LineParser;
class NodeScopeStack;

// Procedure Node - Process3D
class Process3DProcedureNode : public ProcedureNode
{
    public:
    Process3DProcedureNode(const Collect3DProcedureNode *target = nullptr);
    ~Process3DProcedureNode() override = default;

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
    // Collect3D node that we are processing
    const Collect3DProcedureNode *collectNode_;
    // Pointer to processed data (stored in processing data list)
    Data3D *processedData_;
    // Export file and format
    Data3DExportFileFormat exportFileAndFormat_;

    public:
    // Return processed data
    const Data3D &processedData() const;
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
