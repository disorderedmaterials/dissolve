// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "classes/energykernel.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include "templates/algorithms.h"
#include <gtest/gtest.h>

namespace UnitTest
{

Vec3<double> manualMim(const Box &box, const Vec3<double> r1, const Vec3<double> r2)
{
    auto mim = box.inverseAxes() * (r1 - r2);
    if (mim.x < -0.5)
        mim.x += 1.0;
    else if (mim.x > 0.5)
        mim.x -= 1.0;
    if (mim.y < -0.5)
        mim.y += 1.0;
    else if (mim.y > 0.5)
        mim.y -= 1.0;
    if (mim.z < -0.5)
        mim.z += 1.0;
    else if (mim.z > 0.5)
        mim.z -= 1.0;
    return box.axes() * mim + r2;
}

void testBox(const Box &box)
{
    // Determine central coordinate from full axes matrix
    auto centroid = box.axes() * Vec3<double>(0.5, 0.5, 0.5);

    // For each corner, determine correct coordinates from full axes matrix, then test optimised imaging / vector functions
    for (auto n = 0; n < 8; ++n)
    {
        auto corner = box.axes() * Vec3<double>((n & 1) ? 1.0 : 0.0, (n & 2) ? 1.0 : 0.0, (n & 4) ? 1.0 : 0.0);

        // Calculate manual minimum image vector
        auto mimCorner = manualMim(box, corner, centroid);

        // Minimum distance
        EXPECT_NEAR((mimCorner - centroid).magnitude(), box.minimumDistance(centroid, corner), 1.0e-8);

        // Minimum distance squared
        EXPECT_NEAR((mimCorner - centroid).magnitudeSq(), box.minimumDistanceSquared(centroid, corner), 1.0e-8);

        // Minimum image vector
        auto mim = box.minimumImage(corner, centroid);
        EXPECT_NEAR(mimCorner.x, mim.x, 1.0e-8);
        EXPECT_NEAR(mimCorner.y, mim.y, 1.0e-8);
        EXPECT_NEAR(mimCorner.z, mim.z, 1.0e-8);

        // Fold
        auto scaledCorner = box.axes() * Vec3<double>((n & 1) ? -2.0 : 0.0, (n & 2) ? 5.0 : 0.0, (n & 4) ? -97.0 : 0.0);
        scaledCorner += centroid;
        auto p = box.fold(scaledCorner);
        EXPECT_NEAR(p.x, centroid.x, 1.0e-8);
        EXPECT_NEAR(p.y, centroid.y, 1.0e-8);
        EXPECT_NEAR(p.z, centroid.z, 1.0e-8);
        p = box.foldFrac(scaledCorner);
        EXPECT_NEAR(p.x, 0.5, 1.0e-8);
        EXPECT_NEAR(p.y, 0.5, 1.0e-8);
        EXPECT_NEAR(p.z, 0.5, 1.0e-8);
    }
}

TEST(BoxTest, Cubic) { testBox(CubicBox(10.0)); }

TEST(BoxTest, Orthorhombic)
{
    testBox(OrthorhombicBox({10.0, 20.0, 30.0}));
    testBox(OrthorhombicBox({15.0, 2.0, 88.0}));
}

TEST(BoxTest, Monoclinic)
{
    testBox(MonoclinicBox({30.0, 30.0, 30.0}, 66.0));
    testBox(MonoclinicBox({10.0, 20.0, 30.0}, 66.0));
}

TEST(BoxTest, Triclinic)
{
    testBox(TriclinicBox({30.0, 30.0, 30.0}, {66.0, 33.0, 77.0}));
    testBox(TriclinicBox({10.0, 20.0, 30.0}, {85.0, 80.0, 90.0}));
    testBox(TriclinicBox({27.0, 25.5, 31.2311}, {89.0, 120.0, 70.0}));
}

} // namespace UnitTest
