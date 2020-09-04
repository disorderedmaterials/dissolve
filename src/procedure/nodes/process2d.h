/*
    *** Procedure Node - Process2D
    *** src/procedure/nodes/process2d.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "procedure/nodes/node.h"
#include "templates/reflist.h"

// Forward Declarations
class Collect2DProcedureNode;
class Data2D;
class LineParser;
class NodeScopeStack;

// Procedure Node - Process2D
class Process2DProcedureNode : public ProcedureNode
{
    public:
    Process2DProcedureNode(const Collect2DProcedureNode *target = nullptr);
    ~Process2DProcedureNode();

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
    // Collect2D node that we are processing
    const Collect2DProcedureNode *collectNode_;
    // Pointer to processed data (stored in processing data list)
    Data2D *processedData_;

    public:
    // Return processed data
    const Data2D &processedData() const;
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
