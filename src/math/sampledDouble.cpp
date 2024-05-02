// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/sampledDouble.h"
#include "base/lineParser.h"
#include <cmath>

SampledDouble::SampledDouble() { clear(); }

SampledDouble::SampledDouble(const double x)
{
    count_ = 1;
    mean_ = x;
    m2_ = 0.0;
}

/*
 * Data
 */

// Clear data
void SampledDouble::clear()
{
    count_ = 0;
    mean_ = 0.0;
    m2_ = 0.0;
}

// Return current (mean) value
double SampledDouble::value() const { return mean_; }

// Return number of samples contributing to averages etc.
int SampledDouble::count() const { return count_; }

// Return variance of sampled data
double SampledDouble::variance() const { return (count_ < 2 ? 0.0 : m2_ / (count_ - 1)); }

// Return standard deviation of sampled data
double SampledDouble::stDev() const { return (count_ < 2 ? 0.0 : sqrt(m2_ / (count_ - 1))); }

/*
 * Operators
 */

SampledDouble::operator double &() { return mean_; }

SampledDouble::operator const double &() const { return mean_; }

SampledDouble &SampledDouble::operator=(double x)
{
    // Clear any existing statistics and set new value
    count_ = 1;
    m2_ = 0.0;
    mean_ = x;

    return *this;
}

SampledDouble &SampledDouble::operator=(const SampledDouble &source) = default;

SampledDouble SampledDouble::operator+(double x) const
{
    auto result = *this;
    result += x;
    return result;
}

SampledDouble SampledDouble::operator*(double x) const
{
    auto result = *this;
    result *= x;
    return result;
}
SampledDouble SampledDouble::operator/(double x) const
{
    auto result = *this;
    result /= x;
    return result;
}

void SampledDouble::operator+=(double x)
{
    // Accumulate value using Welford's online algorithm
    // B. P. Welford, "Note on a method for calculating corrected sums of squares and products", Technometrics, 4(3),
    // 419–420 (1962).

    // Increase sample size counter
    ++count_;

    // Determine difference between supplied value and current mean
    const auto delta = x - mean_;

    // Accumulate mean
    mean_ += delta / count_;

    // Accumulate m2 using deltas of new value with old and new mean
    m2_ += delta * (x - mean_);
}

void SampledDouble::operator+=(int i) { (*this) += (double)i; }

void SampledDouble::operator+=(const SampledDouble &source)
{
    // Accumulate other values using parallel algorithm of Chan
    // T. F. Chan, G. H. Golub, R. J. LeVeque, "Updating Formulae and a Pairwise Algorithm for Computing Sample Variances.",
    // Technical Report STAN-CS-79-773, Department of Computer Science, Stanford University (1979).

    // Nothing to do if there are no samples in the source data
    if (source.count_ == 0)
        return;

    // Determine difference in mean values between samples B and A and reciprocal of total counts
    const auto deltaMean = source.value() - mean_;
    const auto rCountNew = 1.0 / (count_ + source.count_);

    // Calculate new mean
    mean_ += deltaMean * source.count_ * rCountNew;

    // Calculate new M2
    m2_ += source.m2_ + deltaMean * deltaMean * count_ * source.count_ * rCountNew;

    // Set new count
    count_ += source.count_;
}

void SampledDouble::operator*=(double x)
{
    // Apply factor to mean and m2_
    mean_ *= x;
    m2_ *= x;
}

void SampledDouble::operator/=(double x)
{
    // Apply factor to mean and m2_
    mean_ /= x;
    m2_ /= x;
}

/*
 * Serialisation
 */

// Read data through specified LineParser
bool SampledDouble::deserialise(LineParser &parser)
{
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    mean_ = parser.argd(0);
    count_ = parser.argi(1);
    m2_ = parser.argd(2);

    return true;
}

// Write data through specified LineParser
bool SampledDouble::serialise(LineParser &parser) const { return parser.writeLineF("{}  {}  {}\n", mean_, count_, m2_); }

/*
 * Parallel Comms
 */

// Sum data over all processes within the pool
bool SampledDouble::allSum(ProcessPool &procPool)
{
#ifdef PARALLEL
    // All processes in the pool send their data to the zero rank, which assembles the statistics and then broadcasts the
    // final result
    for (auto n = 1; n < procPool.nProcesses(); ++n)
    {
        if (procPool.poolRank() == 0)
        {
            // Rank zero receives the data and sums it
            SampledDouble data;
            if (!procPool.receive(data.count_, 0))
                return false;
            if (!procPool.receive(data.mean_, 0))
                return false;
            if (!procPool.receive(data.m2_, 0))
                return false;

            (*this) += data;
        }
        else
        {
            // Send our data to rank zero
            if (!procPool.send(count_, 0))
                return false;
            if (!procPool.send(mean_, 0))
                return false;
            if (!procPool.send(m2_, 0))
                return false;
        }
    }

    if (!procPool.broadcast(count_))
        return false;
    if (!procPool.broadcast(mean_))
        return false;
    if (!procPool.broadcast(m2_))
        return false;
#endif
    return true;
}
