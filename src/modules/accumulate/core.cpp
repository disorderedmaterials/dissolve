// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/bool.h"
#include "keywords/enumoptions.h"
#include "keywords/modulevector.h"
#include "modules/accumulate/accumulate.h"

AccumulateModule::AccumulateModule() : Module()
{
    // Targets
    keywords_.addTarget<ModuleVectorKeyword>("Target", "Module containing the target partial set data to accumulate",
                                             targetModule_, std::vector<std::string>{"NeutronSQ", "XRaySQ", "SQ", "RDF"}, 1);
    keywords_.addTarget<EnumOptionsKeyword<AccumulateModule::TargetPartialSet>>(
        "Data", "Data type to accumulate", targetPartialSet_, AccumulateModule::targetPartialSet());

    // Export
    keywords_.add<BoolKeyword>("Export", "Save", "Whether to save the accumulated partials to disk", save_);
}

// Return type of module
std::string_view AccumulateModule::type() const { return "Accumulate"; }
