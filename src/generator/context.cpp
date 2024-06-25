// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/context.h"
#include "main/dissolve.h"
#include <stdexcept>

// Set reference to Dissolve
void GeneratorContext::set(Dissolve &dissolve)
{
    dissolve_ = dissolve;
    processPool_ = dissolve.worldPool();
}

// Set available ProcessPool
void GeneratorContext::set(const ProcessPool &procPool) { processPool_ = procPool; }

// Set target Configuration
void GeneratorContext::set(Configuration *cfg) { configuration_ = cfg; }

// Return target Configuration
Configuration *GeneratorContext::configuration() const
{
    if (!configuration_)
        throw(std::runtime_error("No configuration set in this context.\n"));
    return configuration_;
}

// Return available process pool
const ProcessPool &GeneratorContext::processPool() const
{
    if (!processPool_)
        throw(std::runtime_error("No reference to a ProcessPool is set in this context.\n"));
    return processPool_->get();
}

// Return potential map
const PotentialMap &GeneratorContext::potentialMap() const
{
    if (!dissolve_)
        throw(std::runtime_error("No reference to Dissolve is set in this context, so cannot return potentialMap.\n"));
    return dissolve_->get().potentialMap();
}
