// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"

// Forward Declarations
class Atom;

// Bond
class Bond : public Serialisable<>
{
    public:
    Bond(Atom *i, Atom *j);
    virtual ~Bond() = default;

    private:
    Atom *i_{nullptr}, *j_{nullptr};

    public:
    // Return the involved atoms
    Atom *i() const;
    Atom *j() const;
    // Return the 'other' atom
    Atom *partner(const Atom *atom) const;
    // Return whether the bond's atoms match those provided
    bool isBetween(const Atom *i, const Atom *j) const;

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const override;
};
