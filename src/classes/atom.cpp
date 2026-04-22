// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/atom.h"

// Return coordinates
Vector3 &Atom::r() { return r_; }
const Vector3 &Atom::r() const { return r_; }

// Set index of associated atom type in parent object
void Atom::setAtomTypeIndex(int id) { atomTypeIndex_ = id; }

// Return associated atom type index
int Atom::atomTypeIndex() const { return atomTypeIndex_; }