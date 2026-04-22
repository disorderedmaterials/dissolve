// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

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

    public:
    // Return coordinates
    Vector3 &r();
    const Vector3 &r() const;
};
