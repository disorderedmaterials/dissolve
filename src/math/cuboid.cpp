// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/cuboid.h"
#include "math/mathFunc.h"

Cuboid::Cuboid()
{
    minima_.zero();
    maxima_.zero();
    minimaSet_ = false;
    maximaSet_ = false;
}

/*
 * Definition
 */

// Update extreme coordinates with supplied vector
void Cuboid::updateExtremes(Vec3<double> v)
{
    if (!minimaSet_)
        minima_ = v;
    else if (v.x < minima_.x)
        minima_.x = v.x;
    else if (v.y < minima_.y)
        minima_.y = v.y;
    else if (v.z < minima_.z)
        minima_.z = v.z;

    if (!maximaSet_)
        maxima_ = v;
    else if (v.x > maxima_.x)
        maxima_.x = v.x;
    else if (v.y > maxima_.y)
        maxima_.y = v.y;
    else if (v.z > maxima_.z)
        maxima_.z = v.z;

    minimaSet_ = true;
    maximaSet_ = true;
}

/*
 * Functions
 */

// Return minima for cuboid
Vec3<double> Cuboid::minima() const { return minima_; }

// Return maxima for cuboid
Vec3<double> Cuboid::maxima() const { return maxima_; }
