// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "kernels/energy.h"
#include "kernels/force.h"
#include <memory>

// Forward Declarations
class Configuration;
class PotentialMap;
class ProcessPool;

// Kernel Factory
class KernelProducer
{
    public:
    // Create energy kernel for specified configuration
    static std::unique_ptr<EnergyKernel> energyKernel(const Configuration *cfg, const ProcessPool &procPool,
                                                      const PotentialMap &potentialMap,
                                                      std::optional<double> energyCutoff = {});
    // Create force kernel for specified configuration
    static std::unique_ptr<ForceKernel> forceKernel(const Configuration *cfg, const ProcessPool &procPool,
                                                    const PotentialMap &potentialMap, std::optional<double> energyCutoff = {});
    // Create force kernel using the specified Box
    static std::unique_ptr<ForceKernel> forceKernel(const Box *box, const ProcessPool &procPool,
                                                    const PotentialMap &potentialMap, std::optional<double> energyCutoff = {});
};
