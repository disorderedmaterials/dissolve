// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/forces/forces.h"

ForcesModule::ForcesModule() : Module(nRequiredTargets())
{
    // Initialise Module - set up keywords etc.
    initialise();
}

/*
 * Instances
 */

// Create instance of this module
Module *ForcesModule::createInstance() const { return new ForcesModule; }
