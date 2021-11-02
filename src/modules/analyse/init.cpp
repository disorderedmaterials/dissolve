// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/procedure.h"
#include "modules/analyse/analyse.h"

// Perform any necessary initialisation for the Module
void AnalyseModule::initialise() { keywords_.addKeyword<ProcedureKeyword>("Analyser", "Analysis procedure to run", analyser_); }
