// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/checks/checks.h"

/*
 * Constructor / Destructor
 */

ChecksModule::ChecksModule() : Module(nRequiredTargets())
{
    // Initialise Module - set up keywords etc.
    initialise();
}

ChecksModule::~ChecksModule() {}

/*
 * Instances
 */

// Create instance of this module
Module *ChecksModule::createInstance() const { return new ChecksModule; }
