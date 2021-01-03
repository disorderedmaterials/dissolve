// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/import/import.h"

ImportModule::ImportModule() : Module(nRequiredTargets())
{
    // Initialise Module - set up keywords etc.
    initialise();
}

ImportModule::~ImportModule() {}

/*
 * Instances
 */

// Create instance of this module
Module *ImportModule::createInstance() const { return new ImportModule; }
