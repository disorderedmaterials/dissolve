// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/geomopt/geomopt.h"

/*
 * Constructor / Destructor
 */

GeometryOptimisationModule::GeometryOptimisationModule() : Module()
{
    // Initialise Module - set up keywords etc.
    initialise();
}

GeometryOptimisationModule::~GeometryOptimisationModule() {}

/*
 * Instances
 */

// Create instance of this module
Module *GeometryOptimisationModule::createInstance() const { return new GeometryOptimisationModule; }
