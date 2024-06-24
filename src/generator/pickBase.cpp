// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/pickBase.h"
#include "classes/configuration.h"
#include "keywords/node.h"

PickGeneratorNodeBase::PickGeneratorNodeBase(GeneratorNode::NodeType nodeType) : GeneratorNode(nodeType), selection_(nullptr)
{
    keywords_.setOrganisation("Options", "Restrictions");
    keywords_.add<NodeKeyword<PickGeneratorNodeBase>>(
        "From", "Existing picked selection of molecules from which to pick", selection_, this,
        NodeTypeVector{NodeType::Pick, NodeType::PickProximity, NodeType::PickRegion});
}

/*
 * Picked Molecules
 */

// Return source molecule pool
const std::vector<std::shared_ptr<Molecule>> &PickGeneratorNodeBase::moleculePool(const Configuration *cfg) const
{
    return selection_ ? selection_->pickedMolecules() : cfg->molecules();
}

// Return source molecule pool name
std::string PickGeneratorNodeBase::moleculePoolName() const
{
    return selection_ ? fmt::format("picked selection '{}'", selection_->name()) : "configuration";
}

// Return vector of picked Molecules
std::vector<std::shared_ptr<Molecule>> &PickGeneratorNodeBase::pickedMolecules() { return pickedMolecules_; }
const std::vector<std::shared_ptr<Molecule>> &PickGeneratorNodeBase::pickedMolecules() const { return pickedMolecules_; }

/*
 * Execution
 */

// Finalise any necessary data after execution
bool PickGeneratorNodeBase::finalise(const ProcedureContext &procedureContext)
{
    // Clear picked molecules
    pickedMolecules_.clear();

    return true;
}
