// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/context.h"
#include "main/dissolve.h"
#include <stdexcept>

// Set reference to Dissolve
void ProcedureContext::set(Dissolve &dissolve)
{
    dissolve_ = dissolve;
    processPool_ = dissolve.worldPool();
}

// Set available ProcessPool
void ProcedureContext::set(const ProcessPool &procPool) { processPool_ = procPool; }

// Set prefix for generated processing data
void ProcedureContext::set(std::string_view prefix) { processingDataPrefix_ = prefix; }

// Set target Configuration
void ProcedureContext::set(Configuration *cfg) { configuration_ = cfg; }

// Return reference to Dissolve
Dissolve &ProcedureContext::dissolve() const
{
    if (!dissolve_)
        throw(std::runtime_error("No reference to Dissolve is set in this context.\n"));
    return dissolve_->get();
}

// Return target Configuration
Configuration *ProcedureContext::configuration() const
{
    if (!configuration_)
        throw(std::runtime_error("No configuration set in this context.\n"));
    return configuration_;
}

// Return available process pool
const ProcessPool &ProcedureContext::processPool() const
{
    if (!processPool_)
        throw(std::runtime_error("No reference to a ProcessPool is set in this context.\n"));
    return processPool_->get();
};

// Return prefix to use for processing data
std::string_view ProcedureContext::processingModuleDataPrefix() const { return processingDataPrefix_; }

// Return processing data list
GenericList &ProcedureContext::processingModuleData() const
{
    if (!dissolve_)
        throw(std::runtime_error("No reference to Dissolve is set in this context, so cannot return processingModuleData.\n"));
    return dissolve_->get().processingModuleData();
}

// Return potential map
const PotentialMap &ProcedureContext::potentialMap() const
{
    if (!dissolve_)
        throw(std::runtime_error("No reference to Dissolve is set in this context, so cannot return potentialMap.\n"));
    return dissolve_->get().potentialMap();
}
