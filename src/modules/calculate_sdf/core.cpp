// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/calculate_sdf/sdf.h"

/*
 * Constructor / Destructor
 */

CalculateSDFModule::CalculateSDFModule() : Module(), analyser_(ProcedureNode::AnalysisContext)
{
    // Set unique name for this instance of the Module
    static int instanceId = 0;
    uniqueName_ = fmt::format("{}{:02d}", type(), instanceId++);

    // Initialise Module - set up keywords etc.
    initialise();
}

CalculateSDFModule::~CalculateSDFModule() {}

/*
 * Instances
 */

// Create instance of this module
Module *CalculateSDFModule::createInstance() const { return new CalculateSDFModule; }
