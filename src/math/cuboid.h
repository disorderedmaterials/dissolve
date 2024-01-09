// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "math/matrix4.h"

// Orthogonal Cuboid
class Cuboid
{
    public:
    Cuboid();

    /*
     * Definition
     */
    private:
    // Minimum and maximum coordinates for cuboid
    Vec3<double> minima_, maxima_;
    // Whether minima and maxima have ever been set
    bool minimaSet_, maximaSet_;

    public:
    // Update extreme coordinates with supplied vector
    void updateExtremes(Vec3<double> v);

    /*
     * Functions
     */
    public:
    // Return minima for cuboid
    Vec3<double> minima() const;
    // Return maxima for cuboid
    Vec3<double> maxima() const;
};
