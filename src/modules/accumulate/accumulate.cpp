// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/accumulate/accumulate.h"
#include "keywords/bool.h"
#include "keywords/enumoptions.h"
#include "keywords/modulevector.h"

AccumulateModule::AccumulateModule() : Module(ModuleTypes::Accumulate)
{
    keywords_.addTarget<ModuleVectorKeyword>(
        "Target", "Module containing the target partial set data to accumulate", targetModule_,
        std::vector<ModuleTypes::ModuleType>{ModuleTypes::NeutronSQ, ModuleTypes::XRaySQ, ModuleTypes::SQ, ModuleTypes::GR}, 1);
    keywords_.addTarget<EnumOptionsKeyword<AccumulateModule::TargetPartialSet>>(
        "Data", "Data type to accumulate", targetPartialSet_, AccumulateModule::targetPartialSet());

    keywords_.setOrganisation("Export");
    keywords_.add<BoolKeyword>("Save", "Whether to save the accumulated partials to disk", save_);
}
