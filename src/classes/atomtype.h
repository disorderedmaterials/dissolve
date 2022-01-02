// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "data/elements.h"
#include "data/ff/ff.h"

// AtomType Definition
class AtomType
{
    public:
    AtomType(Elements::Element Z = Elements::Unknown);
    ~AtomType() = default;

    /*
     * Character
     */
    private:
    // Name
    std::string name_{"XX"};
    // Associated chemical element
    Elements::Element Z_;

    public:
    // Set name of AtomType
    void setName(std::string_view name);
    // Return name of AtomType
    std::string_view name() const;
    // Set chemical element
    void setZ(Elements::Element Z);
    // Return chemical element
    Elements::Element Z() const;

    /*
     * Interaction Parameters
     */
    private:
    // Short-range interaction type
    Forcefield::ShortRangeType shortRangeType_{Forcefield::ShortRangeType::Undefined};
    // Vector of parameters for short-range potential
    std::vector<double> parameters_;
    // Atomic charge
    double charge_{0.0};
    // Index of this type in the master type index
    int index_{-1};

    public:
    // Set short-range interaction type
    void setShortRangeType(Forcefield::ShortRangeType srType);
    // Return short-range interaction type
    Forcefield::ShortRangeType shortRangeType() const;
    // Set short-range parameters vector
    void setShortRangeParameters(const std::vector<double> &parameters);
    // Set single short-range parameter
    void setShortRangeParameter(int index, double parameter);
    // Return short-range parameters vector
    const std::vector<double> &shortRangeParameters() const;
    // Set atomic charge
    void setCharge(double q);
    // Return atomic charge
    double charge() const;
    // Set index of this type in the master type index
    void setIndex(int id);
    // Return index of this type in the master type index
    int index() const;
    // Return whether our parameters are the same as those provided
    bool sameParametersAs(const AtomType *other, bool checkCharge = false);
};
