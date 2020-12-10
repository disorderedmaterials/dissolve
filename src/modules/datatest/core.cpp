// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/datatest/datatest.h"

DataTestModule::DataTestModule() : Module(nRequiredTargets())
{
    // Initialise Module - set up keywords etc.
    initialise();
}

DataTestModule::~DataTestModule() {}

/*
 * Instances
 */

// Create instance of this module
Module *DataTestModule::createInstance() const { return new DataTestModule; }
