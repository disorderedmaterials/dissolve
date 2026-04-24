// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/atom.h"

// Set basic properties
void Atom::set(Elements::Element Z, const Vector3 &r, double q)
{
    r_ = r;
    Z_ = Z;
    q_ = q;
}

// Return coordinates
Vector3 &Atom::r() { return r_; }
const Vector3 &Atom::r() const { return r_; }

// Return atomic element
Elements::Element &Atom::Z() { return Z_; }
Elements::Element Atom::Z() const { return Z_; }

// Return atomic charge
double &Atom::q() { return q_; };
double Atom::q() const { return q_; }

// Return index (0->[N-1])
int Atom::index() const { return index_; };

// Set index
void Atom::setIndex(int index) { index_ = index; }

// Set index of associated atom type in parent object
void Atom::setAtomTypeIndex(int id) { atomTypeIndex_ = id; }

// Return associated atom type index
int Atom::atomTypeIndex() const { return atomTypeIndex_; }