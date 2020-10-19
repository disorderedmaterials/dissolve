// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/refine/refine.h"

/*
 * Constructor / Destructor
 */

RefineModule::RefineModule() : Module(nRequiredTargets()), interpolatedFitData_(fitData_)
{
    // Initialise Module - set up keywords etc.
    initialise();
}

RefineModule::~RefineModule() {}

/*
 * Instances
 */

// Create instance of this module
Module *RefineModule::createInstance() const { return new RefineModule; }
