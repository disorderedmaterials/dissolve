// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "task/context.h"
#include <stdexcept>

TaskContext::TaskContext(const ProcessPool &procPool) : processPool_(procPool) {}
TaskContext::TaskContext(const ProcessPool &procPool, Dissolve &dissolve) : processPool_(procPool), dissolve_(dissolve) {}

const ProcessPool &TaskContext::processPool() { return processPool_; }

Dissolve& TaskContext::dissolve()
{
    if (!dissolve_)
        throw(std::runtime_error("Dissolve is not set in this module's context.\n"));
    return dissolve_->get();
}
