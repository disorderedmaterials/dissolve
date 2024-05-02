// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atomType.h"
#include "modules/epsrManager/epsrManager.h"

// Return key for supplied atom type pair
std::string EPSRManagerModule::pairKey(const std::shared_ptr<AtomType> &at1, const std::shared_ptr<AtomType> &at2)
{
    if (at1->name() < at2->name())
        return fmt::format("{}-{}", at1->name(), at2->name());
    else
        return fmt::format("{}-{}", at2->name(), at1->name());
}
