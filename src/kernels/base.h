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
class CellArray;
class Configuration;
class Molecule;

// Kernel Base
class KernelBase
{
    public:
    KernelBase(const Configuration *cfg, const PotentialMap &potentialMap);
    KernelBase(const Box *box, const PotentialMap &potentialMap);
    ~KernelBase() = default;

    // Alias for force storage vector
    using ForceVector = std::vector<Vector3>;

    protected:
    // Potential map to use
    const PotentialMap potentialMap_;
    // Squared cutoff distance to use in calculation
    double cutoffDistanceSquared_;
    // Periodic Box
    const Box *box_;
    // Periodic box CellArray (optional)
    OptionalReferenceWrapper<const CellArray> cellArray_;
    // Target molecule array (if available))
    OptionalReferenceWrapper<const std::vector<std::shared_ptr<Molecule>>> molecules_;
};
