// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/box.h"
#include "main/dissolve.h"
#include "modules/calculate_avgmol/avgmol.h"
#include "templates/algorithms.h"

// Run set-up stage
bool CalculateAvgMolModule::setUp(Dissolve &dissolve, ProcessPool &procPool)
{
    auto *site = keywords_.retrieve<SpeciesSite *>("Site");

    // Clear species
    averageSpecies_.clear();

    // If the targetSpecies_ is different from the current target site, or the site is nullptr, clear the arrays
    if (!site)
        targetSpecies_ = nullptr;
    else
    {
        if (site->parent() == nullptr)
        {
            targetSpecies_ = nullptr;

            return Messenger::error("Target site has no parent species.\n");
        }
        else if (site->parent() != targetSpecies_)
        {
            targetSpecies_ = site->parent();

            // Copy basic atom and bond information from species
            for (const auto &i : targetSpecies_->atoms())
                averageSpecies_.addAtom(i.Z(), i.r());
            for (const auto &bond : targetSpecies_->bonds())
                averageSpecies_.addBond(bond.indexI(), bond.indexJ());
        }
    }

    // Set name and object tag for average species
    averageSpecies_.setName(fmt::format("{}@{}", site ? site->name() : "???", targetSpecies_ ? targetSpecies_->name() : "???"));

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
    if (targetConfigurationsKeyword_.data().empty())
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Grab Configuration and Box pointers
    auto *cfg = targetConfigurationsKeyword_.data().front();
    const auto *box = cfg->box();

    // Set up process pool - must do this to ensure we are using all available processes
    procPool.assignProcessesToGroups(cfg->processPool());

    // Get the target site
    auto *site = keywords_.retrieve<SpeciesSite *>("Site");
    if (!site)
        return Messenger::error("No target site defined.\n");

    // Get site parent species
    auto *sp = site->parent();
    if (sp != targetSpecies_)
        return Messenger::error("Internal error - target site parent is not the same as the target species.\n");

    // Update arrays
    updateArrays(dissolve);

    // Get the site stack
    const auto *stack = cfg->siteStack(site);

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
