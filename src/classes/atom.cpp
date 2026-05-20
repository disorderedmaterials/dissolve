// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/atom.h"

/*
 * Properties
 */

// Set basic properties
void Atom::set(Elements::Element Z, const Vector3 &r, double q)
{
    r_ = r;
    Z_ = Z;
    q_ = q;
}

// Set coordinates
void Atom::setR(const Vector3 &r) { r_ = r; }

// Return coordinates
const Vector3 &Atom::r() const { return r_; }

// Set atomic element
void Atom::setZ(Elements::Element z) { Z_ = z; }

// Return atomic element
Elements::Element Atom::Z() const { return Z_; }

// Return presence of atom
AtomConstants::Presence Atom::presence() const
{
    return Z_ == Elements::Phantom ? AtomConstants::Presence::Phantom : AtomConstants::Presence::Physical;
}

// Return whether the atom is of the presence specified
bool Atom::isPresence(AtomConstants::Presence presenceType) const
{
    return presenceType == AtomConstants::Presence::Any || presence() == presenceType;
}

// Set atomic charge
void Atom::setQ(double q) { q_ = q; }

// Return atomic charge
double Atom::q() const { return q_; }

// Return index (0->[N-1])
int Atom::index() const { return index_; };

// Set index
void Atom::setIndex(int index) { index_ = index; }

// Set index of associated atom type in parent object
void Atom::setAtomTypeIndex(int id) { atomTypeIndex_ = id; }

// Return associated atom type index
int Atom::atomTypeIndex() const { return atomTypeIndex_; }

/*
 * Coordinate Manipulation Operators
 */

void Atom::operator+=(const Vector3 &delta) { r_ += delta; }

void Atom::operator-=(const Vector3 &delta) { r_ -= delta; }

/*
 * Serialisation
 */

// Express as a serialisable value
void Atom::serialise(std::string tag, SerialisedValue &target) const
{
    target[tag] = {{"index", index_}, {"z", Z_}, {"r", r_}, {"q", q_}};
}

// Read values from a serialisable value
void Atom::deserialise(const SerialisedValue &node)
{
    index_ = toml::find<int>(node, "index");

    set(toml::find<Elements::Element>(node, "z"), toml::find<Vector3>(node, "r"), toml::find_or<double>(node, "q", 0));
}
