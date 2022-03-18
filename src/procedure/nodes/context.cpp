// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "procedure/nodes/context.h"

ProcedureContext::ProcedureContext(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList)
    : processPool_(procPool), configuration_(cfg), dataPrefix_(prefix), dataList_(targetList)
{
}

// Return available process pool
ProcessPool &ProcedureContext::processPool() const { return processPool_; };

// Return target Configuration
Configuration *ProcedureContext::configuration() const { return configuration_; }

// Return prefix for generated data
std::string_view ProcedureContext::dataPrefix() const { return dataPrefix_; }

// Return target list for generated data
GenericList &ProcedureContext::dataList() const { return dataList_; }
