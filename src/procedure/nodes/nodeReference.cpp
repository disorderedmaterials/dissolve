// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/nodeReference.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/coreData.h"

ProcedureNodeReference::ProcedureNodeReference(ConstNodeRef node) { node_ = node; }

/*
 * Data
 */

// Return target node
ConstNodeRef ProcedureNodeReference::node() { return node_; }

// Return node type
ProcedureNode::NodeType ProcedureNodeReference::type() const
{
    assert(node_);
    return node_->type();
}

// Add allowable node type
void ProcedureNodeReference::addAllowableNodeType(ProcedureNode::NodeType nt) { allowedTypes_.push_back(nt); }

// Return if node pointer is NULL
bool ProcedureNodeReference::isNull() const { return (node_ == nullptr); }

/*
 * Operators
 */

void ProcedureNodeReference::operator=(ConstNodeRef node) { node_ = node; }

void ProcedureNodeReference::operator=(const ProcedureNodeReference &nodeRef)
{
    node_ = nodeRef.node_;
    allowedTypes_ = nodeRef.allowedTypes_;
}

/*
 * Read / Write second argument was provided we assume this is the name of an AnalyseModule
 */

// Read structure from specified LineParser
bool ProcedureNodeReference::read(LineParser &parser, int startArg, const CoreData &coreData, const Procedure *procedure)
{
    node_ = nullptr;

    node_ = procedure->node(parser.argsv(startArg));

    if (!node_)
        return Messenger::error("No node named '{}' exists in the current Procedure.\n", parser.argsv(startArg));

    // Check the type of the node
    if (std::find(allowedTypes_.begin(), allowedTypes_.end(), node_->type()) == allowedTypes_.end())
        return Messenger::error("Node '{}' is not of the correct type.\n", node_->name());

    return (node_ != nullptr);
}

// Write structure to specified LineParser
bool ProcedureNodeReference::write(LineParser &parser, std::string_view prefix)
{
    return parser.writeLineF("{}  '{}'\n", prefix, node_->name());
}
