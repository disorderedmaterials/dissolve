// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "templates/listitem.h"
#include <map>

// Forward Declarations
/* none */

// Geometry Definition
class Geometry : public ListItem<Geometry>
{
    public:
    Geometry();
    ~Geometry();
    // Type of geometry data passed
    enum GeometryType
    {
        AngleType,
        DistanceType,
        TorsionType
    };

    /*
     * Data
     */
    private:
    // Reference value
    double value_;
    // Array for indices
    int indices_[4];

    public:
    // Set reference value and indices
    void set(double value, int a, int b, int c = -1, int d = -1);
    // Return reference value
    double value();
    // Return specified index
    int indices(int i) const;
};
