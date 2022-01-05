// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/box.h"
#include "main/dissolve.h"
#include "modules/calculate_avgmol/avgmol.h"
#include "templates/algorithms.h"

// Run set-up stage
bool CalculateAvgMolModule::setUp(Dissolve &dissolve, ProcessPool &procPool, KeywordSignals actionSignals)
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
        {
            targetSpecies_ = targetSite_->parent();

            // Copy basic atom and bond information from species
            for (const auto &i : targetSpecies_->atoms())
                averageSpecies_.addAtom(i.Z(), i.r());
            for (const auto &bond : targetSpecies_->bonds())
                averageSpecies_.addBond(bond.indexI(), bond.indexJ());
        }
    }

    // Set name and object tag for average species
    averageSpecies_.setName(
        fmt::format("{}@{}", targetSite_ ? targetSite_->name() : "???", targetSpecies_ ? targetSpecies_->name() : "???"));

    // Realise arrays
    updateArrays(dissolve);

    // Update the species coordinates
    updateSpecies(dissolve.processingModuleData());

    return true;
}

// Run main processing
bool CalculateAvgMolModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    // Check for zero Configuration targets
    if (!targetConfiguration_)
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Grab Box pointer
    const auto *box = targetConfiguration_->box();

    // Set up process pool - must do this to ensure we are using all available processes
    procPool.assignProcessesToGroups(targetConfiguration_->processPool());

    // Get the target site
    if (!targetSite_)
        return Messenger::error("No target site defined.\n");

    // Get site parent species
    auto *sp = targetSite_->parent();
    if (sp != targetSpecies_)
        return Messenger::error("Internal error - target site parent is not the same as the target species.\n");

    // Update arrays
    updateArrays(dissolve);

    // Get the site stack
    const auto *stack = targetConfiguration_->siteStack(targetSite_);

    // Retrieve data arrays
    auto &sampledX = dissolve.processingModuleData().retrieve<SampledVector>("X", uniqueName());
    auto &sampledY = dissolve.processingModuleData().retrieve<SampledVector>("Y", uniqueName());
    auto &sampledZ = dissolve.processingModuleData().retrieve<SampledVector>("Z", uniqueName());

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

    return true;
}
