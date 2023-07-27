// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "templates/optionalRef.h"

// Forward Declarations
class CoreData;
class ProcessPool;
class PotentialMap;


class ModuleContext
{
    public:
    explicit ModuleContext(const ProcessPool &procPool);

    private:
    // Available process pool
    const ProcessPool& processPool_;
    const CoreData& coreData_;
    const double& pairPotentialRange_;
    const double& pairPotentialDelta_;
    const PotentialMap& potentialMap_;

    public:
    // Return available process pool
    const ProcessPool &processPool() const;
    const CoreData& coreData() const;
    const double& pairPotentialRange() const;
    const double& pairPotentialDelta() const;
    const PotentialMap& potentialMap() const;
};