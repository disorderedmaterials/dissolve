// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/atomType.h"
#include "math/vector3.h"

// Basic Atom
class Atom
{
    /*
     * Properties
     */
    private:
    // Coordinates
    Vector3 r_;
    // Atom type index in parent object
    int atomTypeIndex_{AtomType::Ignore};

    public:
    // Return coordinates
    Vector3 &r();
    const Vector3 &r() const;
    // Set index of associated atom type in parent object
    void setAtomTypeIndex(int id);
    // Return associated atom type index
    int atomTypeIndex() const;
};
