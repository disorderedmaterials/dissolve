// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
    ~Collect2DProcedureNode() override = default;

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
    // Observable to bin along x (retrieved from keyword)
    CalculateProcedureNodeBase *xObservable_;
    // Index of x observable data to use (retrieved from keyword)
    int xObservableIndex_;
    // Observable to bin along y (retrieved from keyword)
    CalculateProcedureNodeBase *yObservable_;
    // Index of y observable data to use (retrieved from keyword)
    int yObservableIndex_;
    // Histogram in which to accumulate data
    OptionalReferenceWrapper<Histogram2D> histogram_;

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
    bool hasBranch() const override;
    // Return SequenceNode for the branch (if it exists)
    SequenceProcedureNode *branch() override;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
    // Execute node, targetting the supplied Configuration
    bool execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
    // Finalise any necessary data after execution
    bool finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};
