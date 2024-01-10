// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/epsrManager/epsrManager.h"
#include "keywords/configuration.h"
#include "keywords/procedure.h"

EPSRManagerModule::EPSRManagerModule() : Module(ModuleTypes::EPSRManager)
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);
}
