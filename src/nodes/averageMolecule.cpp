// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/averageMolecule.h"

AverageMoleculeNode::AverageMoleculeNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Configuration", "Set target configuration for the node", configuration_)->setFlags({ParameterBase::Required});

    addOption("Site", "Target site about which to calculate average molecule geometry", targetSite_)
        ->setFlags({ParameterBase::Required});
}

/*
 * Definition
 */

std::string_view AverageMoleculeNode::type() const { return "AverageMolecule"; }

std::string_view AverageMoleculeNode::summary() const
{
    return "Calculate average atomic positions of a molecule type around an oriented site";
}

/*
 * Data
 */

// Clear any local data
void AverageMoleculeNode::clearData()
{
    sampledX_.reset();
    sampledY_.reset();
    sampledZ_.reset();
    structure_.clear();
}

// Temporary accessors to data for testing
const Structure &AverageMoleculeNode::structure() const { return structure_; }

/*
 * Processing
 */

// Update the average structure from sampled coordinate vectors
void AverageMoleculeNode::updateStructure()
{
    for (auto &&[i, rx, ry, rz] : zip(structure_.atoms(), sampledX_->values(), sampledY_->values(), sampledZ_->values()))
        i->setR({rx, ry, rz});
}

// Run main processing
NodeConstants::ProcessResult AverageMoleculeNode::process()
{
    // Get parent Species of site
    auto *targetSpecies = targetSite_->parent();
    auto requiredSize = targetSpecies->nAtoms();

    // Initialise data if required
    if (!sampledX_)
    {
        message("Initialising arrays for average molecule: size = {}\n", requiredSize);

        sampledX_.emplace().initialise(requiredSize);
        sampledY_.emplace().initialise(requiredSize);
        sampledZ_.emplace().initialise(requiredSize);

        // Copy current geometry of species to set up atom / bond relationships in the Structure
        structure_ = targetSpecies->asStructure();
    }

    // Grab Box pointer
    const auto &box = configuration_->box();

    // Get the site stack
    const auto *stack = configuration_->siteStack(targetSite_);

    // Loop over sites
    std::vector<double> rx(requiredSize), ry(requiredSize), rz(requiredSize);
    for (auto n = 0; n < stack->nSites(); ++n)
    {
        const auto &s = stack->site(n);

        // Get axes and take inverse
        auto inverseAxes = s.axes();
        inverseAxes.invert();

        // Loop over atoms, taking delta position with origin, and rotating into local axes
        for (auto &&[i, x, y, z] : zip(s.molecule()->atoms(), rx, ry, rz))
        {
            auto r = inverseAxes * box.minimumVector(s.origin(), i->r());
            x = r.x;
            y = r.y;
            z = r.z;
        }

        // Accumulate positions
        (*sampledX_) += rx;
        (*sampledY_) += ry;
        (*sampledZ_) += rz;
    }

    updateStructure();

    // // Export data?
    // if (exportFileAndFormat_.hasFilename())
    // {
    //     if (!exportFileAndFormat_.exportData(&averageSpecies_))
    //         return ExecutionResult::Failed;
    // }

    return NodeConstants::ProcessResult::Success;
}
