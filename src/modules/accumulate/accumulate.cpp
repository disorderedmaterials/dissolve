// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/accumulate/accumulate.h"
#include "keywords/bool.h"
#include "keywords/enumOptions.h"
#include "keywords/moduleVector.h"

AccumulateModule::AccumulateModule() : Module(ModuleTypes::Accumulate)
{
    keywords_.addTarget<ModuleVectorKeyword>(
        "Targets", "Module(s) containing the target partial set data to accumulate", targetModules_,
        std::vector<ModuleTypes::ModuleType>{ModuleTypes::NeutronSQ, ModuleTypes::XRaySQ, ModuleTypes::SQ, ModuleTypes::GR}, 1);
    keywords_.addTarget<EnumOptionsKeyword<AccumulateModule::TargetPartialSet>>(
        "Data", "Data type to accumulate", targetPartialSet_, AccumulateModule::targetPartialSet());

    keywords_.setOrganisation("Export");
    keywords_.add<BoolKeyword>("Save", "Whether to save the accumulated partials to disk", save_);

    // Deprecated Keywords
    keywords_.addDeprecated<ModuleVectorKeyword>(
        "Target", "Module containing the target partial set data to accumulate", targetModules_,
        std::vector<ModuleTypes::ModuleType>{ModuleTypes::NeutronSQ, ModuleTypes::XRaySQ, ModuleTypes::SQ, ModuleTypes::GR}, 1);
}
