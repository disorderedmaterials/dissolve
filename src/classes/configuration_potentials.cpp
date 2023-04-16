// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/configuration.h"
#include "kernels/potentials/base.h"

// Add global potential
void Configuration::addGlobalPotential(std::unique_ptr<ExternalPotential> potential)
{
    globalPotentials_.emplace_back(std::move(potential));
}

// Return vector of defined global potentials
const std::vector<std::unique_ptr<ExternalPotential>> &Configuration::globalPotentials() const { return globalPotentials_; }
