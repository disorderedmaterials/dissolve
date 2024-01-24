// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "main/dissolve.h"
#include "math/gaussFit.h"
#include "math/poissonFit.h"
#include "module/context.h"
#include "modules/epsrManager/epsrManager.h"
#include "templates/algorithms.h"

// Make pair of strings for each atom type present
std::string EPSRManagerModule::pairKey(std::shared_ptr<AtomType> at1, std::shared_ptr<AtomType> at2)
{
    if (at1->name() < at2->name())
        return fmt::format("{}-{}", at1->name(), at2->name());
    else
        return fmt::format("{}-{}", at2->name(), at1->name());
}
