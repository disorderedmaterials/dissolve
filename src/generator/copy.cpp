// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/copy.h"
#include "classes/configuration.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/speciesVector.h"
#include <algorithm>

CopyProcedureNode::CopyProcedureNode(Configuration *cfg) : ProcedureNode(NodeType::Copy), source_(cfg)
{
    keywords_.setOrganisation("Options", "Configuration");
    keywords_.add<ConfigurationKeyword>("Source", "Source configuration to copy", source_);
    keywords_.add<SpeciesVectorKeyword>("Exclude", "Species types to exclude from copy", excludedSpecies_);
    keywords_.add<BoolKeyword>("CopyGlobalPotentials", "Whether to copy global potentials defined in the source configuration",
                               copyGlobalPotentials_);
}

/*
 * Identity
 */

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

    // Copy global potentials
    if (copyGlobalPotentials_)
    {
        for (const auto &gpot : source_->globalPotentials())
        {
            if (gpot->type() == ExternalPotentialTypes::ExternalPotentialType::Regional)
            {
                Messenger::warn("Can't copy Regional-type global potentials - it will not exist in the new configuration.\n");
                continue;
            }

            procedureContext.configuration()->addGlobalPotential(gpot->duplicate());
        }
    }

    cfg->updateObjectRelationships();

    return true;
}
