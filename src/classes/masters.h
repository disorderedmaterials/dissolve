// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "classes/speciesangle.h"
#include "classes/speciesbond.h"
#include "classes/speciesimproper.h"
#include "classes/speciestorsion.h"

class Masters : public Serialisable
{
    public:
    Masters() = default;
    // Master Bond parameters for Species
    std::vector<std::shared_ptr<MasterBond>> masterBonds;
    // Master Angles parameters for Species
    std::vector<std::shared_ptr<MasterAngle>> masterAngles;
    // Master Torsions parameters for Species
    std::vector<std::shared_ptr<MasterTorsion>> masterTorsions;
    // Master Improper parameters for Species
    std::vector<std::shared_ptr<MasterImproper>> masterImpropers;
    SerialisedValue serialise() const override { return {}; };
};
