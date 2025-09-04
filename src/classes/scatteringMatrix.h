// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/neutronWeights.h"
#include "classes/xRayWeights.h"
#include "data/formFactors.h"
#include "data/structureFactors.h"
#include "math/data1D.h"
#include "templates/array2D.h"
#include <memory>
#include <tuple>
#include <vector>

// Forward Declarations
class AtomType;

struct ScatteringMatrixRow
{
    Data1D data;
    std::vector<double> coefficients;
    std::optional<XRayWeights> xRayWeights;
    StructureFactors::NormalisationType xRayNormalisation;
};

// Scattering Matrix Container
class ScatteringMatrix
{
    public:
    ScatteringMatrix(const std::vector<const AtomType *> &atomTypes);

    /*
     * Data
     *
     * Representing: AX = B
     *
     * 	[  1,1 ... 1,n ] [ x1 ]   [ B2 ]
     * 	[  2,1     2,n ] [ x2 ] = [ B2 ]
     * 	[  n,1     n,n ] [ xn ]   [ Bn ]
     */
    private:
    // Source AtomTypes involved
    std::vector<const AtomType *> atomTypes_;
    // Reference pairs of AtomTypes
    std::vector<std::pair<const AtomType *, const AtomType *>> typePairs_;
    // Scattering matrix rows
    DoubleKeyedMap<ScatteringMatrixRow> rows_;
    // Scattering matrix and inverse at Q = 0
    Array2D<double> qZeroMatrix_, qZeroInverse_;
    // Scattering matrix / inverse pairs at specific Q values
    std::vector<std::tuple<double, Array2D<double>, Array2D<double>>> qMatrices_;
    // Whether the matrices up-to-date
    bool matricesValid_{false};

    private:
    // Return whether Q-dependent weighting is required
    bool qDependentWeighting() const;
    // Create and return the full coefficients matrix
    Array2D<double> A() const;

    public:
    // Return number of AtomTypes involved
    int nAtomTypes() const;
    // Return atom types
    const std::vector<const AtomType *> &atomTypes() const;
    // Return atom type at index specified
    const AtomType *atomType(int index) const;
    // Return index of atom type in our local vector
    int indexOf(const AtomType *typeI) const;
    // Return index pair of atom types in our local vector
    std::pair<int, int> pairIndexOf(const AtomType *typeI, const AtomType *typeJ) const;
    // Return column of specified AtomType pair
    int columnIndex(const AtomType *typeI, const AtomType *typeJ) const;
    // Generate matrices
    void generateMatrices();
    // Return the precalculated Q = 0.0 scattering matrix inverse
    const Array2D<double> &qZeroMatrixInverse() const;
    // Calculate and return the scattering matrix at the specified Q value
    Array2D<double> matrix(double q = 0.0) const;
    // Calculate and return the inverse matrix at the specified Q value
    Array2D<double> inverse(double q = 0.0) const;
    // Print the scattering coefficients matrix at the specified Q value
    void print(double q = 0.0) const;
    // Print the inverse matrix at the specified Q value
    void printInverse(double q = 0.0) const;
    // Generate estimated partials from reference data using the inverse coefficients matrix
    DoubleKeyedMap<Data1D> generateEstimatedPartials() const;
    // Return the product of inverseA_ and A_ (which should be the identity matrix) at the specified Q value
    Array2D<double> matrixProduct(double q = 0.0) const;

    /*
     * Construction
     */
    public:
    // Set data and coefficients for the supplied row (from NeutronWeights)
    void setRow(DoubleKeyedMapKey key, const Data1D &data, const NeutronWeights &weights, double factor);
    // Set data and coefficients for the supplied row (from XRayWeights)
    void setRow(DoubleKeyedMapKey key, const Data1D &data, const XRayWeights &weights, double factor);
    // Set data and coefficients for the supplied row (single coefficient)
    void setRow(DoubleKeyedMapKey key, const Data1D &data, const AtomType *i, const AtomType *j, double factor);
};
