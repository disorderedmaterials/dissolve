// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/tr/tr.h"
#include "keywords/module.h"
#include "modules/neutronSQ/neutronSQ.h"
#include "keywords/bool.h"

TRModule::TRModule() : Module(ModuleTypes::TR)
{
    keywords_.addTarget<ModuleKeyword<const NeutronSQModule>>(
        "SourceNeutronSQs", "Source NeutronSQ to calculate total T(r) from", sourceNeutronSQ_, ModuleTypes::NeutronSQ);
    keywords_.setOrganisation("Export");
    keywords_.add<BoolKeyword>("SaveTR", "Save weighted g(r) and G(r)", saveTR_);
}
