// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "procedure/nodes/context.h"
#include <exception>

ProcedureContext::ProcedureContext(ProcessPool &procPool, Configuration *cfg) : processPool_(procPool), configuration_(cfg) {}

// Return available process pool
ProcessPool &ProcedureContext::processPool() const { return processPool_; };

// Set target Configuration
void ProcedureContext::setConfiguration(Configuration *cfg) { configuration_ = cfg; }

// Return target Configuration
Configuration *ProcedureContext::configuration() const
{
    if (!configuration_)
        throw(std::runtime_error("No configuration set in this procedure's context.\n"));
    return configuration_;
}

// Set target data list and prefix
void ProcedureContext::setDataListAndPrefix(GenericList &list, std::string_view prefix)
{
    dataList_ = list;
    dataPrefix_ = prefix;
}

// Return prefix for generated data
std::string_view ProcedureContext::dataPrefix() const { return dataPrefix_; }

// Return target list for generated data
GenericList &ProcedureContext::dataList() const
{
    if (!dataList_)
        throw(std::runtime_error("No data list set in this procedure's context.\n"));
    return dataList_->get();
}
