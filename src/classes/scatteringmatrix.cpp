/*
    *** Scattering Matrix Helper Class
    *** src/classes/scatteringmatrix.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "classes/scatteringmatrix.h"
#include "classes/atomtype.h"
#include "classes/neutronweights.h"
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
double ScatteringMatrix::pairWeightInverse(std::shared_ptr<AtomType> typeI, std::shared_ptr<AtomType> typeJ,
                                           int dataIndex) const
{
    /*
     * The required row of the inverse matrix is the index of the AtomType pair.
     * The required column of the inverse matrix is the original (row) index of the supplied data.
     */

    auto index = pairIndex(typeI, typeJ);
    return inverseA_.constAt(index, dataIndex);
}

// Print the matrix
void ScatteringMatrix::print() const
{
    // Write header
    std::string text, line;
    auto nColsWritten = 0;
    for (auto [i, j] : typePairs_)
    {
        text = fmt::format("{}-{}", i->name(), j->name());
        line = fmt::format("{:10} ", text);

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
    for (int row = 0; row < data_.nItems(); ++row)
    {
        line.clear();
        for (int n = 0; n < A_.nColumns(); ++n)
        {
            line += fmt::format("{:10f} ", A_.constAt(row, n));

            // Limit output to sensible length
            if (line.length() >= 80)
            {
                line += " ...";
                break;
            }
        }
        Messenger::print("{}  {}\n", line, data_.constAt(row).name());

        // Limit to sensible number of rows
        if (row >= std::max(nColsWritten, 10))
        {
            line.clear();
            for (int n = 0; n < nColsWritten; ++n)
                line += "    ...    ";
            Messenger::print("{}\n", line);
            break;
        }
    }
}

// Print the inverse matrix
void ScatteringMatrix::printInverse() const
{
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
    for (auto col = 0; col < inverseA_.nColumns(); ++col)
    {
        line.clear();
        for (auto row = 0; row < inverseA_.nRows(); ++row)
        {
            line += fmt::format("{:10f} ", inverseA_.constAt(row, col));

            // Limit output to sensible length
            if (line.length() >= 80)
            {
                line += " ...";
                break;
            }
        }
        Messenger::print("{}  {}\n", line, data_.constAt(col).name());

        // Limit to sensible number of rows
        if (col >= std::max(nColsWritten, 10))
        {
            line.clear();
            for (int n = 0; n < nColsWritten; ++n)
                line += "    ...    ";
            Messenger::print(line);
            break;
        }
    }
}

// Generate partials from reference data using the inverse coefficients matrix
void ScatteringMatrix::generatePartials(Array2D<Data1D> &estimatedSQ)
{
    /*
     * Currently our scattering matrix / data look as follows:
     *
     * [ c11 c12 ... c1N ] [ P1 ]   [ D1 ]
     * [ c21 c22 ... c2N ] [ P2 ] = [ D2 ]		N is number of partials
     * [         ...     ] [ .. ]   [ .. ]		M is number of data (where M >= N)
     * [ cM1 cM2 ... cMN ] [ PN ]   [ DM ]
     *
     * ... where the coefficients in the matrix are the partial weights, P are the (unknown) partial S(Q), and D are the
     * (known) data.
     *
     * Take the matrix inverse and multiply it by the known data to generate the estimated partials.
     */

    // Get linear array from estimatedSQ
    Data1D *partials = estimatedSQ.linearArray();

    // Clear current partials
    for (int n = 0; n < estimatedSQ.linearArraySize(); ++n)
        partials[n].clear();

    // Generate new partials (nPartials = nColumns)
    for (int n = 0; n < A_.nColumns(); ++n)
    {
        // Add in contribution from each datset (row).
        for (int m = 0; m < data_.nItems(); ++m)
        {
            Interpolator::addInterpolated(partials[n], data_[m], inverseA_.constAt(n, m));
        }
    }
}

// Return if the scattering matrix is underdetermined
bool ScatteringMatrix::underDetermined() const { return (data_.nItems() < A_.nColumns()); }

// Return the product of inverseA_ and A_ (which should be the identity matrix)
Array2D<double> ScatteringMatrix::matrixProduct() const { return inverseA_ * A_; }

/*
 * Construction
 */

// Initialise from supplied list of AtomTypes
void ScatteringMatrix::initialise(const std::vector<std::shared_ptr<AtomType>> &types, Array2D<Data1D> &estimatedSQ,
                                  std::string_view objectNamePrefix, std::string_view groupName)
{
    // Clear coefficients matrix and its inverse_, and empty our typePairs_ and data_ lists
    A_.clear();
    inverseA_.clear();
    data_.clear();
    typePairs_.clear();

    // Copy atom types
    for_each_pair(types.begin(), types.end(), [this](int i, auto at1, int j, auto at2) { typePairs_.emplace_back(at1, at2); });

    // Create partials array
    estimatedSQ.initialise(types.size(), types.size(), true);
    Data1D *partials = estimatedSQ.linearArray();
    auto index = 0;
    for (auto [i, j] : typePairs_)
    {
        partials[index].setName(fmt::format("EstimatedSQ-{}-{}-{}.sq", i->name(), j->name(), groupName));
        partials[index].setObjectTag(
            fmt::format("{}//EstimatedSQ//{}//{}-{}", objectNamePrefix, groupName, i->name(), j->name()));
        ++index;
    }
}

// Finalise
bool ScatteringMatrix::finalise()
{
    // Check that we have the correct number of reference data to be able to invert the matrix
    if (data_.nItems() < A_.nColumns())
    {
        Messenger::error("Can't finalise this scattering matrix, since there are not enough reference data ({}) "
                         "compared to rows in the matrix ({}).\n",
                         data_.nItems(), A_.nColumns());
        return false;
    }

    inverseA_ = A_;
    if (!SVD::pseudoinverse(inverseA_))
        return false;

    return true;
}

// Add reference data
bool ScatteringMatrix::addReferenceData(const Data1D &weightedData, NeutronWeights &dataWeights, double factor)
{
    // Make sure that the scattering weights are valid
    if (!dataWeights.isValid())
        return Messenger::error("Reference data '{}' does not have valid scattering weights.\n", weightedData.name());

    // Extend the scattering matrix by one row
    A_.addRow(typePairs_.size());
    const auto rowIndex = A_.nRows() - 1;

    // Set coefficients in A_
    const auto nUsedTypes = dataWeights.nUsedTypes();
    AtomTypeList &usedTypes = dataWeights.atomTypes();
    for (int n = 0; n < nUsedTypes; ++n)
    {
        for (int m = n; m < nUsedTypes; ++m)
        {
            auto colIndex = pairIndex(usedTypes.atomType(n), usedTypes.atomType(m));
            if (colIndex == -1)
            {
                Messenger::error("Weights associated to reference data contain one or more unknown AtomTypes "
                                 "('{}' and/or '{}').\n",
                                 usedTypes.atomType(n)->name(), usedTypes.atomType(m)->name());
                return false;
            }

            // Now have the local column index of the AtomType pair in our matrix A_...
            A_.at(rowIndex, colIndex) = dataWeights.weight(n, m) * factor;
        }
    }

    // Add reference data and its associated factor
    data_.add(weightedData);
    data_.last().values() *= factor;

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
    {
        Messenger::error("Weights associated to reference data contain one or more unknown AtomTypes ('{}' and/or '{}').\n",
                         at1->name(), at2->name());
        return false;
    }

    // Now have the local column index of the AtomType pair in our matrix A_...
    A_.setRow(rowIndex, 0.0);
    A_.at(rowIndex, colIndex) = dataWeight * factor;

    // Add reference data and its associated factor
    data_.add(weightedData);
    data_.last().values() *= factor;

    return true;
}
