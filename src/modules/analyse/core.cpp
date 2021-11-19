// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/configuration.h"
#include "keywords/procedure.h"
#include "modules/analyse/analyse.h"

AnalyseModule::AnalyseModule() : Module(), analyser_(ProcedureNode::AnalysisContext, "EndAnalyser")
{
    // Targets
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.addKeyword<ProcedureKeyword>("Analyser", "Analysis procedure to run", analyser_);
}
