// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/import/import.h"

/*
 * Constructor / Destructor
 */

ImportModule::ImportModule() : Module()
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
