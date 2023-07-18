// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "main/dissolve.h"
#include "modules/skeleton/skeleton.h"

// Run main processing
enum Module::executionResult SkeletonModule::process(Dissolve &dissolve, const ProcessPool &procPool)
{
    // Check for zero Configuration targets
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return failed;
    }

    // MODULE CODE

    return failed;
}
