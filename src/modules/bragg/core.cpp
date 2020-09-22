// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/bragg/bragg.h"

/*
 * Constructor / Destructor
 */

BraggModule::BraggModule() : Module()
{
    // Initialise Module - set up keywords etc.
    initialise();
}

BraggModule::~BraggModule() {}

/*
 * Instances
 */

// Create instance of this module
Module *BraggModule::createInstance() const { return new BraggModule; }
