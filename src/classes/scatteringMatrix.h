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

// Scattering Matrix Row
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
    // Scattering matrix rows
    KeyedVector<std::string, ScatteringMatrixRow> rows_;
    // Scattering matrix and inverse at Q = 0
    Array2D<double> qZeroMatrix_, qZeroInverse_;
    // Q values to use when generating matrices and data
    std::vector<double> qValues_;
    // Scattering matrix / inverse at specific Q values
    std::vector<Array2D<double>> qMatrices_, qInverses_;

    private:
    // Return whether Q-dependent weighting is required
    bool qDependentWeighting() const;
    // Create and return the full coefficients matrix
    Array2D<double> A() const;

    public:
    // Generate matrices
    void generateMatrices(const std::vector<double> &qValues);
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
    void setRow(const std::string &key, const Data1D &data, const NeutronWeights &weights, double factor);
    // Set data and coefficients for the supplied row (from XRayWeights)
    void setRow(const std::string &key, const Data1D &data, const XRayWeights &weights, double factor);
    // Set data and coefficients for the supplied row (single coefficient)
    void setRow(const std::string &key, const Data1D &data, const AtomType *i, const AtomType *j, double factor);
};
