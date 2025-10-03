// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once
#include <vector>

// Forward Declarations
class Data1D;

// Interpolator
class Interpolator
{
    public:
    // Interpolation Schemes
    enum InterpolationScheme
    {
        SplineInterpolation,
        /* ConstrainedSplineInterpolation, */ // Removed for now as it produces spurious features in some fits.
        LinearInterpolation,
        ThreePointInterpolation
    };
    Interpolator(const std::vector<double> &x, const std::vector<double> &y, InterpolationScheme scheme = SplineInterpolation);
    Interpolator(const Data1D &source, InterpolationScheme scheme = SplineInterpolation);
    ~Interpolator() = default;

    /*
     * Interpolation
     */
    private:
    // Target x array
    std::vector<double> x_;
    // Target y array
    std::vector<double> y_;
    // Interpolation scheme currently employed
    InterpolationScheme scheme_;
    // Interpolation parameters
    std::vector<double> a_, b_, c_, d_, h_;
    // Interval of last returned interpolated point
    int lastInterval_;

    private:
    // Prepare natural spline interpolation of data
    void interpolateSpline();
    // Prepare constrained natural spline interpolation of data
    void interpolateConstrainedSpline();
    // Prepare linear interpolation of data
    void interpolateLinear();
    // Prepare three-point interpolation of data
    void interpolateThreePoint();
    // Regenerate using specified scheme
    void interpolate();

    public:
    // Return interpolated y value for supplied x
    double y(double x) const;
    // Return interpolated y value for supplied x from the specified interval
    double y(double x, int interval) const;
    // Return interpolated y values for supplied, sequentially increasing x values
    std::vector<double> y(const std::vector<double> &xs) const;

    /*
     * Static Functions
     */
    public:
    // Approximate y at specified x value using three-point interpolation of supplied data
    static double approximate(const Data1D &data, double x);
    // Add interpolated source data into destination with supplied multiplication factor
    static void addInterpolated(const Data1D &source, Data1D &dest, double factor = 1.0);
    // Add interpolation into destination with supplied multiplication factor
    static void addInterpolated(Interpolator &source, Data1D &dest, double factor = 1.0);
};
