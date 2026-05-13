// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/bond.h"
#include "classes/atom.h"

Bond::Bond(Atom *i, Atom *j) : i_(i), j_(j)
{
    i_->addBond(this);
    j_->addBond(this);
}

// Return the involved atoms
Atom *Bond::i() const { return i_; }
Atom *Bond::j() const { return j_; }

// Return the 'other' atom
Atom *Bond::partner(const Atom *atom) const { return (atom == i_ ? j_ : i_); }

// Return whether the bond's atoms match those provided
bool Bond::isBetween(const Atom *i, const Atom *j) const { return (i == i_ && j == j_) || (i == j_ && j == i_); }

/*
 * Serialisation
 */

// Express as a serialisable value
void Bond::serialise(std::string tag, SerialisedValue &target) const { target[tag] = {{"i", i_->index()}, {"j", j_->index()}}; }
