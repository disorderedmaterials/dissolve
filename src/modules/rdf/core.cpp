// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/rdf/rdf.h"

/*
 * Constructor / Destructor
 */

RDFModule::RDFModule() : Module()
{
    // Initialise Module - set up keywords etc.
    initialise();
}

RDFModule::~RDFModule() {}

/*
 * Instances
 */

// Create instance of this module
Module *RDFModule::createInstance() const { return new RDFModule; }
