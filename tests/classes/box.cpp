// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/box.h"
#include <gtest/gtest.h>

namespace UnitTest
{
Vec3<double> manualMim(Box &box, const Vec3<double> r1, const Vec3<double> r2)
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

void scaleBox(Box &box, double requestedVolume, Vec3<bool> scalableAxes, bool uniform = false)
{
    // Get original lengths, angles, and volume for comparison
    auto lengths = box.axisLengths();
    auto angles = box.axisAngles();
    auto volume = box.volume();

    // Scale box to new volume and check it
    if (box.type() == Box::BoxType::Cubic && !uniform)
    {
        EXPECT_ANY_THROW(box.scale(box.scaleFactors(requestedVolume, scalableAxes)));
        return;
    }
    else
        box.scale(box.scaleFactors(requestedVolume, scalableAxes));
    EXPECT_NEAR(requestedVolume, box.volume(), 1.0e-8);
    if (uniform)
    {
        EXPECT_NEAR(lengths.x / lengths.y, box.axisLength(0) / box.axisLength(1), 1.0e-8);
        EXPECT_NEAR(lengths.x / lengths.z, box.axisLength(0) / box.axisLength(2), 1.0e-8);
        EXPECT_NEAR(lengths.y / lengths.z, box.axisLength(1) / box.axisLength(2), 1.0e-8);
    }

    // Rescale back to old volume and check parameters
    box.scale(box.scaleFactors(volume, scalableAxes));
    EXPECT_NEAR(volume, box.volume(), 1.0e-8);
    EXPECT_NEAR(lengths.x, box.axisLength(0), 1.0e-8);
    EXPECT_NEAR(lengths.y, box.axisLength(1), 1.0e-8);
    EXPECT_NEAR(lengths.z, box.axisLength(2), 1.0e-8);
    EXPECT_NEAR(angles.x, box.axisAngle(0), 1.0e-8);
    EXPECT_NEAR(angles.y, box.axisAngle(1), 1.0e-8);
    EXPECT_NEAR(angles.z, box.axisAngle(2), 1.0e-8);
}

void testBasicOperations(Box &box)
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

void testScaling(Box &box)
{
    // Scaling
    auto factor = 1.8371286;
    // -- Uniform scaling
    scaleBox(box, box.volume() * factor, {true, true, true}, true);
    // -- Non-uniform, single axis
    scaleBox(box, box.volume() * factor, {true, false, false});
    scaleBox(box, box.volume() * factor, {false, true, false});
    scaleBox(box, box.volume() * factor, {false, false, true});
    // -- Non-uniform, two axes
    scaleBox(box, box.volume() * factor, {true, true, false});
    scaleBox(box, box.volume() * factor, {false, true, true});
    scaleBox(box, box.volume() * factor, {true, false, true});
}

void testPBC(const Box &box, const Vec3<double> origin, int nPoints = 100)
{
    // Create a number of points on a sphere about the specified origin, on a radius corresponding to the max mim
    std::vector<Vec3<double>> points;
    points.reserve(nPoints);
    const auto r = box.inscribedSphereRadius();
    for (auto n = 0; n < nPoints; ++n)
    {
        auto theta = DissolveMath::random() * M_PI;
        auto phi = DissolveMath::random() * 2.0 * M_PI;
        points.emplace_back(Vec3<double>(r * sin(theta) * cos(phi), r * sin(theta) * sin(phi), r * cos(theta)) + origin);
    }

    // Test point distances from origin
    for (const auto &p : points)
        EXPECT_NEAR(r, box.minimumDistance(origin, p), 1.0e-6);
}

TEST(BoxTest, Cubic)
{
    auto box = CubicBox(10.0);
    testBasicOperations(box);
    testPBC(box, {1.0, 2.4, 5.2131});
    testScaling(box);
}

TEST(BoxTest, Orthorhombic)
{
    auto box1 = OrthorhombicBox({10.0, 20.0, 30.0});
    testBasicOperations(box1);
    testPBC(box1, {9.4, 13.0491, 1.325});
    testScaling(box1);
    auto box2 = OrthorhombicBox({15.0, 2.0, 88.0});
    testBasicOperations(box2);
    testPBC(box2, {9.4, 13.0491, 1.325});
    testScaling(box2);
}

TEST(BoxTest, Monoclinic)
{
    auto box1 = MonoclinicAlphaBox({30.0, 30.0, 30.0}, 66.0);
    testBasicOperations(box1);
    testPBC(box1, {3.4, 4.902, 15.875});
    testScaling(box1);
    auto box2 = MonoclinicAlphaBox({10.0, 20.0, 30.0}, 120.0);
    testBasicOperations(box2);
    testPBC(box2, {3.4, 4.902, 15.875});
    testScaling(box2);
    auto box3 = MonoclinicBetaBox({30.0, 30.0, 30.0}, 66.0);
    testBasicOperations(box3);
    testPBC(box3, {3.4, 4.902, 15.875});
    testScaling(box3);
    auto box4 = MonoclinicBetaBox({10.0, 20.0, 30.0}, 120.0);
    testBasicOperations(box4);
    testPBC(box4, {3.4, 4.902, 15.875});
    testScaling(box4);
    auto box5 = MonoclinicGammaBox({30.0, 30.0, 30.0}, 66.0);
    testBasicOperations(box5);
    testPBC(box5, {3.4, 4.902, 15.875});
    testScaling(box5);
    auto box6 = MonoclinicGammaBox({10.0, 20.0, 30.0}, 120.0);
    testBasicOperations(box6);
    testPBC(box6, {3.4, 4.902, 15.875});
    testScaling(box6);
}

TEST(BoxTest, Triclinic)
{
    auto box1 = TriclinicBox({30.0, 30.0, 30.0}, {66.0, 33.0, 77.0});
    testBasicOperations(box1);
    testPBC(box1, {14.8, 8.77, 0.01});
    testScaling(box1);
    auto box2 = TriclinicBox({10.0, 20.0, 30.0}, {85.0, 80.0, 90.0});
    testBasicOperations(box2);
    testPBC(box2, {14.8, 8.77, 0.01});
    testScaling(box2);
    auto box3 = TriclinicBox({27.0, 25.5, 31.2311}, {89.0, 120.0, 70.0});
    testBasicOperations(box3);
    testPBC(box3, {14.8, 8.77, 0.01});
    testScaling(box3);
}

} // namespace UnitTest
