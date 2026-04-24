// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/atomType.h"
#include "math/vector3.h"

// Basic Atom
class Atom
{
    public:
    Atom() = default;
    virtual ~Atom() = default;

    /*
     * Properties
     */
    protected:
    // Coordinates
    Vector3 r_;
    // Atomic element
    Elements::Element Z_{Elements::Unknown};
    // Charge
    double q_{0.0};
    // Index in parent
    int index_{-1};
    // Atom type index in parent object
    int atomTypeIndex_{AtomType::Ignore};

    public:
    // Set basic properties
    virtual void set(Elements::Element Z, const Vector3 &r, double q = 0.0);
    // Return coordinates
    Vector3 &r();
    const Vector3 &r() const;
    // Return atomic element
    Elements::Element &Z();
    Elements::Element Z() const;
    // Return atomic charge
    double &q();
    double q() const;
    // Return index
    int index() const;
    // Set index
    void setIndex(int index);
    // Set index of associated atom type in parent object
    void setAtomTypeIndex(int id);
    // Return associated atom type index
    int atomTypeIndex() const;
};
