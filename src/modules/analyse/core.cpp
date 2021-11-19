// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/analyse/analyse.h"

AnalyseModule::AnalyseModule() : Module(), analyser_(ProcedureNode::AnalysisContext, "EndAnalyser")
{
    // Initialise Module - set up keywords etc.
    initialise();
}
