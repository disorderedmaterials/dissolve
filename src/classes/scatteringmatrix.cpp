// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/scatteringmatrix.h"
#include "classes/atomtype.h"
#include "classes/neutronweights.h"
#include "classes/xrayweights.h"
#include "math/interpolator.h"
#include "math/svd.h"
#include "templates/algorithms.h"
#include <algorithm>

ScatteringMatrix::ScatteringMatrix() {}

/*
 * Data
 */

// Return number of reference AtomType pairs
int ScatteringMatrix::nPairs() const { return typePairs_.size(); }

// Return index of specified AtomType pair
int ScatteringMatrix::pairIndex(std::shared_ptr<AtomType> typeI, std::shared_ptr<AtomType> typeJ) const
{
    auto index = 0;
    for (auto [i, j] : typePairs_)
    {
        if ((i == typeI) && (j == typeJ))
            return index;
        if ((i == typeJ) && (j == typeI))
            return index;
        ++index;
    }

    return -1;
}

// Return weight of the specified AtomType pair in the inverse matrix
double ScatteringMatrix::pairWeightInverse(double q, std::shared_ptr<AtomType> typeI, std::shared_ptr<AtomType> typeJ,
                                           int dataIndex) const
{
    /*
     * The required row of the inverse matrix is the index of the AtomType pair.
     * The required column of the inverse matrix is the original (row) index of the supplied data.
     */

    auto index = pairIndex(typeI, typeJ);
    return inverse(q)[{index, dataIndex}];
}

// Calculate and return the scattering matrix at the specified Q value
Array2D<double> ScatteringMatrix::matrix(double q) const
{
    // Take a copy of A to begin with
    auto m = A_;

    // Go over rows of the matrix (corresponding to the reference data) and check if any need to be xray-weighted
    for (auto row = 0; row < m.nRows(); ++row)
    {
        // If this is not xray data we can move on to the next
        if (!std::get<0>(xRayData_[row]))
            continue;

        // Grab the weights and normalisation to apply to the matrix elements
        auto optWeights = std::get<1>(xRayData_[row]);
        auto &weights = (*optWeights);
        auto normType = std::get<2>(xRayData_[row]);
        auto normFactor = 1.0;
        if (normType == StructureFactors::AverageOfSquaresNormalisation)
            normFactor = weights.boundCoherentAverageOfSquares(q);
        else if (normType == StructureFactors::SquareOfAverageNormalisation)
            normFactor = weights.boundCoherentSquareOfAverage(q);

        // Loop over columns (partials) and weight according to the elements of the atom types
        auto col = 0;
        for (auto [i, j] : typePairs_)
        {
            auto ffi = XRayFormFactors::formFactorData(weights.formFactors(), i->Z());
            if (!ffi)
                throw(std::runtime_error(fmt::format("No form factor data available for element {} in dataset {}.",
                                                     Elements::name(i->Z()),
                                                     XRayFormFactors::xRayFormFactorData().keyword(weights.formFactors()))));
            auto ffj = XRayFormFactors::formFactorData(weights.formFactors(), j->Z());
            if (!ffj)
                throw(std::runtime_error(fmt::format("No form factor data available for element {} in dataset {}.",
                                                     Elements::name(j->Z()),
                                                     XRayFormFactors::xRayFormFactorData().keyword(weights.formFactors()))));

            m[{row, col}] *= ffi->get().magnitude(q) * ffj->get().magnitude(q) / normFactor;

            ++col;
        }
    }

    return m;
}

// Calculate and return the inverse matrix at the specified Q value
Array2D<double> ScatteringMatrix::inverse(double q) const
{
    // Get the scattering matrix at the specified Q value
    auto inverseA = matrix(q);

    // Invert the matrix and return
    if (!SVD::pseudoinverse(inverseA))
        throw(std::runtime_error("Failed to invert the scattering matrix."));

    return inverseA;
}

// Print the scattering coefficients matrix at the specified Q value
void ScatteringMatrix::print(double q) const
{
    auto m = matrix(q);

    // Write header
    std::string text, line;
    auto nColsWritten = 0;
    for (auto [i, j] : typePairs_)
    {
        text = fmt::format("{}-{}", i->name(), j->name());
        line += fmt::format("{:^10} ", text);

        // Limit output to sensible length
        if (line.length() >= 80)
        {
            line += " ...";
            break;
        }

        ++nColsWritten;
    }
    Messenger::print("{}", line);

    // Loop over reference data
    for (auto row = 0; row < data_.nItems(); ++row)
    {
        line.clear();
        for (auto n = 0; n < m.nColumns(); ++n)
        {
            line += fmt::format("{:10f} ", m[{row, n}]);

            // Limit output to sensible length
            if (line.length() >= 80)
            {
                line += " ...";
                break;
            }
        }
        Messenger::print("{}  {}\n", line, data_.at(row).tag());

        // Limit to sensible number of rows
        if (row >= std::max(nColsWritten, 10))
        {
            line.clear();
            for (auto n = 0; n < nColsWritten; ++n)
                line += "    ...    ";
            Messenger::print("{}\n", line);
            break;
        }
    }
}

// Print the inverse matrix at the specified Q value
void ScatteringMatrix::printInverse(double q) const
{
    auto inverseA = inverse(q);

    // Write header
    std::string line;
    auto nColsWritten = 0;
    for (auto [i, j] : typePairs_)
    {
        line += fmt::format("{:10} ", fmt::format("{}-{}", i->name(), j->name()));

        // Limit output to sensible length
        if (line.length() >= 80)
        {
            line += " ...";
            break;
        }

        ++nColsWritten;
    }
    Messenger::print(line);

    // Loop over inverse matrix columns, rather than rows, to match the AtomType headers
    for (auto col = 0; col < inverseA.nColumns(); ++col)
    {
        line.clear();
        for (auto row = 0; row < inverseA.nRows(); ++row)
        {
            line += fmt::format("{:10f} ", inverseA[{row, col}]);

            // Limit output to sensible length
            if (line.length() >= 80)
            {
                line += " ...";
                break;
            }
        }
        Messenger::print("{}  {}\n", line, data_.at(col).tag());

        // Limit to sensible number of rows
        if (col >= std::max(nColsWritten, 10))
        {
            line.clear();
            for (auto n = 0; n < nColsWritten; ++n)
                line += "    ...    ";
            Messenger::print(line);
            break;
        }
    }
}

// Generate partials from reference data using the inverse coefficients matrix
bool ScatteringMatrix::generatePartials(Array2D<Data1D> &estimatedSQ)
{
    // Check that we have the correct number of reference data to be able to invert the matrix
    if (data_.nItems() < A_.nColumns())
        return Messenger::error("Can't finalise this scattering matrix, since there are not enough reference data ({}) "
                                "compared to rows in the matrix ({}).\n",
                                data_.nItems(), A_.nColumns());

    /*
     * Currently our scattering matrix / data look as follows:
     *
     * [ c11 c12 ... c1N ] [ P1 ]   [ D1 ]
     * [ c21 c22 ... c2N ] [ P2 ] = [ D2 ]		N is number of partials
     * [         ...     ] [ .. ]   [ .. ]		M is number of data (where M >= N)
     * [ cM1 cM2 ... cMN ] [ PN ]   [ DM ]
     *
     * ... where the coefficients in the matrix are the partial weights, P are the (unknown) partial S(Q), and D are the
     * (known) data. The partial weights cMN contain the bound coherent scattering lengths if the row corresponds to a
     * neutron dataset. For xray data, the scattering weights must be added in as a function of Q below.
     *
     * Take the matrix inverse and multiply it by the known data to generate the estimated partials.
     */

    // Template the estimatedSQ from the first data item
    for (auto &n : estimatedSQ)
        n.initialise(data_[0]);

    Array2D<double> inverseA;
    auto qDependentMatrix =
        std::find_if(xRayData_.begin(), xRayData_.end(), [](auto data) { return std::get<0>(data); }) != xRayData_.end();

    if (qDependentMatrix)
    {
        // Generate interpolations for each dataset
        std::vector<Interpolator> interpolations;
        for (auto refDataIndex = 0; refDataIndex < data_.nItems(); ++refDataIndex)
            interpolations.emplace_back(Interpolator(data_[refDataIndex]));

        // Q-dependent terms in the scattering matrix, so need to invert once at each distinct Q value
        const auto &x = estimatedSQ[0].xAxis();
        for (auto n = 0; n < x.size(); ++n)
        {
            const auto q = x[n];

            // Generate inverse matrix at this Q value
            inverseA = matrix(q);
            if (!SVD::pseudoinverse(inverseA))
                return false;

            // Sum in contributions from each dataset at this Q value, provided it is within the range of the dataset
            for (auto partialIndex = 0; partialIndex < A_.nColumns(); ++partialIndex)
            {
                for (auto refDataIndex = 0; refDataIndex < data_.nItems(); ++refDataIndex)
                {
                    if ((q < data_[refDataIndex].xAxis().front()) || (q > data_[refDataIndex].xAxis().back()))
                        continue;
                    estimatedSQ[partialIndex].value(n) +=
                        interpolations[refDataIndex].y(q) * inverseA[{partialIndex, refDataIndex}];
                }
            }
        }
    }
    else
    {
        // No Q-dependent terms in the scattering matrix, so only need to invert once
        inverseA = A_;
        if (!SVD::pseudoinverse(inverseA))
            return false;

        // Generate new partials (nPartials = nColumns)
        for (auto partialIndex = 0; partialIndex < A_.nColumns(); ++partialIndex)
        {
            // Add in contribution from each datset (row).
            for (auto refDataIndex = 0; refDataIndex < data_.nItems(); ++refDataIndex)
                Interpolator::addInterpolated(estimatedSQ[partialIndex], data_[refDataIndex],
                                              inverseA[{partialIndex, refDataIndex}]);
        }
    }

    return true;
}

// Return if the scattering matrix is underdetermined
bool ScatteringMatrix::underDetermined() const { return (data_.nItems() < A_.nColumns()); }

// Return the product of inverseA_ and A_ (which should be the identity matrix) at the specified Q value
Array2D<double> ScatteringMatrix::matrixProduct(double q) const { return inverse(q) * matrix(q); }

/*
 * Construction
 */

// Initialise from supplied list of AtomTypes
void ScatteringMatrix::initialise(const std::vector<std::shared_ptr<AtomType>> &types, Array2D<Data1D> &estimatedSQ)
{
    // Clear coefficients matrix and its inverse_, and empty our typePairs_ and data_ lists
    A_.clear();
    data_.clear();
    typePairs_.clear();

    // Copy atom types
    for_each_pair(types.begin(), types.end(), [this](int i, auto at1, int j, auto at2) { typePairs_.emplace_back(at1, at2); });

    // Create partials array
    estimatedSQ.initialise(types.size(), types.size(), true);
    auto index = 0;
    for (auto [i, j] : typePairs_)
        estimatedSQ[index++].setTag(fmt::format("{}-{}", i->name(), j->name()));
}

// Add reference data with its associated NeutronWeights, applying optional factor to those weights and the data itself
bool ScatteringMatrix::addReferenceData(const Data1D &weightedData, const NeutronWeights &dataWeights, double factor)
{
    // Make sure that the scattering weights are valid
    if (!dataWeights.isValid())
        return Messenger::error("Reference data '{}' does not have valid scattering weights.\n", weightedData.tag());

    // Extend the scattering matrix by one row
    A_.addRow(typePairs_.size());
    const auto rowIndex = A_.nRows() - 1;

    // Set coefficients in A_
    const auto nUsedTypes = dataWeights.nUsedTypes();
    const auto &usedTypes = dataWeights.atomTypes();
    for (auto n = 0; n < nUsedTypes; ++n)
    {
        for (auto m = n; m < nUsedTypes; ++m)
        {
            auto colIndex = pairIndex(usedTypes.atomType(n), usedTypes.atomType(m));
            if (colIndex == -1)
                return Messenger::error("Weights associated to reference data contain one or more unknown AtomTypes "
                                        "('{}' and/or '{}').\n",
                                        usedTypes.atomType(n)->name(), usedTypes.atomType(m)->name());

            // Now have the local column index of the AtomType pair in our matrix A_...
            A_[{rowIndex, colIndex}] = dataWeights.weight(n, m) * factor;
        }
    }

    // Add reference data and apply the associated factor
    data_.add(weightedData);
    data_.last() *= factor;

    // Neutron data, so store dummy XRay form factor data indicator
    xRayData_.emplace_back(false, std::nullopt, StructureFactors::NoNormalisation);

    return true;
}

// Add reference data with its associated XRayWeights, applying optional factor to those weights and the data itself
bool ScatteringMatrix::addReferenceData(const Data1D &weightedData, const XRayWeights &dataWeights, double factor)
{
    // Make sure that the scattering weights are valid
    if (!dataWeights.isValid())
        return Messenger::error("Reference data '{}' does not have valid scattering weights.\n", weightedData.tag());

    // Extend the scattering matrix by one row
    A_.addRow(typePairs_.size());
    const auto rowIndex = A_.nRows() - 1;

    // Set coefficients in A_
    const auto nUsedTypes = dataWeights.nUsedTypes();
    const auto &usedTypes = dataWeights.atomTypes();
    for (int n = 0; n < nUsedTypes; ++n)
    {
        for (int m = n; m < nUsedTypes; ++m)
        {
            auto colIndex = pairIndex(usedTypes.atomType(n), usedTypes.atomType(m));
            if (colIndex == -1)
                return Messenger::error("Weights associated to reference data contain one or more unknown AtomTypes "
                                        "('{}' and/or '{}').\n",
                                        usedTypes.atomType(n)->name(), usedTypes.atomType(m)->name());

            // Now have the local column index of the AtomType pair in our matrix A_.
            // Since this is X-ray data, we will just store the product of the concentrtion weights and the factor
            A_[{rowIndex, colIndex}] = dataWeights.preFactor(n, m) * factor;
        }
    }

    // Add reference data and apply the associated factor
    data_.add(weightedData);
    data_.last() *= factor;

    // Store XRay form factor data indicator
    xRayData_.emplace_back(true, dataWeights, StructureFactors::AverageOfSquaresNormalisation);

    return true;
}

// Add reference partial data between specified AtomTypes, applying optional factor to the weight and the data itself
bool ScatteringMatrix::addPartialReferenceData(Data1D &weightedData, std::shared_ptr<AtomType> at1,
                                               std::shared_ptr<AtomType> at2, double dataWeight, double factor)
{
    // Extend the scattering matrix by one row
    A_.addRow(typePairs_.size());
    const auto rowIndex = A_.nRows() - 1;

    auto colIndex = pairIndex(at1, at2);
    if (colIndex == -1)
        return Messenger::error(
            "Weights associated to reference data contain one or more unknown AtomTypes ('{}' and/or '{}').\n", at1->name(),
            at2->name());

    // Now have the local column index of the AtomType pair in our matrix A_...
    A_.setRow(rowIndex, 0.0);
    A_[{rowIndex, colIndex}] = dataWeight * factor;

    // Add reference data and its associated factor
    data_.add(weightedData);
    data_.last() *= factor;

    // Simulated partial data, so store dummy XRay form factor data indicator
    xRayData_.emplace_back(false, std::nullopt, StructureFactors::NoNormalisation);

    return true;
}
