// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "module/context.h"
#include <stdexcept>

ModuleContext::ModuleContext(const ProcessPool &procPool) : processPool_(procPool) {}
ModuleContext::ModuleContext(const ProcessPool &procPool, Dissolve &dissolve) : processPool_(procPool), dissolve_(dissolve) {}

const ProcessPool &ModuleContext::processPool() const { return processPool_; }

Dissolve &ModuleContext::dissolve()
{
    if (!dissolve_)
        throw(std::runtime_error("Dissolve is not set in this module's context.\n"));
    return dissolve_->get();
}

const Dissolve &ModuleContext::dissolve() const
{
    if (!dissolve_)
        throw(std::runtime_error("Dissolve is not set in this module's context.\n"));
    return dissolve_->get();
}