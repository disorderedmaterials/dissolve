// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/potentialMap.h"
#include "math/vector3.h"
#include "templates/optionalRef.h"
#include <memory>
#include <vector>

// Forward Declarations
class Box;

// Kernel Base
class KernelBase
{
    public:
    KernelBase(const Box *box, const PotentialMap &potentialMap);
    ~KernelBase() = default;

    protected:
    // Potential map to use
    const PotentialMap potentialMap_;
    // Squared cutoff distance to use in calculation
    double cutoffDistanceSquared_;
    // Periodic Box
    const Box *box_;
};
