// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/tr/tr.h"
#include "keywords/atomTypeVector.h"
#include "keywords/bool.h"
#include "keywords/double.h"
#include "keywords/function1D.h"
#include "keywords/integer.h"
#include "keywords/module.h"
#include "keywords/optionalDouble.h"
#include "keywords/optionalInt.h"
#include "modules/neutronSQ/neutronSQ.h"

TRModule::TRModule() : Module(ModuleTypes::TR)
{
    keywords_.addTarget<ModuleKeyword<const NeutronSQModule>>(
        "SourceNeutronSQs", "Source NeutronSQ to calculate total T(r) from", sourceNeutronSQ_, ModuleTypes::NeutronSQ);
    // Deprecated
    static bool deprecatedBool_{false};
    keywords_.addDeprecated<BoolKeyword>(
        "UseHalfCellRange", "Whether to use the maximal RDF range possible that avoids periodic images", deprecatedBool_);
}
