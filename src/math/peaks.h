// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "math/data1D.h"
#include "templates/vector3.h"
#include <variant>

// Forward declarations

/*
 * Peaks
 */
class Peaks
{
    public:
    Peaks(const std::vector<double> &values, const std::vector<double> &domain);
    Peaks(const Data1D &source);
    ~Peaks() = default;

    private:
    // Maximum y-axis value
    double maxValue_;
    // Values for which we are characterising peaks
    std::vector<double> values_;
    // Domain for which we are characterising peaks
    std::vector<double> domain_;
    // Characterise only peaks occuring above a given vertical threshold
    double threshold_;
    // Characterise only peaks occuring above a given horizontal isolation
    double isolation_;
    // Check if neighbouring points correspond to a local maximum
    bool isLocalMaximum(double *points);
    // Check if neighbouring points correspond to a local minimum
    bool isLocalMinimum(double *points);
    // Check if neighbouring points correspond to an inflection point
    bool isInflectionPoint(double *points);

    public:
    // Container for a peak occuring in 1D data
    struct Peak1D
    {
        double peak;
        double valueAt;
        int index;
        Peak1D(double peak, double valueAt, int index) : peak(peak), valueAt(valueAt), index(index) {}
    };

    // Container for a prominence occuring in 1D data
    struct Prominence1D : public Peak1D
    {
        double prominence;
        Prominence1D(const Peak1D *parent, double prominence)
            : Peak1D(parent->peak, parent->valueAt, parent->index), prominence(prominence)
        {
        }
    };

    public:
    // Reset the threshold and isolation constraints
    void resetConstraints();
    // Sort peaks in place by peak value, from highest to lowest
    void sortPeaks(std::vector<Peak1D> &peaks);
    // Sort peaks in place by index, from first to last
    template <typename T> void sortIndices(std::vector<T> &items);
    // Sort prominences in place by prominence value, from highest to lowest
    void sortProminences(std::vector<Prominence1D> &proms);
    // Sort peaks in place by index, from first to last
    void setThreshold(double range);
    // Set horizontal threshold for peaks
    void setIsolation(double range);
    // Get top n peaks
    std::vector<Peak1D> top(int n, std::vector<Peak1D>);
    // Find the peaks (local maxima) of data
    std::vector<Peak1D> find(bool heightOrder = false);
    // Calculate prominences of peaks
    std::vector<Prominence1D> prominences(bool heightOrder = false);
    std::vector<Prominence1D> prominences(const std::vector<Peak1D> &peaks, bool heightOrder = false);
};