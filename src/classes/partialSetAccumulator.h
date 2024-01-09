// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/partialSet.h"
#include "math/sampledData1D.h"
#include "templates/array2D.h"

// Accumulator for PartialSet
class PartialSetAccumulator
{
    public:
    PartialSetAccumulator();
    ~PartialSetAccumulator() = default;
    void operator+=(const PartialSet &source);

    /*
     * Partials Data
     */
    private:
    // Number of accumulated points
    int nAccumulated_{0};
    // Full matrix, containing sampling of full atom-atom partial
    Array2D<SampledData1D> partials_;
    // Bound matrix, containing sampling of atom-atom partial of bound pairs
    Array2D<SampledData1D> boundPartials_;
    // Unbound matrix, containing sampling of atom-atom partial of unbound pairs
    Array2D<SampledData1D> unboundPartials_;
    // Sample of the total function
    SampledData1D total_;

    public:
    // Return number of accumulated points
    int nAccumulated() const;
    // Return full matrix, containing sampling of full atom-atom partial
    const Array2D<SampledData1D> &partials() const;
    // Return bound matrix, containing sampling of atom-atom partial of bound pairs
    const Array2D<SampledData1D> &boundPartials() const;
    // Return unbound matrix, containing sampling of atom-atom partial of unbound pairs
    const Array2D<SampledData1D> &unboundPartials() const;
    // Return the sampled total function
    const SampledData1D &total() const;
    // Save all partials and total (with errors)
    bool save(std::string_view prefix, std::string_view tag, std::string_view suffix, std::string_view abscissaUnits) const;

    /*
     * Searchers
     */
    public:
    // Return SampledData1D with specified tag, if it exists
    OptionalReferenceWrapper<const SampledData1D> searchSampledData1D(std::string_view tag) const;

    /*
     * Serialisation
     */
    private:
    // Read array through specified LineParser
    static bool deserialiseArray(LineParser &parser, Array2D<SampledData1D> &array, const std::vector<double> &xAxis);
    // Write array through specified LineParser
    static bool serialiseArray(LineParser &parser, const Array2D<SampledData1D> &array);

    public:
    // Read data through specified LineParser
    bool deserialise(LineParser &parser);
    // Write data through specified LineParser
    bool serialise(LineParser &parser) const;
};
