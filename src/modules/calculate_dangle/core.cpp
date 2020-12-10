// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/calculate_dangle/dangle.h"

/*
 * Constructor / Destructor
 */

CalculateDAngleModule::CalculateDAngleModule() : Module(nRequiredTargets()), analyser_(ProcedureNode::AnalysisContext)
{
    // Set unique name for this instance of the Module
    static int instanceId = 0;
    uniqueName_ = fmt::format("{}{:02d}", type(), instanceId++);

    // Initialise Module - set up keywords etc.
    initialise();
}

CalculateDAngleModule::~CalculateDAngleModule() {}

/*
 * Instances
 */

// Create instance of this module
Module *CalculateDAngleModule::createInstance() const { return new CalculateDAngleModule; }
