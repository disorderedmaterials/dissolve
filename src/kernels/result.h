// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

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
    EnergyResult(PairPotentialEnergyValue pp = {}, double geom = 0.0, double ext = 0.0)
        : total_(pp.total() + geom + ext), geometry_(geom), extended_(ext), pairPotential_(pp)
    {
    }

    private:
    // Components
    double total_, geometry_, extended_;
    PairPotentialEnergyValue pairPotential_;

    public:
    double total() const { return total_; };
    PairPotentialEnergyValue pairPotential() const { return pairPotential_; }
    double geometry() const { return geometry_; }
    double extended() const { return extended_; }
    double totalUnbound() const { return pairPotential_.total() + extended_; }
};
