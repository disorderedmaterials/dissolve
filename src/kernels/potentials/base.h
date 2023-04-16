// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "keywords/store.h"

// Forward Declarations
class Atom;
class Box;
class LineParser;

// Extended Potential Base Class
class ExternalPotential
{
    public:
    AdditionalPotential();
    ~AdditionalPotential() = default;

    /*
     * Keywords
     */
    protected:
    // Keywords for the potential
    KeywordStore keywords_;

    public:
    // Return keywords for this potential
    KeywordStore &keywords();
    const KeywordStore &keywords() const;

    /*
     * Potential Calculation
     */
    public:
    // Calculate energy on specified atom
    virtual double energy(const Atom &i, const Box *box) const;
    // Calculate force on specified atom, summing in to supplied vector
    virtual void force(const Atom &i, const Box *box, Vec3<double> &f) const;

    /*
     * Read / Write
     */
    public:
    // Read data from specified LineParser
    bool deserialise(LineParser &parser, const CoreData &coreData);
    // Write data to specified LineParser
    bool serialise(LineParser &parser, std::string_view prefix) const;
};
