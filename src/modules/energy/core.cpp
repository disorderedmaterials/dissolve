// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/energy/energy.h"

/*
 * Constructor / Destructor
 */

EnergyModule::EnergyModule() : Module(nRequiredTargets())
{
    // Initialise Module - set up keywords etc.
    initialise();
}

EnergyModule::~EnergyModule() {}

/*
 * Instances
 */

// Create instance of this module
Module *EnergyModule::createInstance() const { return new EnergyModule; }
