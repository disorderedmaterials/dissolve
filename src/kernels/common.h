// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "math/vector3.h"
#include "templates/combinable.h"

namespace Kernel
{
// Calculation flags
enum CalculationFlags
{
    ExcludeInterMolecularPairPotential,
    ExcludeIntraMolecularPairPotential,
    ExcludeGeometric,
    ExcludeExtended
};

// Geometry Energy Value
struct GeometryEnergyValue
{
    GeometryEnergyValue(double bond = 0.0, double angle = 0.0, double torsion = 0.0, double improper = 0.0)
        : bondEnergy(bond), angleEnergy(angle), torsionEnergy(torsion), improperEnergy(improper)
    {
    }

    // Energy values
    double bondEnergy{0.0};
    double angleEnergy{0.0};
    double torsionEnergy{0.0};
    double improperEnergy{0.0};

    GeometryEnergyValue operator+(const GeometryEnergyValue &other) const
    {
        return {bondEnergy + other.bondEnergy, angleEnergy + other.angleEnergy, torsionEnergy + other.torsionEnergy,
                improperEnergy + other.improperEnergy};
    }

    double total() const { return bondEnergy + angleEnergy + torsionEnergy + improperEnergy; }
};

// PairPotential Energy Value
struct PairPotentialEnergyValue
{
    PairPotentialEnergyValue(double inter = 0.0, double intra = 0.0) : interMolecular(inter), intraMolecular(intra) {}

    // Energy values
    double interMolecular{0.0}, intraMolecular{0.0};

    PairPotentialEnergyValue operator+(const PairPotentialEnergyValue &value) const
    {
        return {interMolecular + value.interMolecular, intraMolecular + value.intraMolecular};
    }
    PairPotentialEnergyValue operator-(const PairPotentialEnergyValue &value) const
    {
        return {interMolecular - value.interMolecular, intraMolecular - value.intraMolecular};
    }
    PairPotentialEnergyValue &operator+=(const PairPotentialEnergyValue &value)
    {
        interMolecular += value.interMolecular;
        intraMolecular += value.intraMolecular;
        return *this;
    }
    PairPotentialEnergyValue &operator*=(const double scale)
    {
        interMolecular *= scale;
        intraMolecular *= scale;
        return *this;
    }

    double total() const { return interMolecular + intraMolecular; }
};

// Energy Result
struct EnergyResult
{
    EnergyResult(PairPotentialEnergyValue pp = {}, GeometryEnergyValue geom = {}, double ext = 0.0)
        : pairPotential(pp), geometry(geom), extended(ext)
    {
    }

    PairPotentialEnergyValue pairPotential;
    GeometryEnergyValue geometry;
    double extended;

    double total() const { return pairPotential.total() + geometry.total() + extended; };
    double totalUnbound() const { return pairPotential.total() + extended; }
};

// Create combinable Vector3 storage container
static dissolve::CombinableContainer<std::vector<Vector3>> createCombinableVector3(std::vector<Vector3> &parentForces)
{
    return {parentForces, [&]() { return std::vector<Vector3>(parentForces.size()); }};
}

}; // namespace Kernel