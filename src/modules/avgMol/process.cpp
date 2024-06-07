// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/box.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/avgMol/avgMol.h"
#include "templates/algorithms.h"

// Run set-up stage
bool AvgMolModule::setUp(ModuleContext &moduleContext, Flags<KeywordBase::KeywordSignal> actionSignals)
{
    // Clear species
    averageSpecies_.clear();

    // If the targetSpecies_ is different from the current target site, or the site is nullptr, clear the arrays
    if (!targetSite_)
        targetSpecies_ = nullptr;
    else
    {
        if (targetSite_->parent() == nullptr)
        {
            targetSpecies_ = nullptr;
            return Messenger::error("Target site has no parent species.\n");
        }
        else if (targetSite_->parent() != targetSpecies_)
            targetSpecies_ = targetSite_->parent();
    }

    // Copy basic atom and bond information from species
    if (targetSpecies_)
    {
        for (const auto &i : targetSpecies_->atoms())
            averageSpecies_.addAtom(i.Z(), i.r());
        for (const auto &bond : targetSpecies_->bonds())
            averageSpecies_.addBond(bond.indexI(), bond.indexJ());
    }

    // Set name and object tag for average species
    averageSpecies_.setName(
        fmt::format("{}@{}", targetSite_ ? targetSite_->name() : "???", targetSpecies_ ? targetSpecies_->name() : "???"));

    // Realise arrays
    updateArrays(moduleContext.dissolve().processingModuleData());

    // Update the species coordinates
    updateSpecies(moduleContext.dissolve().processingModuleData());

    return true;
}

// Run main processing
Module::ExecutionResult AvgMolModule::process(ModuleContext &moduleContext)
{
    // Grab Box pointer
    const auto *box = targetConfiguration_->box();

    // Get the target site
    if (!targetSite_)
    {
        Messenger::error("No target site defined.\n");
        return ExecutionResult::Failed;
    }

    // Get site parent species
    auto *sp = targetSite_->parent();
    if (sp != targetSpecies_)
    {
        Messenger::error("Internal error - target site parent is not the same as the target species.\n");
        return ExecutionResult::Failed;
    }

    Messenger::print("AvgMol: Target site (species) is {} ({}).\n", targetSite_->name(), targetSpecies_->name());
    if (exportFileAndFormat_.hasFilename())
        Messenger::print("AvgMol: Coordinates will be exported to '{}' ({}).\n", exportFileAndFormat_.filename(),
                         exportFileAndFormat_.formatDescription());

    Messenger::print("\n");

    // Update arrays
    updateArrays(moduleContext.dissolve().processingModuleData());

    // Get the site stack
    const auto *stack = targetConfiguration_->siteStack(targetSite_);

    // Retrieve data arrays
    auto &sampledX = moduleContext.dissolve().processingModuleData().retrieve<SampledVector>("X", name());
    auto &sampledY = moduleContext.dissolve().processingModuleData().retrieve<SampledVector>("Y", name());
    auto &sampledZ = moduleContext.dissolve().processingModuleData().retrieve<SampledVector>("Z", name());

    // Loop over sites
    std::vector<double> rx(targetSpecies_->nAtoms()), ry(targetSpecies_->nAtoms()), rz(targetSpecies_->nAtoms());
    Vec3<double> r;
    for (auto n = 0; n < stack->nSites(); ++n)
    {
        const auto &s = stack->site(n);
        assert(s.molecule()->species() == targetSpecies_);

        // Get axes and take inverse
        auto inverseAxes = s.axes();
        inverseAxes.invert();

        // Loop over atoms, taking delta position with origin, and rotating into local axes
        for (auto &&[i, x, y, z] : zip(s.molecule()->atoms(), rx, ry, rz))
        {
            r = inverseAxes * box->minimumVector(s.origin(), i->r());
            x = r.x;
            y = r.y;
            z = r.z;
        }

        // Accumulate positions
        sampledX += rx;
        sampledY += ry;
        sampledZ += rz;
    }

    updateSpecies(sampledX, sampledY, sampledZ);

    // Export data?
    if (exportFileAndFormat_.hasFilename())
    {
        if (!exportFileAndFormat_.exportData(&averageSpecies_))
            return ExecutionResult::Failed;
    }

    return ExecutionResult::Success;
}
