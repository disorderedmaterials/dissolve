// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/pickBase.h"
#include "classes/configuration.h"
#include "keywords/node.h"

PickProcedureNodeBase::PickProcedureNodeBase(ProcedureNode::NodeType nodeType) : ProcedureNode(nodeType), selection_(nullptr)
{
    keywords_.setOrganisation("Options", "Restrictions");
    keywords_.add<NodeKeyword<PickProcedureNodeBase>>(
        "From", "Existing picked selection of molecules from which to pick", selection_, this,
        NodeTypeVector{NodeType::Pick, NodeType::PickProximity, NodeType::PickRegion});
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
bool PickProcedureNodeBase::finalise(const ProcedureContext &procedureContext)
{
    // Clear picked molecules
    pickedMolecules_.clear();

    return true;
}
