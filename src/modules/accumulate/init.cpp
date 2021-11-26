// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/accumulate/accumulate.h"

// Perform any necessary initialisation for the Module
void AccumulateModule::initialise()
{
    // Targets
    keywords_.addTarget<ModuleVectorKeyword>("Target", "Module containing the target partial set data to accumulate",
                                             targetModule_, std::vector<std::string>{"NeutronSQ", "XRaySQ", "SQ", "RDF"}, 1);
    keywords_.addTarget<EnumOptionsKeyword<AccumulateModule::TargetPartialSet>>(
        "Data", "Data type to accumulate", targetPartialSet_, AccumulateModule::targetPartialSet());

    // Export
    keywords_.add<BoolKeyword>("Export", "Save", "Whether to save the accumulated partials to disk", save_);
}
