// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"

// Forward Declarations
class AtomBase;

// Bond
template <class AtomClass> class Bond : public Serialisable
{
    public:
    Bond(AtomClass *i = nullptr, AtomClass *j = nullptr) : i_(i), j_(j) {}
    virtual ~Bond() = default;

    protected:
    AtomClass *i_{nullptr}, *j_{nullptr};

    public:
    // Return the involved atoms
    AtomClass *i() const { return i_; }
    AtomClass *j() const { return j_; }
    // Return the 'other' atom
    AtomClass *partner(const AtomBase *atom) const { return (atom == i_ ? j_ : i_); }
    // Return whether the bond's atoms match those provided
    bool isBetween(const AtomBase *i, const AtomBase *j) const { return (i == i_ && j == j_) || (i == j_ && j == i_); }

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const override
    {
        target[tag] = {{"i", i_->index()}, {"j", j_->index()}};
    }
};
