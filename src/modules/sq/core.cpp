// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/sq/sq.h"

/*
 * Constructor / Destructor
 */

SQModule::SQModule() : Module(nRequiredTargets())
{
    // Initialise Module - set up keywords etc.
    initialise();
}

SQModule::~SQModule() {}

/*
 * Instances
 */

// Create instance of this module
Module *SQModule::createInstance() const { return new SQModule; }
