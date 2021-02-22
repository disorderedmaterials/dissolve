// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/accumulate/accumulate.h"

// Perform any necessary initialisation for the Module
void AccumulateModule::initialise()
{
    keywords_.add("Control", new ModuleVectorKeyword({"NeutronSQ", "XRaySQ", "SQ", "RDF"}, 1), "Target",
                  "Module containing the target partial set data to accumulate", "<ModuleName>");
    keywords_.add("Control", new EnumOptionsKeyword<AccumulateModule::TargetPartialSet>(AccumulateModule::targetPartialSet()),
                  "Data", "Data type to accumulate");
    keywords_.add("Export", new BoolKeyword(false), "Export", "Whether to save the accumulated partials to disk");
}
