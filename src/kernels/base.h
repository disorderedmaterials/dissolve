// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "templates/optionalRef.h"
#include "templates/vector3.h"
#include <memory>
#include <vector>

// Forward Declarations
class Box;
class CellArray;
class Configuration;
class Molecule;
class PotentialMap;
class ProcessPool;

// Kernel Base
class KernelBase
{
    public:
    KernelBase(const Configuration *cfg, const ProcessPool &procPool, const PotentialMap &potentialMap,
               std::optional<double> energyCutoff = {});
    KernelBase(const Box *box, const ProcessPool &procPool, const PotentialMap &potentialMap,
               std::optional<double> energyCutoff = {});
    ~KernelBase() = default;

    // Alias for force storage vector
    using ForceVector = std::vector<Vec3<double>>;

    protected:
    // Process pool over which this kernel operates
    const ProcessPool &processPool_;
    // Potential map to use
    const PotentialMap &potentialMap_;
    // Squared cutoff distance to use in calculation
    double cutoffDistanceSquared_;
    // Periodic Box
    const Box *box_;
    // Periodic box CellArray (optional)
    OptionalReferenceWrapper<const CellArray> cellArray_;
    // Target molecule array (if available))
    OptionalReferenceWrapper<const std::vector<std::shared_ptr<Molecule>>> molecules_;
};
