// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/intrashake/intrashake.h"

/*
 * Constructor / Destructor
 */

IntraShakeModule::IntraShakeModule() : Module(nRequiredTargets())
{
    // Initialise Module - set up keywords etc.
    initialise();
}

IntraShakeModule::~IntraShakeModule() {}

/*
 * Instances
 */

// Create instance of this module
Module *IntraShakeModule::createInstance() const { return new IntraShakeModule; }
