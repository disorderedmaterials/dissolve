// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/avgmol/avgmol.h"
#include "nodes/constants.h"

void AvgMolNode::updateArrays()
{
    auto requiredSize = targetSpecies_ ? targetSpecies_->nAtoms() : -1;

    if (requiredSize > 0)
    {
        if (x_.values().size() == requiredSize && y_.values().size() == requiredSize && z_.values().size() == requiredSize)
            Messenger::print("Using existing coordinate arrays for average species.\n");
        else
        {
            Messenger::print("Initialising arrays for average molecule: size = {}\n", requiredSize);
            x_.initialise(requiredSize);
            y_.initialise(requiredSize);
            z_.initialise(requiredSize);
        }
    }
    else
    {
        x_.clear();
        y_.clear();
        z_.clear();
    }
}

void AvgMolNode::updateSpecies()
{
    for (auto &&[i, rx, ry, rz] : zip(averageSpecies_.atoms(), x_.values(), y_.values(), z_.values()))
        averageSpecies_.setAtomCoordinates(&i, {rx, ry, rz});
}

NodeConstants::ProcessResult AvgMolNode::process()
{
    // Grab Box pointer
    const auto *box = targetConfiguration_->box();

    // Get the target site
    if (!targetSite_)
    {
        Messenger::error("No target site defined.\n");
        return NodeConstants::ProcessResult::Failed;
    }

    // Get site parent species
    targetSpecies_ = targetSite_->parent();

    Messenger::print("AvgMol: Target site (species) is {} ({}).\n", targetSite_->name(), targetSpecies_->name());
    if (exportFileAndFormat_.hasFilename())
        Messenger::print("AvgMol: Coordinates will be exported to '{}' ({}).\n", exportFileAndFormat_.filename(),
                         exportFileAndFormat_.formatDescription());

    Messenger::print("\n");

    // Update arrays
    updateArrays();

    // Get the site stack
    const auto *stack = targetConfiguration_->siteStack(targetSite_);

    // Loop over sites
    std::vector<double> rx(targetSpecies_->nAtoms()), ry(targetSpecies_->nAtoms()), rz(targetSpecies_->nAtoms());
    Vector3 r;
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
        x_ += rx;
        y_ += ry;
        z_ += rz;
    }

    updateSpecies();

    // Export data?
    if (exportFileAndFormat_.hasFilename())
    {
        if (!exportFileAndFormat_.exportData(&averageSpecies_))
            return NodeConstants::ProcessResult::Failed;
    }

    return NodeConstants::ProcessResult::Success;
}
