// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/scatteringMatrix.h"
#include "classes/atomType.h"
#include "classes/neutronWeights.h"
#include "classes/xRayWeights.h"
#include "math/interpolator.h"
#include "math/svd.h"
#include "templates/algorithms.h"
#include <algorithm>
#include <utility>

/*
 * Data
 */

// Return whether Q-dependent weighting is required
bool ScatteringMatrix::qDependentWeighting() const
{
    return std::find_if(xRayData_.begin(), xRayData_.end(), [](auto data) { return std::get<0>(data); }) != xRayData_.end();
}

// Return number of atom types involved
int ScatteringMatrix::nAtomTypes() const { return atomTypes_.size(); }

// Return atom types
const std::vector<std::shared_ptr<AtomType>> &ScatteringMatrix::atomTypes() const { return atomTypes_; }

// Return atom type at index specified
std::shared_ptr<AtomType> ScatteringMatrix::atomType(int index) const { return atomTypes_[index]; }

// Return index of atom type in our local vector
int ScatteringMatrix::indexOf(const std::shared_ptr<AtomType> &typeI) const
{
    auto it = std::find(atomTypes_.begin(), atomTypes_.end(), typeI);
    assert(it != atomTypes_.end());
    return it - atomTypes_.begin();
}

// Return index pair of atom types in our local vector
std::tuple<int, int> ScatteringMatrix::pairIndexOf(const std::shared_ptr<AtomType> &typeI,
                                                   const std::shared_ptr<AtomType> &typeJ) const
{
    return {indexOf(typeI), indexOf(typeJ)};
}

// Return column index of specified AtomType pair
int ScatteringMatrix::columnIndex(const std::shared_ptr<AtomType> &typeI, const std::shared_ptr<AtomType> &typeJ) const
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

// Generate matrices
void ScatteringMatrix::generateMatrices()
{
    // We always generate the matrices for Q = 0
    Messenger::printVerbose("Generating Q = 0.0 matrix and inverse.\n");
    qZeroMatrix_ = matrix(0.0);
    qZeroInverse_ = qZeroMatrix_;
    if (!SVD::pseudoinverse(qZeroInverse_))
        throw(std::runtime_error("Failed to invert the scattering matrix at Q = 0.0.\n"));

    // Generate Q-dependent matrices if we need them
    qMatrices_.clear();
    if (qDependentWeighting())
    {
        // Use the first reference data as the Q-axis template (as is done elsewhere)
        assert(!data_.empty());
        auto &qs = data_[0].xAxis();
        qMatrices_.reserve(qs.size());
        for (auto q : qs)
        {
            Messenger::printVerbose("Generating Q = {} matrix and inverse.\n", q);

            auto &&[qValue, mat, inv] = qMatrices_.emplace_back();
            qValue = q;
            mat = matrix(q);
            inv = mat;
            if (!SVD::pseudoinverse(inv))
                throw(std::runtime_error(fmt::format("Failed to invert the scattering matrix at Q = {}.\n", q)));
        }
    }
}

// Return the precalculated Q = 0.0 scattering matrix inverse
const Array2D<double> &ScatteringMatrix::qZeroMatrixInverse() const { return qZeroInverse_; }

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
    for (auto row = 0; row < data_.size(); ++row)
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
    if (data_.size() < A_.nColumns())
        return Messenger::error("Can't finalise this scattering matrix, since there are not enough reference data ({}) "
                                "compared to rows in the matrix ({}).\n",
                                data_.size(), A_.nColumns());

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
    for (auto &estSQ : estimatedSQ)
        estSQ.initialise(data_[0]);

    if (qDependentWeighting())
    {
        // Generate interpolations for each dataset
        std::vector<Interpolator> interpolations;
        interpolations.reserve(data_.size());
        for (const auto &ref : data_)
            interpolations.emplace_back(Interpolator(ref));

        // Q-dependent terms in the scattering matrix, so need to invert once at each distinct Q value
        const auto &x = estimatedSQ[0].xAxis();
        for (auto n = 0; n < x.size(); ++n)
        {
            const auto q = x[n];

            // Grab the pre-calculated scattering matrix
            auto &inverseA = std::get<2>(qMatrices_[n]);

            // Sum in contributions from each dataset at this Q value, provided it is within the range of the dataset
            for (auto partialIndex = 0; partialIndex < A_.nColumns(); ++partialIndex)
            {
                for (auto refDataIndex = 0; refDataIndex < data_.size(); ++refDataIndex)
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
        // Generate new partials (nPartials = nColumns)
        for (auto refDataIndex = 0; refDataIndex < data_.size(); ++refDataIndex)
        {
            // Generate interpolation for this dataset (row).
            Interpolator I(data_[refDataIndex]);

            for (auto partialIndex = 0; partialIndex < A_.nColumns(); ++partialIndex)
                Interpolator::addInterpolated(I, estimatedSQ[partialIndex], qZeroInverse_[{partialIndex, refDataIndex}]);
        }
    }

    return true;
}

// Return the product of inverseA_ and A_ (which should be the identity matrix) at the specified Q value
Array2D<double> ScatteringMatrix::matrixProduct(double q) const { return inverse(q) * matrix(q); }

/*
 * Construction
 */

// Initialise from supplied list of AtomTypes
void ScatteringMatrix::initialise(const AtomTypeMix &typeMix, Array2D<Data1D> &estimatedSQ)
{
    // Clear coefficients matrix and its inverse_, and empty our typePairs_ and data_ lists
    A_.clear();
    data_.clear();
    atomTypes_.clear();
    typePairs_.clear();
    qZeroMatrix_.clear();
    qZeroInverse_.clear();
    qMatrices_.clear();

    // Copy atom types and construct pairs
    atomTypes_.resize(typeMix.nItems());
    std::transform(typeMix.begin(), typeMix.end(), atomTypes_.begin(), [](const auto &atd) { return atd.atomType(); });
    dissolve::for_each_pair(ParallelPolicies::seq, atomTypes_.begin(), atomTypes_.end(),
                            [this](int i, auto &at1, int j, auto &at2) { typePairs_.emplace_back(at1, at2); });

    // Create partials array
    estimatedSQ.initialise(atomTypes_.size(), atomTypes_.size(), true);
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
            auto colIndex = columnIndex(usedTypes.atomType(n), usedTypes.atomType(m));
            if (colIndex == -1)
                return Messenger::error("Weights associated to reference data contain one or more unknown AtomTypes "
                                        "('{}' and/or '{}').\n",
                                        usedTypes.atomType(n)->name(), usedTypes.atomType(m)->name());

            // Now have the local column index of the AtomType pair in our matrix A_...
            A_[{rowIndex, colIndex}] = dataWeights.weight(n, m) * factor;
        }
    }

    // Add reference data and apply the associated factor
    data_.emplace_back(weightedData) *= factor;

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
    const auto &usedTypes = dataWeights.atomTypeMix();
    for (int n = 0; n < nUsedTypes; ++n)
    {
        for (int m = n; m < nUsedTypes; ++m)
        {
            auto colIndex = columnIndex(usedTypes.atomType(n), usedTypes.atomType(m));
            if (colIndex == -1)
                return Messenger::error("Weights associated to reference data contain one or more unknown AtomTypes "
                                        "('{}' and/or '{}').\n",
                                        usedTypes.atomType(n)->name(), usedTypes.atomType(m)->name());

            // Now have the local column index of the AtomType pair in our matrix A_.
            // Since this is X-ray data, we will just store the product of the concentration weights and the factor
            A_[{rowIndex, colIndex}] = dataWeights.preFactor(n, m) * factor;
        }
    }

    // Add reference data and apply the associated factor
    data_.emplace_back(weightedData) *= factor;

    // Store XRay form factor data indicator
    xRayData_.emplace_back(true, dataWeights, StructureFactors::AverageOfSquaresNormalisation);

    return true;
}

// Update reference data)
bool ScatteringMatrix::updateReferenceData(const Data1D &weightedData, double factor)
{
    auto it = std::find_if(data_.begin(), data_.end(), [weightedData](auto &data) { return weightedData.tag() == data.tag(); });
    if (it == data_.end())
        return false;

    *it = weightedData;
    *it *= factor;

    return true;
}

// Add reference partial data between specified AtomTypes, applying optional factor to the weight and the data itself
bool ScatteringMatrix::addPartialReferenceData(Data1D &weightedData, const std::shared_ptr<AtomType> &at1,
                                               const std::shared_ptr<AtomType> &at2, double dataWeight, double factor)
{
    // Extend the scattering matrix by one row
    A_.addRow(typePairs_.size());
    const auto rowIndex = A_.nRows() - 1;

    auto colIndex = columnIndex(at1, at2);
    if (colIndex == -1)
        return Messenger::error(
            "Weights associated to reference data contain one or more unknown AtomTypes ('{}' and/or '{}').\n", at1->name(),
            at2->name());

    // Now have the local column index of the AtomType pair in our matrix A_...
    A_.setRow(rowIndex, 0.0);
    A_[{rowIndex, colIndex}] = dataWeight * factor;

    // Add reference data and its associated factor
    data_.emplace_back(weightedData) *= factor;

    // Simulated partial data, so store dummy XRay form factor data indicator
    xRayData_.emplace_back(false, std::nullopt, StructureFactors::NoNormalisation);

    return true;
}

// Return number of currently-defined reference data sets (== matrix rows)
int ScatteringMatrix::nReferenceData() const { return A_.nRows(); }
