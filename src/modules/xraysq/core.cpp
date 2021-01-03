// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/xraysq/xraysq.h"

XRaySQModule::XRaySQModule() : Module(nRequiredTargets())
{
    // Initialise Module - set up keywords etc.
    initialise();
}

XRaySQModule::~XRaySQModule() {}

/*
 * Instances
 */

// Create instance of this module
Module *XRaySQModule::createInstance() const { return new XRaySQModule; }
