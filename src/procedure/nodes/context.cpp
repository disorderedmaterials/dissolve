// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/context.h"
#include "main/dissolve.h"
#include <stdexcept>

ProcedureContext::ProcedureContext(const ProcessPool &procPool) : processPool_(procPool) {}

ProcedureContext::ProcedureContext(const ProcessPool &procPool, Configuration *cfg)
    : processPool_(procPool), configuration_(cfg)
{
}

ProcedureContext::ProcedureContext(const ProcessPool &procPool, Dissolve &dissolve)
    : processPool_(procPool), dissolve_(dissolve)
{
}

// Return available process pool
const ProcessPool &ProcedureContext::processPool() const { return processPool_; };

// Set target Configuration
void ProcedureContext::setConfiguration(Configuration *cfg) { configuration_ = cfg; }

// Return target Configuration
Configuration *ProcedureContext::configuration() const
{
    if (!configuration_)
        throw(std::runtime_error("No configuration set in this procedure's context.\n"));
    return configuration_;
}

// Set reference to Dissolve
void ProcedureContext::setDissolve(Dissolve &dissolve) { dissolve_ = dissolve; }

// Return reference to Dissolve
Dissolve &ProcedureContext::dissolve() const
{
    if (!dissolve_)
        throw(std::runtime_error("No reference to Dissolve is set in this procedure's context.\n"));
    return dissolve_->get();
}

// Set target data list and prefix
void ProcedureContext::setProcessingDataPrefix(std::string_view prefix) { processingDataPrefix_ = prefix; }

// Return prefix for generated data
std::string_view ProcedureContext::processingDataPrefix() const { return processingDataPrefix_; }

// Return target list for generated data
GenericList &ProcedureContext::dataList() const
{
    if (!dissolve_)
        throw(std::runtime_error(
            "No reference to Dissolve is set in this procedure's context, so cannot return processingModuleData.\n"));
    return dissolve_->get().processingModuleData();
}

// Return potential map
const PotentialMap &ProcedureContext::potentialMap() const
{
    if (!dissolve_)
        throw(std::runtime_error(
            "No reference to Dissolve is set in this procedure's context, so cannot return potentialMap.\n"));
    return dissolve_->get().potentialMap();
}