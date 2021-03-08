// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/export_pairpotentials/exportpp.h"

ExportPairPotentialsModule::ExportPairPotentialsModule() : Module(nRequiredTargets())
{
    // Initialise Module - set up keywords etc.
    initialise();
}

/*
 * Instances
 */

// Create instance of this module
Module *ExportPairPotentialsModule::createInstance() const { return new ExportPairPotentialsModule; }
