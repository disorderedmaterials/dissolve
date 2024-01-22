// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "main/dissolve.h"
#include "math/gaussFit.h"
#include "math/poissonFit.h"
#include "module/context.h"
#include "modules/epsrManager/epsrManager.h"
#include "templates/algorithms.h"

// Make pair of strings for each atom type present
std::string EPSRManagerModule::pairKey(std::pair<std::shared_ptr<AtomType>, std::shared_ptr<AtomType>> atomTypes)
{
    if (atomTypes.first->name() < atomTypes.second->name())
        return fmt::format("{}-{}", atomTypes.first->name(), atomTypes.second->name());
    else
        return fmt::format("{}-{}", atomTypes.second->name(), atomTypes.first->name());
}