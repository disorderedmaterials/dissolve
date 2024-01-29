// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/configuration.h"
#include "kernels/potentials/base.h"

// Add global potential
void Configuration::addGlobalPotential(std::unique_ptr<ExternalPotential> potential)
{
    globalPotentials_.emplace_back(std::move(potential));
}

// Return vector of defined global potentials
const std::vector<std::unique_ptr<ExternalPotential>> &Configuration::globalPotentials() const { return globalPotentials_; }

// Add targeted potential
void Configuration::addTargetedPotential(std::unique_ptr<ExternalPotential> potential)
{
    targetedPotentials_.emplace_back(std::move(potential));
}

// Return vector of defined targeted potentials
const std::vector<std::unique_ptr<ExternalPotential>> &Configuration::targetedPotentials() const { return targetedPotentials_; }

// Link targeted potentials to atoms
void Configuration::linkTargetedPotentials()
{
    // Clear any existing targeted potentials from atoms
    std::for_each(atoms_.begin(), atoms_.end(), [](auto &atom) { atom.clearTargetedPotentials(); });

    for (const auto &pot : targetedPotentials_)
    {
        // Individual atoms
        for (auto i : pot->targetAtomIndices())
            atoms_[i].addTargetedPotential(pot.get());

        // Atom types
        for (const auto &at : pot->targetAtomTypes())
            for (auto &atom : atoms_)
                if (atom.speciesAtom()->atomType() == at)
                    atom.addTargetedPotential(pot.get());

        // Species
        for (const auto &sp : pot->targetSpecies())
        {
            for (auto &mol : molecules_)
                if (mol->species() == sp)
                    std::for_each(mol->atoms().begin(), mol->atoms().end(),
                                  [&pot](auto &atom) { atom.addTargetedPotential(pot.get()); });
        }
    }
}
