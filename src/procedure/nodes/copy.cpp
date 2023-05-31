// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/copy.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "keywords/configuration.h"
#include "keywords/speciesvector.h"
#include <algorithm>

CopyProcedureNode::CopyProcedureNode(Configuration *cfg) : ProcedureNode(ProcedureNode::NodeType::Copy), source_(cfg)
{
    keywords_.setOrganisation("Options", "Target");
    keywords_.add<ConfigurationKeyword>("Source", "Source configuration to copy", source_);
    keywords_.add<SpeciesVectorKeyword>("Exclude", "Species types to exclude from copy", excludedSpecies_);
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool CopyProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::GenerationContext);
}

// Return whether a name for the node must be provided
bool CopyProcedureNode::mustBeNamed() const { return false; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool CopyProcedureNode::prepare(const ProcedureContext &procedureContext)
{
    // Check for valid source configuration
    if (!source_)
        return Messenger::error("No source configuration set in node '{}'.\n", name());
    if (source_ == procedureContext.configuration())
        return Messenger::error("Source configuration set in node '{}' is the same as the context target.\n", name());

    return true;
}

// Execute node
bool CopyProcedureNode::execute(const ProcedureContext &procedureContext)
{
    auto *cfg = procedureContext.configuration();

    // Raise error if the destination configuration is not empty
    if (cfg->nMolecules() != 0)
        return Messenger::error("Context configuration for copy must be empty.\n");

    // Copy the source box
    cfg->createBox(source_->box()->axes());

    // Pre-determine array sizes
    auto nAtoms = source_->nAtoms();
    if (!excludedSpecies_.empty())
    {
        nAtoms = 0;
        for (const auto &mol : source_->molecules())
            if (std::find(excludedSpecies_.begin(), excludedSpecies_.end(), mol->species()) == excludedSpecies_.end())
                nAtoms += mol->nAtoms();
    }
    procedureContext.configuration()->atoms().reserve(nAtoms);

    // Copy molecules
    for (const auto &mol : source_->molecules())
    {
        // Check for exclusion
        if (std::find(excludedSpecies_.begin(), excludedSpecies_.end(), mol->species()) != excludedSpecies_.end())
            continue;

        // Copy the molecule
        cfg->copyMolecule(mol);
    }

    cfg->updateObjectRelationships();

    return true;
}
