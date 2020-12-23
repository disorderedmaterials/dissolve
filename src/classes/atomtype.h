// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/parameters.h"
#include "data/elements.h"
#include "data/ff.h"

// AtomType Definition
class AtomType
{
    public:
    AtomType();
    ~AtomType();

    /*
     * Character
     */
    private:
    // Name
    std::string name_;
    // Associated Element
    Elements::Element *element_;

    public:
    // Set name of AtomType
    void setName(std::string_view name);
    // Return name of AtomType
    std::string_view name() const;
    // Set atomic element
    void setElement(Elements::Element *el);
    // Return atomic Element
    Elements::Element *element() const;

    /*
     * Interaction Parameters
     */
    private:
    // Short-range interaction type
    Forcefield::ShortRangeType shortRangeType_;
    // Interatomic interaction parameters (including charge)
    InteractionParameters parameters_;
    // Whether this AtomType is exchangeable
    bool exchangeable_;
    // Index of this type in the master type index
    int index_;

    public:
    // Set short-range interaction type
    void setShortRangeType(Forcefield::ShortRangeType srType);
    // Return short-range interaction type
    Forcefield::ShortRangeType shortRangeType() const;
    // Return interatomic interaction interaction parameters
    InteractionParameters &parameters();
    // Set index of this type in the master type index
    void setIndex(int id);
    // Return index of this type in the master type index
    int index() const;
};
