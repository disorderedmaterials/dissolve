// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/svd.h"
#include "templates/algorithms.h"
#include "templates/array2D.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class SVDTest : public ::testing::Test
{
    public:
    SVDTest() = default;

    void testUnitary(const Array2D<double> &A)
    {
        // Take the transpose of the supplied matrix, multiply, and check for the identity matrix
        auto B = A.transposed();
        EXPECT_EQ(A.nRows(), B.nColumns());
        EXPECT_EQ(B.nRows(), A.nColumns());
        auto C = B * A;
        C.print("A * At (=Identity)");
        EXPECT_EQ(C.nRows(), C.nColumns());
        EXPECT_EQ(C.nRows(), B.nRows());
        EXPECT_EQ(C.nColumns(), A.nColumns());
        PairIterator iter(C.nRows());
        for (auto &&[i, j] : iter)
        {
            auto x = C[{i, j}];
            EXPECT_NEAR(x, i == j ? 1.0 : 0.0, 1.0e-10);
        }
    }
    void testDecomposition(const Array2D<double> &A)
    {
        Array2D<double> U, V, S;

        EXPECT_TRUE(SVD::decompose(A, U, S, V));
        U.print("U");
        S.print("S");
        V.print("V");

        // Test for unitary matrices U and V
        testUnitary(U);
        testUnitary(V);
    }
    void testPseudoInverse(Array2D<double> &A, std::optional<std::vector<double>> Ainv = std::nullopt)
    {
        auto B = A;
        SVD::pseudoinverse(A);
        auto C = A * B;
        C.print("A * Ainv (=Identity)");
        EXPECT_EQ(C.nRows(), C.nColumns());
        EXPECT_EQ(C.nRows(), A.nRows());
        PairIterator iter(C.nRows());
        for (auto &&[i, j] : iter)
        {
            auto x = C[{i, j}];
            EXPECT_NEAR(x, i == j ? 1.0 : 0.0, 1.0e-10);
        }

        if (Ainv)
            for (auto &&[ai, bi] : zip(A.linearArray(), Ainv.value()))
                EXPECT_NEAR(ai, bi, 5.0e-3);
    }
};

TEST_F(SVDTest, Identity)
{
    Array2D<double> A(3, 3);
    A = 0.0;
    A[{0, 0}] = 1.0;
    A[{1, 1}] = 1.0;
    A[{2, 2}] = 1.0;
    testDecomposition(A);
    testPseudoInverse(A);
}

TEST_F(SVDTest, Random)
{
    Array2D<double> A(4, 3);
    A.linearArray() = {0.98498, 4.48964, 2.61269, 0.46751, 0.98652, 3.33773,
                       3.59075, 1.19015, 3.85076, 1.83666, 0.66603, 4.52099};
    testDecomposition(A);
    testPseudoInverse(A);
}

TEST_F(SVDTest, Integers)
{
    Array2D<double> A(4, 3);
    A.linearArray() = {1, -2, 3, 5, 8, -1, 2, 1, 1, -1, 4, -3};
    testDecomposition(A);
    testPseudoInverse(A, std::vector<double>{-0.64516134, 0.13709678, 0.16129033, -0.63709681, 0.50967731, 0.02419355,
                                             -0.07741934, 0.47580636, 0.88387102, -0.04032259, -0.07096775, 0.54032262});
}

TEST_F(SVDTest, Real)
{
    // Water H2O, D2O, and HDO system with 0.9 feedback factor
    Array2D<double> A(6, 3);
    A.linearArray() = {0.033675, 0.154847, 0.178009, 0.033675, -0.086790, 0.055920, 0.033675, 0.034029, 0.008597,
                       0.100000, 0.000000, 0.000000, 0.000000, 0.100000,  0.000000, 0.000000, 0.000000, 0.100000};
    testDecomposition(A);
    testPseudoInverse(A, std::vector<double>{0.2373138, 1.73991434, 2.7562965, 8.40164722, 0.20355797, -1.63119659, 2.06693726,
                                             -3.69920922, 1.02631152, 0.20295693, 3.24235017, -1.70019258, 2.86276839,
                                             2.82518789, -0.83593246, -1.63275674, -1.697695, 3.3957371});
}

}; // namespace UnitTest
