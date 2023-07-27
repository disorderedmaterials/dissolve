// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "module/context.h"

ModuleContext::ModuleContext(const ProcessPool &procPool) : processPool_(procPool);

const ProcessPool &ModuleContext::processPool() const { return processPool_; }
