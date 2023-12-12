// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/nodeReference.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/coreData.h"
#include "modules/analyse/analyse.h"

ProcedureNodeReference::ProcedureNodeReference(ConstNodeRef node)
{
    node_ = node;

    analyseModuleParent_ = nullptr;
}

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

    // If two arguments are provided, the second is the identifying name of an AnalyseModule
    if (parser.nArgs() == (startArg + 2))
    {
	Module *module = Module::find(coreData, parser.argsv(startArg + 1));
        if (!module)
            return Messenger::error("No Analyse module named '{}' exists.\n", parser.argsv(startArg + 1));
        if (module->type() != ModuleTypes::Analyse)
            return Messenger::error("Specified module '{}' must be an Analyse module.\n", parser.argsv(startArg + 1));

        // Found the target AnalyseModule, so cast it up and search for the named node in its Analyser
        analyseModuleParent_ = dynamic_cast<AnalyseModule *>(module);
        if (!analyseModuleParent_)
            return Messenger::error("Couldn't cast module into an AnalyseModule.\n");

        node_ = analyseModuleParent_->analyser().node(parser.argsv(startArg));

        if (!node_)
            return Messenger::error("No node named '{}' exists in the Analyse module specified ({}).\n", parser.argsv(startArg),
                                    parser.argsv(startArg + 1));
    }
    else
    {
        node_ = procedure->node(parser.argsv(startArg));

        if (!node_)
            return Messenger::error("No node named '{}' exists in the current Procedure.\n", parser.argsv(startArg));
    }

    // Check the type of the node
    if (std::find(allowedTypes_.begin(), allowedTypes_.end(), node_->type()) == allowedTypes_.end())
        return Messenger::error("Node '{}' is not of the correct type.\n", node_->name());

    return (node_ != nullptr);
}

// Write structure to specified LineParser
bool ProcedureNodeReference::write(LineParser &parser, std::string_view prefix)
{
    if (analyseModuleParent_)
        return parser.writeLineF("{}  '{}'  '{}'\n", prefix, node_->name(), analyseModuleParent_->name());
    else
        return parser.writeLineF("{}  '{}'\n", prefix, node_->name());
}
