// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/benchmark/benchmark.h"

BenchmarkModule::BenchmarkModule() : Module(nRequiredTargets())
{
    // Set unique name for this instance of the Module
    static int instanceId = 0;
    uniqueName_ = fmt::format("{}{:02d}", type(), instanceId++);

    // Initialise Module - set up keywords etc.
    initialise();
}

/*
 * Instances
 */

// Create instance of this module
Module *BenchmarkModule::createInstance() const { return new BenchmarkModule; }
