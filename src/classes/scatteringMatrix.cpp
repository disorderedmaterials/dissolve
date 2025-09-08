// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/scatteringMatrix.h"
#include "classes/atomType.h"
#include "classes/neutronWeights.h"
#include "classes/xRayWeights.h"
#include "math/interpolator.h"
#include "math/svd.h"
#include "templates/algorithms.h"
#include <algorithm>
#include <utility>

ScatteringMatrix::ScatteringMatrix(const std::vector<const AtomType *> &atomTypes)
{
    atomTypes_ = atomTypes;
    rows_.clear(true);
    matricesValid_ = false;
}

/*
 * Data
 */

// Return whether Q-dependent weighting is required
bool ScatteringMatrix::qDependentWeighting() const
{
    return std::find_if(rows_.begin(), rows_.end(), [](auto &row) { return row.second.xRayWeights; }) != rows_.end();
}

// Return number of atom types involved
int ScatteringMatrix::nAtomTypes() const { return atomTypes_.size(); }

// Return atom types
const std::vector<const AtomType *> &ScatteringMatrix::atomTypes() const { return atomTypes_; }

// Return atom type at index specified
const AtomType *ScatteringMatrix::atomType(int index) const { return atomTypes_[index]; }

// Return index of atom type in our local vector
int ScatteringMatrix::indexOf(const AtomType *typeI) const
{
    auto it = std::find(atomTypes_.begin(), atomTypes_.end(), typeI);
    assert(it != atomTypes_.end());
    return it - atomTypes_.begin();
}

// Return index pair of atom types in our local vector
std::pair<int, int> ScatteringMatrix::pairIndexOf(const AtomType *typeI, const AtomType *typeJ) const
{
    return {indexOf(typeI), indexOf(typeJ)};
}

// Return column index of specified AtomType pair
int ScatteringMatrix::columnIndex(const AtomType *typeI, const AtomType *typeJ) const
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

// Create and return the full coefficients matrix
Array2D<double> ScatteringMatrix::A() const
{
    Array2D<double> result(rows_.size(), (atomTypes_.size() * (atomTypes_.size() + 1)) / 2, false);
    auto row = 0;
    for (const auto &[rowKey, rowData] : rows_)
    {
        for (auto col = 0; col < rowData.coefficients.size(); ++col)
            result[{row, col}] = rowData.coefficients[col];
        ++row;
    }

    return result;
}

// Generate matrices
void ScatteringMatrix::generateMatrices()
{
    if (matricesValid_)
        return;

    // We always generate the matrices for Q = 0
    Messenger::printVerbose("Generating Q = 0.0 matrix and inverse.\n");
    qZeroMatrix_ = matrix(0.0);
    qZeroInverse_ = qZeroMatrix_;
    if (!SVD::pseudoinverse(qZeroInverse_))
        Messenger::exception("Failed to invert the scattering matrix at Q = 0.0.");

    // Generate Q-dependent matrices if we need them
    qMatrices_.clear();
    if (qDependentWeighting())
    {
        // Use the first reference data as the Q-axis template (as is done elsewhere)
        assert(!rows_.empty());
        auto &qs = rows_.begin()->second.data.xAxis();
        qMatrices_.reserve(qs.size());
        for (auto q : qs)
        {
            Messenger::printVerbose("Generating Q = {} matrix and inverse.\n", q);

            auto &&[qValue, mat, inv] = qMatrices_.emplace_back();
            qValue = q;
            mat = matrix(q);
            inv = mat;
            if (!SVD::pseudoinverse(inv))
                Messenger::exception("Failed to invert the scattering matrix at Q = {}.\n", q);
        }
    }

    matricesValid_ = true;
}

// Return the precalculated Q = 0.0 scattering matrix inverse
const Array2D<double> &ScatteringMatrix::qZeroMatrixInverse() const { return qZeroInverse_; }

// Calculate and return the scattering matrix at the specified Q value
Array2D<double> ScatteringMatrix::matrix(double q) const
{
    // Start with the plain scattering matrix coefficients
    auto m = A();

    // Check rows to see if any are xray-weighted
    auto row = 0;
    for (const auto &[rowKey, rowData] : rows_)
    {
        if (rowData.xRayWeights)
        {
            // Grab the weights and normalisation to apply to the matrix elements
            auto &xRayWeights = *rowData.xRayWeights;
            auto normFactor = 1.0;
            if (rowData.xRayNormalisation == StructureFactors::AverageOfSquaresNormalisation)
                normFactor = xRayWeights.boundCoherentAverageOfSquares(q);
            else if (rowData.xRayNormalisation == StructureFactors::SquareOfAverageNormalisation)
                normFactor = xRayWeights.boundCoherentSquareOfAverage(q);

            // Loop over columns and get weights for columns according to the elements of the atom type pairs
            std::vector<double> weights;
            dissolve::for_each_pair(ParallelPolicies::seq, atomTypes_, [&](int i, auto &atI, int j, auto &atJ)
                                    { weights.push_back(xRayWeights.formFactorProduct(atI, atJ, q) / normFactor); });

            // Apply the weights
            for (auto col = 0; col < weights.size(); ++col)
                m[{row, col}] *= weights[col];
        }

        ++row;
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
        text = std::format("{}-{}", i->name(), j->name());
        line += std::format("{:^10} ", text);

        // Limit output to sensible length
        if (line.length() >= 80)
        {
            line += " ...";
            break;
        }

        ++nColsWritten;
    }
    Messenger::print("{}", line);

    // Loop over rows
    auto row = 0;
    for (const auto &[rowKey, rowData] : rows_)
    {
        line.clear();
        for (auto n = 0; n < m.nColumns(); ++n)
        {
            line += std::format("{:10f} ", m[{row, n}]);

            // Limit output to sensible length
            if (line.length() >= 80)
            {
                line += " ...";
                break;
            }
        }
        Messenger::print("{}  {}\n", line, rowKey);
        ++row;

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
        line += std::format("{:10} ", std::format("{}-{}", i->name(), j->name()));

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
    auto col = 0;
    for (const auto &[rowKey, rowData] : rows_)
    {
        line.clear();
        for (auto row = 0; row < inverseA.nRows(); ++row)
        {
            line += std::format("{:10f} ", inverseA[{row, col}]);

            // Limit output to sensible length
            if (line.length() >= 80)
            {
                line += " ...";
                break;
            }
        }
        Messenger::print("{}  {}\n", line, rowKey);

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

// Generate estimated partials from reference data using the inverse coefficients matrix
DoubleKeyedMap<Data1D> ScatteringMatrix::generateEstimatedPartials() const
{
    // Check that we have the correct number of reference data to be able to invert the matrix
    if (rows_.size() < (atomTypes_.size() * (atomTypes_.size() + 1)) / 2)
        return Messenger::error("Can't finalise this scattering matrix, since there are not enough reference data ({}) "
                                "compared to rows in the matrix ({}).\n",
                                rows_.size(), (atomTypes_.size() * (atomTypes_.size() + 1)) / 2);

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

    DoubleKeyedMap<Data1D> estimatedPartials;

    // Template the estimated partials from the first data item
    dissolve::for_each_pair(ParallelPolicies::seq, atomTypes_,
                            [&](int indexI, const auto &typeI, int indexJ, const auto &typeJ)
                            {
                                estimatedPartials[{typeI->name(), typeJ->name()}].setTag(
                                    std::format("{}-{}", typeI->name(), typeJ->name()));
                                estimatedPartials[{typeI->name(), typeJ->name()}].initialise(rows_.begin()->second.data);
                            });

    auto qDependentWeights = qDependentWeighting();

    // Loop over reference data
    auto dataIndex = 0;
    for (const auto &[rowKey, rowData] : rows_)
    {
        // Generate interpolation for this dataset (row).
        Interpolator I(rowData.data);

        // Loop over columns (atom-atom partials)
        auto partialIndex = 0;
        dissolve::for_each_pair(ParallelPolicies::seq, atomTypes_,
                                [&](int indexI, const auto &typeI, int indexJ, const auto &typeJ)
                                {
                                    // Get estimated partial
                                    auto &partial = estimatedPartials.get(typeI->name(), typeJ->name());

                                    // Loop over Q points
                                    const auto &qAxis = partial.xAxis();
                                    for (auto qIndex = 0; qIndex < qAxis.size(); ++qIndex)
                                    {
                                        partial.value(qIndex) +=
                                            I.y(qAxis[qIndex]) *
                                            (qDependentWeights ? std::get<2>(qMatrices_[qIndex])[{partialIndex, dataIndex}]
                                                               : qZeroInverse_[{partialIndex, dataIndex}]);
                                    }
                                });
    }

    return estimatedPartials;
}

// Return the product of inverseA_ and A_ (which should be the identity matrix) at the specified Q value
Array2D<double> ScatteringMatrix::matrixProduct(double q) const { return inverse(q) * matrix(q); }

/*
 * Construction
 */

// Set data and coefficients for the supplied row (from NeutronWeights)
void ScatteringMatrix::setRow(DoubleKeyedMapKey key, const Data1D &data, const NeutronWeights &weights, double factor)
{
    auto &row = rows_[key];

    row.data = data;
    row.data *= factor;
    row.coefficients.clear();
    dissolve::for_each_pair(ParallelPolicies::seq, atomTypes_, [&](int indexI, auto &atI, int indexJ, auto &atJ)
                            { row.coefficients.push_back(weights.weights().get({atI->name(), atJ->name()}) * factor); });
}

// Set data and coefficients for the supplied row (from XRayWeights)
void ScatteringMatrix::setRow(DoubleKeyedMapKey key, const Data1D &data, const XRayWeights &weights, double factor)
{
    auto &row = rows_[key];

    row.data = data;
    row.data *= factor;

    row.coefficients.clear();
    dissolve::for_each_pair(ParallelPolicies::seq, atomTypes_, [&](int indexI, auto &atI, int indexJ, auto &atJ)
                            { row.coefficients.push_back(weights.preFactors().get({atI->name(), atJ->name()}) * factor); });

    row.xRayWeights = weights;
    row.xRayNormalisation = StructureFactors::AverageOfSquaresNormalisation;
}

// Set data and coefficients for the supplied row (single coefficient)
void ScatteringMatrix::setRow(DoubleKeyedMapKey key, const Data1D &data, const AtomType *i, const AtomType *j, double factor)
{
    auto &row = rows_[key];

    row.data = data;
    row.data *= factor;

    row.coefficients.clear();
    dissolve::for_each_pair(ParallelPolicies::seq, atomTypes_, [&](int indexI, auto &atI, int indexJ, auto &atJ)
                            { row.coefficients.push_back((atI == i && atJ == j) || (atI == j && atJ == i) ? factor : 0.0); });
}
