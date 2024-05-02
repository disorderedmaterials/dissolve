// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/sampledVector.h"
#include "base/lineParser.h"
#include "templates/algorithms.h"
#include <cmath>

SampledVector::SampledVector() {}

/*
 * Data
 */

// Initialise arrays
void SampledVector::initialise(int nValues)
{
    mean_.resize(nValues);
    m2_.resize(nValues);
    stDev_.resize(nValues);
    reset();
}

// Initialise from pre-existing arrays
void SampledVector::initialise(int count, const std::vector<double> &mean, const std::vector<double> &m2,
                               const std::vector<double> &stDev)
{
    clear();

    count_ = count;
    mean_ = mean;
    m2_ = m2;
    stDev_ = stDev;
}

// Clear all arrays
void SampledVector::clear()
{
    mean_.clear();
    m2_.clear();
    stDev_.clear();
    count_ = 0;
}

// Reset values and statistics
void SampledVector::reset()
{
    count_ = 0;
    std::fill(mean_.begin(), mean_.end(), 0.0);
    std::fill(m2_.begin(), m2_.end(), 0.0);
    std::fill(stDev_.begin(), stDev_.end(), 0.0);
}

// Return number of samples contributing to averages etc.
int SampledVector::count() const { return count_; }

// Return current (mean) values
const std::vector<double> &SampledVector::values() const { return mean_; }

// Return standard deviations of values
const std::vector<double> &SampledVector::stDev() const { return stDev_; }

// Return squared deviations
const std::vector<double> &SampledVector::m2() const { return m2_; }

/*
 * Operators
 */

SampledVector &SampledVector::operator=(const SampledVector &source)
{
    count_ = source.count_;
    mean_.resize(source.mean_.size());
    std::copy(source.mean_.begin(), source.mean_.end(), mean_.begin());
    m2_.resize(source.m2_.size());
    std::copy(source.m2_.begin(), source.m2_.end(), m2_.begin());
    stDev_.resize(source.stDev_.size());
    std::copy(source.stDev_.begin(), source.stDev_.end(), stDev_.begin());

    return *this;
}

SampledVector &SampledVector::operator+=(const std::vector<double> &source)
{
    // Accumulate value using Welford's online algorithm
    // B. P. Welford, "Note on a method for calculating corrected sums of squares and products", Technometrics, 4(3),
    // 419–420 (1962).

    // If the sample size is currently zero, initialise to the size of the source vector
    if (count_ == 0)
        initialise(source.size());

    // Check vector size consistency
    if (mean_.size() != source.size())
        throw(std::runtime_error(
            fmt::format("Vector passed to SampledVector::operator+= has a different size ({}) to the current data ({}).\n",
                        source.size(), mean_.size())));

    // Increase sample size counter
    ++count_;

    double delta;
    for (auto &&[mean, m2, stDev, x] : zip(mean_, m2_, stDev_, source))
    {
        // Determine difference between supplied value and current mean
        delta = x - mean;

        // Accumulate mean
        mean += delta / count_;

        // Accumulate m2 using deltas of new value with old and new mean
        m2 += delta * (x - mean);

        // Determine new standard deviation
        stDev = (count_ < 2 ? 0.0 : sqrt(m2 / (count_ - 1)));
    }

    return *this;
}

SampledVector &SampledVector::operator+=(const SampledVector &source)
{
    // Accumulate other values using parallel algorithm of Chan
    // T. F. Chan, G. H. Golub, R. J. LeVeque, "Updating Formulae and a Pairwise Algorithm for Computing Sample Variances.",
    // Technical Report STAN-CS-79-773, Department of Computer Science, Stanford University (1979).

    // Check vector size consistency
    if (mean_.size() != source.values().size())
        throw(std::runtime_error(fmt::format(
            "SampledVector passed to SampledVector::operator+= has a different size ({}) to the current data ({}).\n",
            source.values().size(), mean_.size())));

    const auto newCount = count_ + source.count_;
    const auto rCountNew = 1.0 / newCount;
    double deltaMean;

    for (auto &&[meanA, m2A, stDevA, meanB, m2B] : zip(mean_, m2_, stDev_, source.mean_, source.m2_))
    {
        // Determine difference in mean values between samples B and A
        deltaMean = meanB - meanA;

        // Calculate new mean
        meanA += deltaMean * source.count_ * rCountNew;

        // Calculate new M2
        m2A += m2B + deltaMean * deltaMean * count_ * source.count_ * rCountNew;

        // Calculate new standard deviation
        stDevA = (count_ < 2 ? 0.0 : sqrt(m2A / (newCount - 1)));
    }

    // Set new count
    count_ = newCount;

    return *this;
}

void SampledVector::operator*=(double x)
{
    // Apply factor to mean_, m2_, and stDev_
    std::transform(mean_.begin(), mean_.end(), mean_.begin(), [=](auto mean) { return mean * x; });
    std::transform(m2_.begin(), m2_.end(), m2_.begin(), [=](auto m2) { return m2 * x; });
    std::transform(stDev_.begin(), stDev_.end(), stDev_.begin(), [=](auto stDev) { return stDev * x; });
}

void SampledVector::operator/=(double x)
{
    // Apply factor to mean_, m2_, and stDev_
    std::transform(mean_.begin(), mean_.end(), mean_.begin(), [=](auto mean) { return mean / x; });
    std::transform(m2_.begin(), m2_.end(), m2_.begin(), [=](auto m2) { return m2 / x; });
    std::transform(stDev_.begin(), stDev_.end(), stDev_.begin(), [=](auto stDev) { return stDev / x; });
}

/*
 * Serialisation
 */

// Read data through specified LineParser
bool SampledVector::deserialise(LineParser &parser)
{
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    initialise(parser.argi(0));
    count_ = parser.argi(1);

    for (auto &&[mean, stDev, m2] : zip(mean_, stDev_, m2_))
    {
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;

        mean = parser.argd(0);
        stDev = parser.argd(1);
        m2 = parser.argd(2);
    }

    return true;
}

// Write data through specified LineParser
bool SampledVector::serialise(LineParser &parser) const
{
    if (!parser.writeLineF("{} {} # nData count\n", mean_.size(), count_))
        return false;

    for (auto &&[mean, stDev, m2] : zip(mean_, stDev_, m2_))
        if (!parser.writeLineF("{} {} {}\n", mean, stDev, m2))
            return false;

    return true;
}
