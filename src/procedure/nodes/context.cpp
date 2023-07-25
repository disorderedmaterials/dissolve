// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/context.h"
#include <stdexcept>
#include "main/dissolve.h"

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

// Set prefix
void ProcedureContext::setPrefix(std::string_view prefix) { dataPrefix_ = prefix; }

// Return prefix for generated data
std::string_view ProcedureContext::dataPrefix() const { return dataPrefix_; }

// Return reference to Dissolve
Dissolve& ProcedureContext::dissolve() const
{
    if (!dissolve_)
        throw(std::runtime_error("No reference to Dissolve is set in this procedure's context.\n"));
    return dissolve_->get();
}

GenericList &ProcedureContext::dataList() const
{
    if (!dissolve_)
        throw(std::runtime_error("No reference to Dissolve is set in this procedure's context.\n"));
    return dissolve_->get().processingModuleData();
}

const PotentialMap &ProcedureContext::potentialMap() const
{
    if (!dissolve_)
        throw(std::runtime_error("No reference to Dissolve is set in this procedure's context.\n"));
    return dissolve_->get().potentialMap();
}