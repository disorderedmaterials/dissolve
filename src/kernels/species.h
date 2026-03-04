// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "kernels/geometry.h"
#include "kernels/result.h"
#include "templates/flags.h"
#include <memory>
#include <optional>

// Forward Declarations
class Atom;
class Cell;
class Species;
class SpeciesAtom;

// Species Kernel
class SpeciesKernel : public GeometryKernel
{
    private:
    friend class KernelProducer;
    SpeciesKernel(const Species *sp, const PotentialMap &potentialMap);

    public:
    ~SpeciesKernel() = default;

    /*
     * Totals
     */
    private:
    // Return pair potential energy of Species
    Kernel::PairPotentialEnergyValue pairPotentialEnergy(const Species *sp);
    // Return geometric energy of Species
    Kernel::GeometryEnergyValue geometryEnergy(const Species *sp);

    public:
    // Return total energy (interatomic and intramolecular) of Species
    Kernel::EnergyResult totalEnergy(const Species *sp);
};
