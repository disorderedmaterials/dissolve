// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/drivenMD/drivenMD.h"
#include "keywords/configuration.h"
#include "keywords/fileAndFormat.h"
#include "keywords/moduleVector.h"
#include "keywords/speciesSiteVector.h"
#include "keywords/weightedModuleVector.h"

DrivenMDModule::DrivenMDModule() : Module(ModuleTypes::DrivenMD)
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_)
        ->setEditSignals({KeywordBase::ClearModuleData, KeywordBase::RecreateRenderables});
    keywords_.addTarget<ModuleVectorKeyword>("Target", "Add specified Module (and it's Reference data) as a refinement target",
                                             targets_,
                                             std::vector<ModuleTypes::ModuleType>{ModuleTypes::NeutronSQ, ModuleTypes::XRaySQ});
}
