// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "templates/array.h"
#include "templates/listitem.h"
#include "templates/vector3.h"

// Forward Declarations
/* none */

// Coordinate Set
class CoordinateSet : public ListItem<CoordinateSet>
{
    public:
    CoordinateSet();
    ~CoordinateSet();

    /*
     * Coordinates
     */
    protected:
    // Coordinates
    Array<Vec3<double>> coordinates_;

    public:
    // Initialise to specified size
    void initialise(int size);
    // Return size
    int size() const;
    // Set specified coordinates
    void set(int index, Vec3<double> r);
    // Return specified coordinates
    Vec3<double> r(int index) const;
    // Return coordinates array
    Array<Vec3<double>> &coordinates();
};
