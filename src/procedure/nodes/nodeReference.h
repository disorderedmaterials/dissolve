// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"

// Forward Declarations
class CoreData;
class Procedure;

// Procedure Node Reference
class ProcedureNodeReference
{
    public:
    ProcedureNodeReference(ConstNodeRef node = nullptr);
    ~ProcedureNodeReference() = default;

    /*
     * Data
     */
    private:
    // Target node
    ConstNodeRef node_;
    // Allowable node types (when reading / setting)
    std::vector<ProcedureNode::NodeType> allowedTypes_;

    public:
    // Return target node
    ConstNodeRef node();
    // Return node type
    ProcedureNode::NodeType type() const;
    // Add allowable node type
    void addAllowableNodeType(ProcedureNode::NodeType nt);
    // Return if node pointer is NULL
    bool isNull() const;

    /*
     * Operators
     */
    public:
    void operator=(ConstNodeRef node);
    void operator=(const ProcedureNodeReference &nodeRef);

    /*
     * Read / Write
     */
    public:
    // Read structure from specified LineParser
    bool read(LineParser &parser, int startArg, const CoreData &coreData, const Procedure *procedure);
    // Write structure to specified LineParser
    bool write(LineParser &parser, std::string_view prefix);
};
