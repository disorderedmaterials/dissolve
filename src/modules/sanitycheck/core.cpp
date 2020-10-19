// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/sanitycheck/sanitycheck.h"

SanityCheckModule::SanityCheckModule() : Module(nRequiredTargets())
{
    // Initialise Module - set up keywords etc.
    initialise();
}

SanityCheckModule::~SanityCheckModule() {}

/*
 * Instances
 */

// Create instance of this module
Module *SanityCheckModule::createInstance() const { return new SanityCheckModule; }
