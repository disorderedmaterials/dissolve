// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "math/data1d.h"
#include "templates/array2d.h"
#include <memory>
#include <tuple>
#include <vector>

// Forward Declarations
class AtomType;
class NeutronWeights;

// Scattering Matrix Container
class ScatteringMatrix
{
    public:
    ScatteringMatrix();

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
    // Reference pairs of AtomTypes
    std::vector<std::tuple<std::shared_ptr<AtomType>, std::shared_ptr<AtomType>>> typePairs_;
    // Coefficients matrix (A) (ci * cj * bi * bj * (typei == typej ? 1 : 2)) (n * n)
    Array2D<double> A_;
    // Inverse of coefficients matrix
    Array2D<double> inverseA_;
    // Reference data (B) (n * 1)
    Array<Data1D> data_;

    public:
    // Return number of reference AtomType pairs
    int nPairs() const;
    // Return index of specified AtomType pair
    int pairIndex(std::shared_ptr<AtomType> typeI, std::shared_ptr<AtomType> typeJ) const;
    // Return weight of the specified AtomType pair in the inverse matrix
    double pairWeightInverse(std::shared_ptr<AtomType> typeI, std::shared_ptr<AtomType> typeJ, int dataIndex) const;
    // Print the matrix
    void print() const;
    // Print the inverse matrix
    void printInverse() const;
    // Generate partials from reference data using inverse matrix
    void generatePartials(Array2D<Data1D> &estimatedSQ);
    // Return if the scattering matrix is underdetermined
    bool underDetermined() const;
    // Return the product of inverseA_ and A_ (which should be the identity matrix)
    Array2D<double> matrixProduct() const;

    /*
     * Construction
     */
    public:
    // Initialise from supplied list of AtomTypes
    void initialise(const std::vector<std::shared_ptr<AtomType>> &types, Array2D<Data1D> &estimatedSQ,
                    std::string_view objectNamePrefix, std::string_view groupName);
    // Finalise
    bool finalise();
    // Add reference data with its associated Weights, applying optional factor to those weights and the data itself
    bool addReferenceData(const Data1D &weightedData, NeutronWeights &dataWeights, double factor = 1.0);
    // Add reference partial data between specified AtomTypes, applying optional factor to the weight and the data itself
    bool addPartialReferenceData(Data1D &weightedData, std::shared_ptr<AtomType> at1, std::shared_ptr<AtomType> at2,
                                 double dataWeight, double factor = 1.0);
};
