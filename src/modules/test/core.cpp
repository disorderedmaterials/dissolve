// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/test/test.h"

/*
 * Constructor / Destructor
 */

TestModule::TestModule() : Module(nRequiredTargets())
{
    // Initialise Module - set up keywords etc.
    initialise();
}

TestModule::~TestModule() {}

/*
 * Instances
 */

// Create instance of this module
Module *TestModule::createInstance() const { return new TestModule; }
