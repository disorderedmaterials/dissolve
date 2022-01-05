// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "procedure/nodes/pickbase.h"
#include "classes/configuration.h"
#include "keywords/node.h"

PickProcedureNodeBase::PickProcedureNodeBase(ProcedureNode::NodeType nodeType)
    : ProcedureNode(nodeType, ProcedureNode::NodeClass::Pick), selection_(nullptr)
{
    keywords_.add<NodeKeyword<PickProcedureNodeBase>>("Control", "From", "Previous selection of molecules from which to pick",
                                                      selection_, this, ProcedureNode::NodeClass::Pick, true);
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
    return selection_ ? selection_->pickedMolecules() : cfg->molecules();
}

// Return source molecule pool name
std::string PickProcedureNodeBase::moleculePoolName() const
{
    return selection_ ? fmt::format("picked selection '{}'", selection_->name()) : "configuration";
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
