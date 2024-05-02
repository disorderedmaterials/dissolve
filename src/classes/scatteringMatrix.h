// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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

// Scattering Matrix Container
class ScatteringMatrix
{
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
    std::vector<std::shared_ptr<AtomType>> atomTypes_;
    // Reference pairs of AtomTypes
    std::vector<std::tuple<std::shared_ptr<AtomType>, std::shared_ptr<AtomType>>> typePairs_;
    // Coefficients matrix (A) (ci * cj * bi * bj * (typei == typej ? 1 : 2)) (n * n)
    Array2D<double> A_;
    // Reference data (B) (n * 1)
    std::vector<Data1D> data_;
    // X-ray specification for reference data (if relevant)
    std::vector<std::tuple<bool, std::optional<XRayWeights>, StructureFactors::NormalisationType>> xRayData_;
    // Scattering matrix and inverse at Q = 0
    Array2D<double> qZeroMatrix_, qZeroInverse_;
    // Scattering matrix / inverse pairs at specific Q values
    std::vector<std::tuple<double, Array2D<double>, Array2D<double>>> qMatrices_;

    private:
    // Return whether Q-dependent weighting is required
    bool qDependentWeighting() const;

    public:
    // Return number of AtomTypes involved
    int nAtomTypes() const;
    // Return atom types
    const std::vector<std::shared_ptr<AtomType>> &atomTypes() const;
    // Return atom type at index specified
    std::shared_ptr<AtomType> atomType(int index) const;
    // Return index of atom type in our local vector
    int indexOf(const std::shared_ptr<AtomType> &typeI) const;
    // Return index pair of atom types in our local vector
    std::tuple<int, int> pairIndexOf(const std::shared_ptr<AtomType> &typeI, const std::shared_ptr<AtomType> &typeJ) const;
    // Return column of specified AtomType pair
    int columnIndex(const std::shared_ptr<AtomType> &typeI, const std::shared_ptr<AtomType> &typeJ) const;
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
    // Generate partials from reference data using inverse matrix
    bool generatePartials(Array2D<Data1D> &estimatedSQ);
    // Return the product of inverseA_ and A_ (which should be the identity matrix) at the specified Q value
    Array2D<double> matrixProduct(double q = 0.0) const;

    /*
     * Construction
     */
    public:
    // Initialise from supplied list of AtomTypes
    void initialise(const AtomTypeMix &typeMix, Array2D<Data1D> &estimatedSQ);
    // Add reference data with its associated NeutronWeights, applying optional factor to those weights and the data itself
    bool addReferenceData(const Data1D &weightedData, const NeutronWeights &dataWeights, double factor = 1.0);
    // Add reference data with its associated XRayWeights, applying optional factor to those weights and the data itself
    bool addReferenceData(const Data1D &weightedData, const XRayWeights &dataWeights, double factor = 1.0);
    // Update reference data)
    bool updateReferenceData(const Data1D &weightedData, double factor = 1.0);
    // Add reference partial data between specified AtomTypes, applying optional factor to the weight and the data itself
    bool addPartialReferenceData(Data1D &weightedData, const std::shared_ptr<AtomType> &at1,
                                 const std::shared_ptr<AtomType> &at2, double dataWeight, double factor = 1.0);
    // Return number of currently-defined reference data sets (== matrix rows)
    int nReferenceData() const;
};
