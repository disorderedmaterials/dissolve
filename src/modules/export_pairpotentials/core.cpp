// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/export_pairpotentials/exportpp.h"

/*
 * Constructor / Destructor
 */

ExportPairPotentialsModule::ExportPairPotentialsModule() : Module()
{
    // Initialise Module - set up keywords etc.
    initialise();
}

ExportPairPotentialsModule::~ExportPairPotentialsModule() {}

/*
 * Instances
 */

// Create instance of this module
Module *ExportPairPotentialsModule::createInstance() const { return new ExportPairPotentialsModule; }
