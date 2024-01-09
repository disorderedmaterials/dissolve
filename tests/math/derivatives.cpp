// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/speciesAngle.h"
#include "classes/speciesAtom.h"
#include "classes/speciesBond.h"
#include "classes/speciesImproper.h"
#include "classes/speciesTorsion.h"
#include <fmt/format.h>
#include <gtest/gtest.h>

namespace UnitTest
{
class DerivativesTest : public ::testing::Test
{
    public:
    DerivativesTest() : bond_(&i_, &j_), angle_(&i_, &j_, &k_), torsion_(&i_, &j_, &k_, &l_)
    {
        i_.set(Elements::H, Vec3<double>(-1.0, 1.0, 0.0));
        j_.set(Elements::C, Vec3<double>(-1.0, 0.0, 0.0));
        k_.set(Elements::C, Vec3<double>(1.0, 0.0, 0.0));
        l_.set(Elements::O, Vec3<double>(1.0, 1.0, 0.0));
    }

    protected:
    // Atoms for use in intramolecular terms
    SpeciesAtom i_, j_, k_, l_;
    // Intramolecular objects
    SpeciesBond bond_;
    SpeciesAngle angle_;
    SpeciesTorsion torsion_;
    // Tolerance for value comparison
    double tolerance_{1.0e-6};

    protected:
    // Test supplied intramolecular function over the supplied range, comparing numerical and analytical derivatives at each
    // point
    template <class Intra, class Form>
    void intraTest(Intra &intraTerm, Form form, const std::vector<double> &params, double xMin, double xMax, double xDelta,
                   bool angular = false)
    {
        intraTerm.setInteractionFormAndParameters(form, params);

        auto x = xMin;
        const auto dx = xDelta / 100.0;
        while (x <= xMax)
        {
            // Calculate derivative
            auto E0 = intraTerm.energy(x - dx);
            auto E1 = intraTerm.energy(x + dx);
            auto grad = (E1 - E0) / (2.0 * dx);

            // Compare analytic value - for angle terms, convert gradient to radians and remove factor of -1.0/sin(x) from force
            if (angular)
                ASSERT_NEAR(-grad * DEGRAD, intraTerm.force(x) * -sin(x / DEGRAD), tolerance_);
            else
                ASSERT_NEAR(-grad, intraTerm.force(x), tolerance_);

            x += xDelta;
        }
    }
};

TEST_F(DerivativesTest, BondFunctions)
{
    tolerance_ = 1.0e-8;

    // Harmonic form (parameters = k, eq)
    intraTest<SpeciesBond>(bond_, BondFunctions::Form::Harmonic, {4184.0, 1.5}, 1.0, 2.0, 0.01);

    // EPSR form (parameters = C/2, eq)
    intraTest<SpeciesBond>(bond_, BondFunctions::Form::EPSR, {65.0, 1.1}, 0.6, 1.6, 0.01);
}

TEST_F(DerivativesTest, AngleFunctions)
{
    tolerance_ = 5.0e-7;

    // Harmonic form (parameters = k, eq)
    intraTest<SpeciesAngle>(angle_, AngleFunctions::Form::Harmonic, {418.4, 109.5}, 80.0, 120.0, 0.1, true);

    // Cosine form (parameters = k, n, eq, s)
    intraTest<SpeciesAngle>(angle_, AngleFunctions::Form::Cosine, {418.4, 2.0, 95.0, -1}, 70.0, 120.0, 0.1, true);

    // Cos2 form (parameters = k, C0, C1, C2)
    intraTest<SpeciesAngle>(angle_, AngleFunctions::Form::Cos2, {418.4, 0.5, 2.0 / 3.0, 0.7}, 70.0, 120.0, 0.1, true);
}

TEST_F(DerivativesTest, TorsionFunctions)
{
    tolerance_ = 9.0e-5;
    // TODO Increase range once #542 is addressed
    auto thetaMin = 1.0, thetaMax = 179.0;

    // Cosine form (parameters = k, n, eq, s)
    intraTest<SpeciesTorsion>(torsion_, TorsionFunctions::Form::Cosine, {41.84, 3.0, 120.0, 1.0}, thetaMin, thetaMax, 1.0,
                              true);

    // Cos3 form (parameters = k1, k2, k3)
    intraTest<SpeciesTorsion>(torsion_, TorsionFunctions::Form::Cos3, {0.0, 11.4, 0.0}, thetaMin, thetaMax, 1.0, true);
    intraTest<SpeciesTorsion>(torsion_, TorsionFunctions::Form::Cos3, {19.0, 1.2, 5.6}, thetaMin, thetaMax, 1.0, true);

    // Cos3C form (parameters = k0, k1, k2, k3)
    intraTest<SpeciesTorsion>(torsion_, TorsionFunctions::Form::Cos3C, {4.0, 0.0, 11.4, 0.0}, thetaMin, thetaMax, 1.0, true);
    intraTest<SpeciesTorsion>(torsion_, TorsionFunctions::Form::Cos3C, {2.2, 19.0, 1.2, 5.6}, thetaMin, thetaMax, 1.0, true);

    // Cos4 form (parameters = k1, k2, k3, k4)
    intraTest<SpeciesTorsion>(torsion_, TorsionFunctions::Form::Cos4, {4.0, 0.0, 11.4, 0.0}, thetaMin, thetaMax, 1.0, true);
    intraTest<SpeciesTorsion>(torsion_, TorsionFunctions::Form::Cos4, {2.2, 19.0, 1.2, 5.6}, thetaMin, thetaMax, 1.0, true);

    // CosN form (parameters = k1, ..., kn)
    intraTest<SpeciesTorsion>(torsion_, TorsionFunctions::Form::CosN, {4.0}, thetaMin, thetaMax, 1.0, true);
    intraTest<SpeciesTorsion>(torsion_, TorsionFunctions::Form::CosN, {1.8, 15.9}, thetaMin, thetaMax, 1.0, true);
    intraTest<SpeciesTorsion>(torsion_, TorsionFunctions::Form::CosN, {1.8, 15.9, 0.4, 5.7, 7.8}, thetaMin, thetaMax, 1.0,
                              true);

    // CosNC form (parameters = k0, ..., kn)
    intraTest<SpeciesTorsion>(torsion_, TorsionFunctions::Form::CosNC, {1.04, 4.0}, thetaMin, thetaMax, 1.0, true);
    intraTest<SpeciesTorsion>(torsion_, TorsionFunctions::Form::CosNC, {2.89, 1.8, 15.9}, thetaMin, thetaMax, 1.0, true);
    intraTest<SpeciesTorsion>(torsion_, TorsionFunctions::Form::CosNC, {98.0, 1.8, 15.9, 0.4, 5.7, 7.8}, thetaMin, thetaMax,
                              1.0, true);

    // UFF Cosine form (parameters = k, n, eq)
    intraTest<SpeciesTorsion>(torsion_, TorsionFunctions::Form::UFFCosine, {10.7, 3, 109.5}, thetaMin, thetaMax, 1.0, true);
    intraTest<SpeciesTorsion>(torsion_, TorsionFunctions::Form::UFFCosine, {19.0, 4, 90.0}, thetaMin, thetaMax, 1.0, true);

    // FourierN form (parameters = k, C0, C1, ..., Cn)
    intraTest<SpeciesTorsion>(torsion_, TorsionFunctions::Form::FourierN, {12.0, 1.0}, thetaMin, thetaMax, 1.0, true);
    intraTest<SpeciesTorsion>(torsion_, TorsionFunctions::Form::FourierN, {44.0, 1.0, -1.0, 0.0}, thetaMin, thetaMax, 1.0,
                              true);
    intraTest<SpeciesTorsion>(torsion_, TorsionFunctions::Form::FourierN, {44.0, 1.0, -2.0, 3.0, 4.0, -5.0}, thetaMin, thetaMax,
                              1.0, true);
}

} // namespace UnitTest
