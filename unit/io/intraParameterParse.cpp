// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/speciesangle.h"
#include "classes/speciesbond.h"
#include "classes/speciestorsion.h"
#include <gtest/gtest.h>
#include <random>

namespace UnitTest
{
template <class I> double testParameters(I &intra, double testValue)
{
    // Get reference energy and force from intramolecular term
    auto refEnergy = intra.energy(testValue);
    auto refForce = intra.force(testValue);

    // Get the parameters from the term as a string, re-set and compare values
    auto params = intra.parametersAsString();
    intra.setFormAndParameters(intra.form(), params);

    EXPECT_DOUBLE_EQ(refEnergy, intra.energy(testValue));
    EXPECT_DOUBLE_EQ(refForce, intra.force(testValue));

    return refEnergy;
}

TEST(IntraParameters, Syntax)
{
    SpeciesBond b;
    // Valid String
    EXPECT_TRUE(b.setFormAndParameters(BondFunctions::Form::None, ""));
    EXPECT_TRUE(b.setFormAndParameters(BondFunctions::Form::Harmonic, "k=4000.0 eq=1.0"));
    EXPECT_TRUE(b.setFormAndParameters(BondFunctions::Form::Harmonic, "k=4000.0   eq=1.0"));
    // Just values
    EXPECT_TRUE(b.setFormAndParameters(BondFunctions::Form::Harmonic, "4000.0 1.0"));
    EXPECT_TRUE(b.setFormAndParameters(BondFunctions::Form::Harmonic, "4000 1"));
    // Too many values
    EXPECT_FALSE(b.setFormAndParameters(BondFunctions::Form::Harmonic, "4000.0 1.0 99"));
    // Missing parameter
    EXPECT_FALSE(b.setFormAndParameters(BondFunctions::Form::Harmonic, "k=4000.0"));
    // Comma, not space
    EXPECT_FALSE(b.setFormAndParameters(BondFunctions::Form::Harmonic, "k=4000.0,eq=1.0"));
    // Missing value
    EXPECT_FALSE(b.setFormAndParameters(BondFunctions::Form::Harmonic, "k= eq=1.0"));
    // Missing name
    EXPECT_FALSE(b.setFormAndParameters(BondFunctions::Form::Harmonic, "=4000.0 eq=1.0"));
    // Mixing values and assignments
    EXPECT_FALSE(b.setFormAndParameters(BondFunctions::Form::Harmonic, "4000.0 eq=1.0"));
    // Unknown parameter
    EXPECT_FALSE(b.setFormAndParameters(BondFunctions::Form::Harmonic, "k=4000.0 X=1.0"));

    SpeciesAngle a1, a2;
    // Valid string
    EXPECT_TRUE(a1.setFormAndParameters(AngleFunctions::Form::Cos2, "k=100.0 C0=1 C1=2 C2=3"));
    EXPECT_TRUE(a1.setFormAndParameters(AngleFunctions::Form::Cos2, "K=100.0 c0=1 c1=2 C2=3"));
    // Missing parameter
    EXPECT_FALSE(a1.setFormAndParameters(AngleFunctions::Form::Cos2, "K=100.0 c0=1 c1=2"));
    // Wrong parameters
    EXPECT_FALSE(a2.setFormAndParameters(AngleFunctions::Form::Cos2, "eq=1.0 k=4000.0"));

    SpeciesTorsion t;
    // Valid String
    EXPECT_TRUE(t.setFormAndParameters(TorsionFunctions::Form::Cosine, "k=5.0 eq=1.0 n=3 s=1"));
    // Parameter jumping
    EXPECT_TRUE(t.setFormAndParameters(TorsionFunctions::Form::CosN, "k6=9.0"));
}

TEST(IntraParameters, Ordering)
{
    SpeciesBond b1, b2;
    EXPECT_TRUE(b1.setFormAndParameters(BondFunctions::Form::Harmonic, "k=4000.0 eq=1.0"));
    EXPECT_TRUE(b2.setFormAndParameters(BondFunctions::Form::Harmonic, "eq=1.0 k=4000.0"));
    EXPECT_DOUBLE_EQ(testParameters(b1, 1.05), testParameters(b2, 1.05));

    SpeciesAngle a1, a2;
    EXPECT_TRUE(a1.setFormAndParameters(AngleFunctions::Form::Cos2, "k=100.0 C0=1 C1=2 C2=3"));
    EXPECT_TRUE(a2.setFormAndParameters(AngleFunctions::Form::Cos2, "C0=1 k=100.0 C2=3 C1=2"));
    EXPECT_DOUBLE_EQ(testParameters(a1, 100.5), testParameters(a2, 100.5));
    EXPECT_TRUE(a1.setFormAndParameters(AngleFunctions::Form::Cos2, "C2=3 k=100.0 C1=2 C0=1"));
    EXPECT_DOUBLE_EQ(testParameters(a1, 100.5), testParameters(a2, 100.5));

    SpeciesTorsion t1, t2;
    EXPECT_TRUE(t1.setFormAndParameters(TorsionFunctions::Form::CosN, "k6=9.0"));
    EXPECT_TRUE(t2.setFormAndParameters(TorsionFunctions::Form::CosN, "k1=0.0 k3=0.0 k6=9.0"));
    EXPECT_DOUBLE_EQ(testParameters(t1, 100.5), testParameters(t2, 100.5));
    EXPECT_TRUE(t2.setFormAndParameters(TorsionFunctions::Form::CosN, "k6=9.0 k1=0.0 k3=0.0"));
    EXPECT_DOUBLE_EQ(testParameters(t1, 100.5), testParameters(t2, 100.5));
}

} // namespace UnitTest
