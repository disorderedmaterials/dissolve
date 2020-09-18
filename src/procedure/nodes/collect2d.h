/*
    *** Procedure Node - Collect2D
    *** src/procedure/nodes/collect2d.h
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

#include "math/histogram2d.h"
#include "procedure/nodes/node.h"

// Forward Declarations
class CalculateProcedureNodeBase;
class SequenceProcedureNode;
class LineParser;
class NodeScopeStack;

// Procedure Node - Collect2D
class Collect2DProcedureNode : public ProcedureNode
{
    public:
    Collect2DProcedureNode(CalculateProcedureNodeBase *xObservable = nullptr, CalculateProcedureNodeBase *yObservable = nullptr,
                           double xMin = 0.0, double xMax = 10.0, double xBinWidth = 0.05, double yMin = 0.0,
                           double yMax = 10.0, double yBinWidth = 0.05);
    ~Collect2DProcedureNode();

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
    // Observable to bin along x (retrieved from keyword)
    CalculateProcedureNodeBase *xObservable_;
    // Index of x observable data to use (retrieved from keyword)
    int xObservableIndex_;
    // Observable to bin along y (retrieved from keyword)
    CalculateProcedureNodeBase *yObservable_;
    // Index of y observable data to use (retrieved from keyword)
    int yObservableIndex_;
    // Histogram in which to accumulate data
    Histogram2D *histogram_;

    public:
    // Return accumulated data
    const Data2D &accumulatedData() const;
    // Return x range minimum
    double xMinimum() const;
    // Return x range maximum
    double xMaximum() const;
    // Return x bin width
    double xBinWidth() const;
    // Return y range minimum
    double yMinimum() const;
    // Return y range maximum
    double yMaximum() const;
    // Return y bin width
    double yBinWidth() const;

    /*
     * Branches
     */
    private:
    // Branch for subcollection (if defined), run if the target quantity is successfully binned
    SequenceProcedureNode *subCollectBranch_;

    public:
    // Add and return subcollection sequence branch
    SequenceProcedureNode *addSubCollectBranch(ProcedureNode::NodeContext context);
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
