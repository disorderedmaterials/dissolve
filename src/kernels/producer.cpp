// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "kernels/producer.h"
#include "classes/configuration.h"
#include "kernels/energy.h"
#include "kernels/externalPotentials.h"
#include "kernels/force.h"

// Create energy kernel for specified configuration
std::unique_ptr<EnergyKernel> KernelProducer::energyKernel(const Configuration *cfg, const PotentialMap &potentialMap,
                                                           std::optional<double> energyCutoff)
{
    if (!cfg->globalPotentials().empty() || !cfg->targetedPotentials().empty())
        return std::unique_ptr<EnergyKernel>(new ExternalPotentialsEnergyKernel(cfg, potentialMap, energyCutoff));
    else
        return std::unique_ptr<EnergyKernel>(new EnergyKernel(cfg, potentialMap, energyCutoff));
}

// Create force kernel for specified configuration
std::unique_ptr<ForceKernel> KernelProducer::forceKernel(const Configuration *cfg, const PotentialMap &potentialMap,
                                                         std::optional<double> energyCutoff)
{
    if (!cfg->globalPotentials().empty() || !cfg->targetedPotentials().empty())
        return std::unique_ptr<ForceKernel>(new ExternalPotentialsForceKernel(cfg, potentialMap, energyCutoff));
    else
        return std::unique_ptr<ForceKernel>(new ForceKernel(cfg, potentialMap, energyCutoff));
}

// Create force kernel using the specified Box
std::unique_ptr<ForceKernel> KernelProducer::forceKernel(const Box *box, const PotentialMap &potentialMap,
                                                         std::optional<double> energyCutoff)
{
    return std::unique_ptr<ForceKernel>(new ForceKernel(box, potentialMap, energyCutoff));
}
