// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/test/test.h"

// Run main processing
bool TestModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    /*
     * This is a XXX routine.
     * XXX
     */

    // Check for zero Configuration targets
    if (!targetConfiguration_)
        return Messenger::error("No configuration target set for module '{}'.\n", uniqueName());

    // Set up process pool - must do this to ensure we are using all available processes
    procPool.assignProcessesToGroups(targetConfiguration_->processPool());

    // MODULE CODE

    return false;
}
