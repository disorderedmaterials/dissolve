// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

// Geometry Energy Value
struct GeometryEnergyValue
{
    double bondEnergy{0.0};
    double angleEnergy{0.0};
    double torsionEnergy{0.0};
    double improperEnergy{0.0};

    GeometryEnergyValue operator+(const GeometryEnergyValue &other) const
    {
        return {bondEnergy + other.bondEnergy, angleEnergy + other.angleEnergy, torsionEnergy + other.torsionEnergy,
                improperEnergy + other.improperEnergy};
    }
    // Return total
    double total() const { return bondEnergy + angleEnergy + torsionEnergy + improperEnergy; }
};

// PairPotential Energy Value
struct PairPotentialEnergyValue
{
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

    // Return total
    double total() const { return interMolecular + intraMolecular; }
};

// Energy Result
class EnergyResult
{
    public:
    EnergyResult(PairPotentialEnergyValue pp = {}, GeometryEnergyValue geom = {}, double ext = 0.0)
        : total_(pp.total() + geom.total() + ext), geometry_(geom), extended_(ext), pairPotential_(pp)
    {
    }

    private:
    // Components
    double total_, extended_;
    PairPotentialEnergyValue pairPotential_;
    GeometryEnergyValue geometry_;

    public:
    double total() const { return total_; };
    PairPotentialEnergyValue pairPotential() const { return pairPotential_; }
    GeometryEnergyValue geometry() const { return geometry_; }
    double extended() const { return extended_; }
    double totalUnbound() const { return pairPotential_.total() + extended_; }
};
