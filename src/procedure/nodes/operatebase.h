// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/node.h"
#include "procedure/nodes/node.h"
#include "templates/array.h"

// Forward Declarations
class Data1D;
class Data2D;
class Data3D;

// Operate Node Base
class OperateProcedureNodeBase : public ProcedureNode
{
    public:
    OperateProcedureNodeBase(ProcedureNode::NodeType nodeType);
    virtual ~OperateProcedureNodeBase() override = default;

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context) override;
    // Return whether a name for the node must be provided
    bool mustBeNamed() const;

    /*
     * Data Target
     */
    protected:
    // Target Data1D (if relevant)
    Data1D *targetData1D_;
    // Target Data2D (if relevant)
    Data2D *targetData2D_;
    // Target Data3D (if relevant)
    Data3D *targetData3D_;

    public:
    // Set target Data1D
    void setTarget(Data1D *target);
    // Set target Data2D
    void setTarget(Data2D *target);
    // Set target Data3D
    void setTarget(Data3D *target);
    // Operate on Data1D target
    virtual bool operateData1D(ProcessPool &procPool, Configuration *cfg);
    // Operate on Data2D target
    virtual bool operateData2D(ProcessPool &procPool, Configuration *cfg);
    // Operate on Data3D target
    virtual bool operateData3D(ProcessPool &procPool, Configuration *cfg);

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
    // Execute node, targetting the supplied Configuration
    bool execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};
