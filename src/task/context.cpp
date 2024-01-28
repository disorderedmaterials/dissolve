// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "task/context.h"
#include "main/dissolve.h"

// Set reference to Dissolve
void TaskContext::set(Dissolve &dissolve)
{
    dissolve_ = dissolve;
    processPool_ = dissolve.worldPool();
}

// Set available ProcessPool
void TaskContext::set(const ProcessPool &procPool) { processPool_ = procPool; }

// Return reference to Dissolve
Dissolve &TaskContext::dissolve() const
{
    if (!dissolve_)
        throw(std::runtime_error("No reference to Dissolve is set in this context.\n"));
    return dissolve_->get();
}

// Return available process pool
const ProcessPool &TaskContext::processPool() const
{
    if (!processPool_)
        throw(std::runtime_error("No reference to a ProcessPool is set in this context.\n"));
    return processPool_->get();
};
