// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/atomshake/atomshake.h"

/*
 * Constructor / Destructor
 */

AtomShakeModule::AtomShakeModule() : Module(nRequiredTargets())
{
    // Initialise Module - set up keywords etc.
    initialise();
}

AtomShakeModule::~AtomShakeModule() {}

/*
 * Instances
 */

// Create instance of this module
Module *AtomShakeModule::createInstance() const { return new AtomShakeModule; }
