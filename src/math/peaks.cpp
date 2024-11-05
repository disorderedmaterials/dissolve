// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/peaks.h"
#include "math/mathFunc.h"
#include <algorithm>

/*
 * Peaks
 */
Peaks::Peaks(const std::vector<double> &values, const std::vector<double> &domain) : values_(values), domain_(domain)
{
    maxValue_ = *std::max_element(values_.begin(), values_.end());
    resetConstraints();
}

Peaks::Peaks(const Data1D &source) : values_(source.values()), domain_(source.xAxis())
{
    maxValue_ = *std::max_element(values_.begin(), values_.end());
    resetConstraints();
}

// Set vertical threshold for peaks
void Peaks::setThreshold(double threshold) { threshold_ = threshold; }

// Set horizontal isolation radius for peaks
void Peaks::setIsolation(double radius) { isolation_ = radius; }

// Reset the threshold and isolation constraints
void Peaks::resetConstraints() { threshold_ = -10e9, isolation_ = 0; }

// Check if neighbouring points correspond to a local maximum
bool Peaks::isLocalMaximum(const std::array<double, 3> &points)
{
    return (points[1] - points[0] > 0) && (points[2] - points[1] <= 0);
}

// Check if neighbouring points correspond to a local minimum
bool Peaks::isLocalMinimum(const std::array<double, 3> &points)
{
    std::array<double, 3> invertedPoints = {-1.0 * points[0], -1.0 * points[1], -1.0 * points[2]};
    return isLocalMaximum(invertedPoints);
}

// Check if neighbouring points correspond to an inflection point
bool Peaks::isInflectionPoint(const std::array<double, 3> &points)
{
    return (((points[1] - points[0] > 0) || ((points[1] - points[0] < 0))) && (points[2] - points[1] == 0)) ||
           (((points[2] - points[1] > 0) || ((points[2] - points[1] < 0))) && (points[1] - points[0] == 0));
}

// Sort peaks in place by peak value, from highest to lowest
void Peaks::sortPeaks(std::vector<Peaks::Peak1D> &peaks)
{
    std::sort(peaks.begin(), peaks.end(), [](const auto &a, const auto &b) { return a.peak > b.peak; });
}

// Sort peaks in place by index, from first to last
template <typename T> void Peaks::sortIndices(std::vector<T> &items)
{
    std::sort(items.begin(), items.end(), [](const auto &a, const auto &b) { return a.index < b.index; });
}

// Sort prominences in place by prominence value, from highest to lowest
void Peaks::sortProminences(std::vector<Peaks::Prominence1D> &prominences)
{
    std::sort(prominences.begin(), prominences.end(), [](const auto &a, const auto &b) { return a.prominence > b.prominence; });
}

// Get top n peaks
std::vector<Peaks::Peak1D> Peaks::top(std::size_t n, std::vector<Peaks::Peak1D> peaks)
{
    sortPeaks(peaks);

    std::vector<Peaks::Peak1D> isolatedPeaks;
    isolatedPeaks.reserve(peaks.size());

    for (const auto &peak : peaks)
    {
        bool withinRadius = std::any_of(isolatedPeaks.begin(), isolatedPeaks.end(),
                                        [this, &peak](const auto &p) {
                                            return (p.index != peak.index) && ((p.valueAt > peak.valueAt - isolation_) &&
                                                                               (p.valueAt < peak.valueAt + isolation_));
                                        });

        if (!withinRadius)
            isolatedPeaks.push_back(peak);

        // Break if we exceed specified number peaks
        if (isolatedPeaks.size() >= n)
            break;
    }
    return isolatedPeaks;
}

// Find the peaks (local maxima) of data
std::vector<Peaks::Peak1D> Peaks::find(bool heightOrder)
{
    std::vector<Peaks::Peak1D> peaks;
    peaks.reserve(values_.size());

    for (int i = 1; i < values_.size() - 1; i++)
    {
        // Check if the gradients either side show this is a local maximum
        std::array<double, 3> neighbours = {values_[i - 1], values_[i], values_[i + 1]};

        if (values_[i] > threshold_ && isLocalMaximum(neighbours))
        {
            peaks.emplace_back(values_[i], domain_[i], i);
        }
    }

    if (isolation_ > 0)
    {
        auto dx = abs(peaks.back().valueAt - peaks.front().valueAt);
        auto isolatedPeaks = top(std::round(dx / isolation_), peaks);
        if (!heightOrder)
            sortIndices<Peak1D>(isolatedPeaks);
        return isolatedPeaks;
    }
    return peaks;
}

/*
 * Calculate the prominence of peaks.
 * Prominence is defined by the height of the peak relative to a reference height.
 * This reference is determined by the heighest minimum of two intervals (bound by either the end of the data
 * or a higher data point), either side of the peak itself.
 */
std::vector<Peaks::Prominence1D> Peaks::prominences(bool heightOrder)
{
    auto peaks = find(heightOrder);
    return prominences(peaks, heightOrder);
}

std::vector<Peaks::Prominence1D> Peaks::prominences(const std::vector<Peaks::Peak1D> &peaks, bool heightOrder)
{
    std::vector<Peaks::Prominence1D> prominences;
    prominences.reserve(peaks.size());

    for (const auto &peak : peaks)
    {
        auto iLeft = peak.index - 1, iRight = peak.index + 1;

        // Get interval to left of peak and find its minimum
        for (; iLeft > 0; iLeft--)
            if (values_[iLeft] >= peak.peak)
                break;

        auto heightRefLeft = *std::min_element(values_.begin() + iLeft, values_.begin() + peak.index);

        // Get interval to right of peak and find its minimum
        for (; iRight < values_.size(); iRight++)
            if (values_[iRight] >= peak.peak)
                break;

        auto heightRefRight = *std::min_element(values_.begin() + peak.index, values_.begin() + iRight);

        // Calculate reference height for prominence from the heighest of the two minima
        auto referenceHeight = std::max(heightRefLeft, heightRefRight);

        auto prominence = peak.peak - referenceHeight;
        prominences.emplace_back(&peak, prominence);
    }

    if (heightOrder)
        sortProminences(prominences);

    return prominences;
}