// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

// Forward Declarations
class Species;
class Isotopologue;
class LineParser;

// Isotopologue Weight
class IsotopologueWeight
{
    public:
    IsotopologueWeight(const Isotopologue *iso = nullptr, double weight = 1.0);
    ~IsotopologueWeight() = default;

    /*
     * Isotopologue and Weight
     */
    private:
    // Associated Isotopologue
    const Isotopologue *isotopologue_;
    // Relative weight
    double weight_;

    public:
    // Set associated Isotopologue and weight
    void set(const Isotopologue *iso, double weight);
    // Set associated Isotopologue
    void setIsotopologue(const Isotopologue *iso);
    // Return associated Isotopologue
    const Isotopologue *isotopologue() const;
    // Set relative weight
    void setWeight(double weight);
    // Add to relative weight
    void addWeight(double weight);
    // Return relative weight
    double weight() const;
};
