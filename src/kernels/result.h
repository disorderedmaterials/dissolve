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
        return {.bondEnergy = this->bondEnergy + other.bondEnergy,
                .angleEnergy = this->angleEnergy + other.angleEnergy,
                .torsionEnergy = this->torsionEnergy + other.torsionEnergy,
                .improperEnergy = this->improperEnergy + other.improperEnergy};
    }
    // Return total
    double total() const { return bondEnergy + angleEnergy + torsionEnergy + improperEnergy; }
};

// PairPotential Energy Value
class PairPotentialEnergyValue
{
    public:
    PairPotentialEnergyValue(double inter = 0.0, double intra = 0.0);
    PairPotentialEnergyValue operator+(const PairPotentialEnergyValue &value) const;
    PairPotentialEnergyValue operator-(const PairPotentialEnergyValue &value) const;
    PairPotentialEnergyValue &operator+=(const PairPotentialEnergyValue &value);
    PairPotentialEnergyValue &operator*=(const double scale);

    private:
    // Energy values
    double interMolecular_{0.0}, intraMolecular_{0.0};

    public:
    // Increment Energies
    void addInterMolecular(double e);
    void addIntraMolecular(double e);
    // Return Energies
    double interMolecular() const;
    double intraMolecular() const;
    double total() const;
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
