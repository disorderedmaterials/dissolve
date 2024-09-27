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
bool Peaks::isLocalMaximum(double *points) { return (points[1] - points[0] > 0) && (points[2] - points[1] <= 0); }

// Check if neighbouring points correspond to a local minimum
bool Peaks::isLocalMinimum(double *points)
{
    double invertedPoints[3] = {-1.0 * points[0], -1.0 * points[1], -1.0 * points[2]};
    return isLocalMaximum(invertedPoints);
}

// Check if neighbouring points correspond to an inflection point
bool Peaks::isInflectionPoint(double *points)
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
void Peaks::sortProminences(std::vector<Peaks::Prominence1D> &proms)
{
    std::sort(proms.begin(), proms.end(), [](const auto &a, const auto &b) { return a.prominence > b.prominence; });
}

// Get top n peaks
std::vector<Peaks::Peak1D> Peaks::top(int n, std::vector<Peaks::Peak1D> peaks)
{
    sortPeaks(peaks);

    if (abs(n) == peaks.size())
        return peaks;

    std::vector<Peaks::Peak1D> isolatedPeaks;
    isolatedPeaks.reserve(peaks.size());

    for (const auto &peak : peaks)
    {
        bool withinRadius = std::any_of(isolatedPeaks.begin(), isolatedPeaks.end(),
                                        [this, &peak](const auto &p)
                                        {
                                            // return compare(p, peak);
                                            return (p.index != peak.index) && ((p.valueAt > peak.valueAt - isolation_) &&
                                                                               (p.valueAt < peak.valueAt + isolation_));
                                        });

        if (!withinRadius)
            isolatedPeaks.push_back(peak);

        if (isolatedPeaks.size() == abs(n))
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
        double neighbours[3] = {values_[i - 1], values_[i], values_[i + 1]};

        if (values_[i] > threshold_ && isLocalMaximum(neighbours))
        {
            peaks.emplace_back(values_[i], domain_[i], i);
        }
    }

    if (isolation_ > 0)
    {
        auto maxX = *std::max_element(domain_.begin(), domain_.end());
        auto isolatedPeaks = top(std::round(maxX / isolation_), peaks);
        if (!heightOrder)
            sortIndices<Peak1D>(isolatedPeaks);
        return isolatedPeaks;
    }
    return peaks;
}

// Calculate prominences of peaks
std::vector<Peaks::Prominence1D> Peaks::prominences(bool heightOrder)
{
    auto peaks = find(heightOrder);
    return prominences(peaks, heightOrder);
}

std::vector<Peaks::Prominence1D> Peaks::prominences(const std::vector<Peaks::Peak1D> &peaks, bool heightOrder)
{
    std::vector<Peaks::Prominence1D> proms;
    proms.reserve(peaks.size());

    for (const auto &peak : peaks)
    {
        int iLeft = peak.index - 1, iRight = peak.index + 1;
        double heightRefLeft = 0, heightRefRight = 0;

        while (iLeft > 0)
        {
            double pointsLeft[3] = {values_[iLeft + 1], values_[iLeft], values_[iLeft - 1]};

            if (isLocalMinimum(pointsLeft) || isInflectionPoint(pointsLeft))
            {
                heightRefLeft += values_[iLeft];
                break;
            }
            else
                iLeft--;
        }

        while (iRight < values_.size())
        {
            double pointsRight[3] = {values_[iRight - 1], values_[iRight], values_[iRight + 1]};

            if (isLocalMinimum(pointsRight) || isInflectionPoint(pointsRight))
            {
                heightRefRight += values_[iRight];
                break;
            }
            else
                iRight++;
        }

        if ((heightRefLeft == 0) && (heightRefRight == 0))
        {
            Messenger::print("Could not calculate reference height for prominence of peak at index {} (no local minima or "
                             "inflection point found in either direction)\n",
                             peak.index);
            break;
        }

        auto prominence = std::min(abs(heightRefLeft - peak.peak), abs(heightRefRight - peak.peak));
        proms.emplace_back(&peak, prominence);
    }
    if (!heightOrder)
        sortIndices<Prominence1D>(proms);
    return proms;
}