// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/analyse/analyse.h"
#include "keywords/configuration.h"
#include "keywords/procedure.h"

AnalyseModule::AnalyseModule() : Module(ModuleTypes::Analyse), analyser_(ProcedureNode::AnalysisContext, "Analyser")
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.addHidden<ProcedureKeyword>("Analyser", "Analysis procedure to run", analyser_);
}

// Return the analyser
Procedure &AnalyseModule::analyser() { return analyser_; }
