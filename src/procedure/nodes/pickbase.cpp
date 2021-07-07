// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/pickbase.h"
#include "classes/configuration.h"
#include "keywords/node.h"

PickProcedureNodeBase::PickProcedureNodeBase(ProcedureNode::NodeType nodeType)
    : ProcedureNode(nodeType, ProcedureNode::NodeClass::Pick)
{
    keywords_.add("Control", new NodeKeyword(this, ProcedureNode::NodeClass::Pick, true), "Source",
                  "Previous selection of molecules from which to pick");
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool PickProcedureNodeBase::isContextRelevant(ProcedureNode::NodeContext context)
{
    return context == ProcedureNode::GenerationContext;
}

/*
 * Picked Molecules
 */

// Return source molecule pool
const std::vector<std::shared_ptr<Molecule>> &PickProcedureNodeBase::moleculePool(const Configuration *cfg) const
{
    auto *node = keywords_.retrieve<const ProcedureNode *>("Source");
    if (node)
    {
        auto *pickNode = dynamic_cast<const PickProcedureNodeBase *>(node);
        assert(pickNode);
        return pickNode->pickedMolecules();
    }

    return cfg->molecules();
}

// Return source molecule pool name
std::string PickProcedureNodeBase::moleculePoolName() const
{
    auto *node = keywords_.retrieve<const ProcedureNode *>("Source");
    if (node)
    {
        auto *pickNode = dynamic_cast<const PickProcedureNodeBase *>(node);
        assert(pickNode);
        return fmt::format("picked selection '{}'", pickNode->name());
    }

    return "configuration";
}

// Return vector of picked Molecules
std::vector<std::shared_ptr<Molecule>> &PickProcedureNodeBase::pickedMolecules() { return pickedMolecules_; }
const std::vector<std::shared_ptr<Molecule>> &PickProcedureNodeBase::pickedMolecules() const { return pickedMolecules_; }

/*
 * Execution
 */

// Finalise any necessary data after execution
bool PickProcedureNodeBase::finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                     GenericList &targetList)
{
    // Clear picked molecules
    pickedMolecules_.clear();

    return true;
}
