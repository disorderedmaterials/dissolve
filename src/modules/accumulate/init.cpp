// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/accumulate/accumulate.h"

// Perform any necessary initialisation for the Module
void AccumulateModule::initialise()
{
    keywords_.add<ModuleVectorKeyword>("Control", "Target", "Module containing the target partial set data to accumulate",
                                       targetModule_, std::vector<std::string>{"NeutronSQ", "XRaySQ", "SQ", "RDF"}, 1);
    keywords_.add<EnumOptionsKeyword<AccumulateModule::TargetPartialSet>>(
        "Control", "Data", "Data type to accumulate", targetPartialSet_, AccumulateModule::targetPartialSet());
    keywords_.add<BoolKeyword>("Export", "Save", "Whether to save the accumulated partials to disk", save_);
}
