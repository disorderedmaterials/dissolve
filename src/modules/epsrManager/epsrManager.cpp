// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/epsrManager/epsrManager.h"
#include "keywords/moduleVector.h"
#include "keywords/procedure.h"

EPSRManagerModule::EPSRManagerModule() : Module(ModuleTypes::EPSRManager)
{
    keywords_.addTarget<ModuleVectorKeyword>("Target", "Target EPSR modules over which the manager will operate",
                                             target_, std::vector<ModuleTypes::ModuleType>{ModuleTypes::EPSR});
}
