// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/atomshake/atomshake.h"

AtomShakeModule::AtomShakeModule() : Module(nRequiredTargets())
{
    // Initialise Module - set up keywords etc.
    initialise();
}

/*
 * Instances
 */

// Create instance of this module
Module *AtomShakeModule::createInstance() const { return new AtomShakeModule; }
