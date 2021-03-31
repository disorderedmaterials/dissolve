// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/species.h"
#include "data/elements.h"
#include "data/ff/ff.h"
#include "neta/neta.h"
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <gtest/gtest.h>

namespace UnitTest
{
class NETATest : public ::testing::Test
{
    public:
    NETATest()
    {
        // Construct basic test species
        basic_.setName("Basic");
        basic_.addAtom(Elements::C, Vec3<double>(-0.208763, -0.975789, -0.095504));
        basic_.addAtom(Elements::C, Vec3<double>(0.045228, 0.295623, -0.380595));
        basic_.addAtom(Elements::C, Vec3<double>(1.300571, 0.857602, -0.348819));
        basic_.addAtom(Elements::C, Vec3<double>(2.360934, -0.005443, -0.027080));
        basic_.addAtom(Elements::C, Vec3<double>(2.111776, -1.366987, 0.240107));
        basic_.addAtom(Elements::C, Vec3<double>(0.805131, -1.869462, 0.219207));
        basic_.addAtom(Elements::H, Vec3<double>(1.468477, 1.902060, -0.572229));
        basic_.addAtom(Elements::H, Vec3<double>(3.373539, 0.371667, 0.002685));
        basic_.addAtom(Elements::H, Vec3<double>(2.939423, -2.029266, 0.465935));
        basic_.addAtom(Elements::H, Vec3<double>(0.596002, -2.910744, 0.426097));
        basic_.addAtom(Elements::N, Vec3<double>(-1.289150, 0.716693, -0.702798));
        basic_.addAtom(Elements::C, Vec3<double>(-1.646291, -0.686037, -0.306734));
        basic_.addAtom(Elements::C, Vec3<double>(-1.785682, 1.724996, 0.228719));
        basic_.addAtom(Elements::H, Vec3<double>(-1.619801, 1.449463, 1.291036));
        basic_.addAtom(Elements::H, Vec3<double>(-1.288061, 2.694431, 0.011695));
        basic_.addAtom(Elements::H, Vec3<double>(-2.873536, 1.867631, 0.046101));
        basic_.addAtom(Elements::H, Vec3<double>(-2.075934, -1.268474, -1.147377));
        basic_.addAtom(Elements::H, Vec3<double>(-2.213863, -0.767963, 0.649555));
        basic_.addMissingBonds();

        // Construct geometry test species
        geometric_.setName("Geometric");
        geometric_.addAtom(Elements::P, Vec3<double>(-5.824000, 3.981000, 0.000000));
        geometric_.addAtom(Elements::P, Vec3<double>(-1.824000, 3.981000, 0.000000));
        geometric_.addAtom(Elements::F, Vec3<double>(-1.824000, 4.981000, 0.000000));
        geometric_.addAtom(Elements::P, Vec3<double>(2.176000, 3.981000, 0.000000));
        geometric_.addAtom(Elements::F, Vec3<double>(2.176000, 4.981000, 0.000000));
        geometric_.addAtom(Elements::F, Vec3<double>(2.176000, 2.981000, 0.000000));
        geometric_.addAtom(Elements::P, Vec3<double>(6.176000, 4.003000, 0.000000));
        geometric_.addAtom(Elements::F, Vec3<double>(6.176000, 5.003000, 0.000000));
        geometric_.addAtom(Elements::F, Vec3<double>(5.241000, 3.463000, 0.000000));
        geometric_.addAtom(Elements::F, Vec3<double>(7.111000, 3.463000, 0.000000));
        geometric_.addAtom(Elements::P, Vec3<double>(-5.000000, 0.000000, 0.000000));
        geometric_.addAtom(Elements::F, Vec3<double>(-5.000000, 1.080000, 0.000000));
        geometric_.addAtom(Elements::F, Vec3<double>(-5.000000, -0.361000, -1.018000));
        geometric_.addAtom(Elements::F, Vec3<double>(-4.118000, -0.360000, 0.509000));
        geometric_.addAtom(Elements::F, Vec3<double>(-5.882000, -0.360000, 0.509000));
        geometric_.addAtom(Elements::P, Vec3<double>(0.000000, 0.000000, 0.000000));
        geometric_.addAtom(Elements::F, Vec3<double>(0.000000, 1.200000, 0.000000));
        geometric_.addAtom(Elements::F, Vec3<double>(0.000000, -1.200000, 0.000000));
        geometric_.addAtom(Elements::F, Vec3<double>(-1.280000, 0.000000, 0.000000));
        geometric_.addAtom(Elements::F, Vec3<double>(1.280000, 0.000000, 0.000000));
        geometric_.addAtom(Elements::F, Vec3<double>(0.000000, 0.000000, -1.200000));
        geometric_.addAtom(Elements::F, Vec3<double>(0.000000, 0.000000, 1.200000));
        geometric_.addAtom(Elements::P, Vec3<double>(4.800000, -0.020000, 0.000000));
        geometric_.addAtom(Elements::F, Vec3<double>(4.800000, 1.180000, 0.000000));
        geometric_.addAtom(Elements::F, Vec3<double>(3.865000, -0.560000, 0.000000));
        geometric_.addAtom(Elements::F, Vec3<double>(5.735000, -0.560000, 0.000000));
        geometric_.addAtom(Elements::F, Vec3<double>(4.800000, -0.020000, -1.200000));
        geometric_.addAtom(Elements::F, Vec3<double>(4.800000, -0.020000, 1.200000));
        geometric_.addAtom(Elements::P, Vec3<double>(-3.000000, -4.300000, 0.000000));
        geometric_.addAtom(Elements::F, Vec3<double>(-3.000000, -3.100000, 0.000000));
        geometric_.addAtom(Elements::F, Vec3<double>(-4.280000, -4.300000, 0.000000));
        geometric_.addAtom(Elements::F, Vec3<double>(-1.720000, -4.300000, 0.000000));
        geometric_.addAtom(Elements::P, Vec3<double>(3.000000, -4.000000, 0.000000));
        geometric_.addAtom(Elements::F, Vec3<double>(3.000000, -2.800000, 0.000000));
        geometric_.addAtom(Elements::F, Vec3<double>(3.000000, -5.200000, 0.000000));
        geometric_.addAtom(Elements::F, Vec3<double>(1.720000, -4.000000, 0.000000));
        geometric_.addAtom(Elements::F, Vec3<double>(4.280000, -4.000000, 0.000000));
        geometric_.addMissingBonds();
    }

    protected:
    // Species
    Species basic_, geometric_;

    protected:
    // Test NETA description on all atom in molecule, expecting success for supplied atom indices
    void testNETA(std::string_view title, const Species &sp, const NETADefinition &neta,
                  const std::vector<int> &matchingIndices)
    {
        fmt::print("Testing: {}...\n", title);
        fmt::print("-- Species '{}', expected matching atoms: {}\n", sp.name(), matchingIndices);

        for (const auto &i : sp.atoms())
        {
            auto score = neta.score(&i);
            fmt::print("NETA score for atom {} is {}.\n", i.index(), score);
            if (std::find(matchingIndices.begin(), matchingIndices.end(), i.index()) != matchingIndices.end())
                EXPECT_NE(score, NETANode::NoMatch);
            else
                EXPECT_EQ(score, NETANode::NoMatch);
        }
    }
};

TEST_F(NETATest, Syntax)
{
    NETADefinition neta;

    // Simple Connectivity
    EXPECT_TRUE(neta.create("-H,-C,-O"));
    // Or'd Connectivity
    EXPECT_TRUE(neta.create("-[Sc,Ti,V,Cr,Mn,Fe,Co,Ni,Cu]"));
    // Nested Connectivity
    EXPECT_TRUE(neta.create("-H(-He(-Li(-Be(-B(-C)))))"));
    // Modifiersz
    EXPECT_TRUE(neta.create("-C(n>=2,-H(n=3),-N(n<4,nbonds=1,root)),nbonds<8"));
    // Rings
    EXPECT_TRUE(neta.create("ring(size=6),-C(ring(size>=5,Sc,Ti,V,Cr,Mn))"));
    // Error - Double '-'
    EXPECT_FALSE(neta.create("-H,-C,--O"));
    // Error - Missing bracket 1
    EXPECT_FALSE(neta.create("-H,-C(-O"));
    // Error - Mis-placed bracket 2
    EXPECT_FALSE(neta.create("-H,-C,-O)"));
    // Error - Unknown element
    EXPECT_FALSE(neta.create("-C,-B,-A]"));
    // Error - Unknown contextual
    EXPECT_FALSE(neta.create("-C(nangles=5)"));
    // Error - Unknown operator
    EXPECT_FALSE(neta.create("-C(nbonds<>5)"));
}

TEST_F(NETATest, Matching)
{
    NETADefinition neta;

    EXPECT_TRUE(neta.create("ring(size=6)"));
    testNETA("Atom in six-membered ring", basic_, neta, {0, 1, 2, 3, 4, 5});

    EXPECT_TRUE(neta.create("nbonds=1,-C(ring(size=6))"));
    testNETA("Hydrogen atom attached to atom within a six-membered ring", basic_, neta, {6, 7, 8, 9});

    EXPECT_TRUE(neta.create("ring(size=6), ring(size=4)"));
    testNETA("Atom at junction of four and six-membered ring", basic_, neta, {0, 1});

    EXPECT_TRUE(neta.create("ring(size=6), ring(size=4), -N"));
    testNETA("Atom at junction of four and six-membered ring, adjacent to nitrogen", basic_, neta, {1});

    EXPECT_TRUE(neta.create("-N,ring(n=0)"));
    testNETA("Atom adjacent to nitrogen, but not in a ring ", basic_, neta, {12});

    EXPECT_TRUE(neta.create("-C(nh=3),nbonds=1"));
    testNETA("Hydrogen atoms present in CH3 group", basic_, neta, {13, 14, 15});
}

TEST_F(NETATest, Forcefield)
{
    class Forcefield_TEST : public Forcefield
    {
        protected:
        bool setUp() override
        {
            addAtomType(Elements::C, 1, "C6", "ring(size=6)", "Carbon in 6-membered ring", 0.0, {0.0, 0.0});
            addAtomType(Elements::C, 2, "C4", "ring(size=4),nh=2", "Carbon in 4-membered ring", 0.0, {0.0, 0.0});
            addAtomType(Elements::N, 3, "N4", "ring(size=4)", "Nitrogen in 4-membered ring", 0.0, {0.0, 0.0});

            return true;
        }
        std::string_view name() const override { return "Test Forcefield for NETA."; }
        std::string_view description() const override { return "Test Forcefield for NETA."; }
        ShortRangeType shortRangeType() const { return Forcefield::LennardJonesType; }
    };

    Forcefield_TEST testFF;
    testFF.prepare();
    testFF.createNETADefinitions();

    NETADefinition neta;

    EXPECT_TRUE(neta.create("-&C6,nbonds=1", &testFF));
    testNETA("Reference to type by name", basic_, neta, {6, 7, 8, 9});

    EXPECT_TRUE(neta.create("-&1,nbonds=1", &testFF));
    testNETA("Reference to type by ID", basic_, neta, {6, 7, 8, 9});

    fmt::print("Testing: Non-existent type name...\n");
    EXPECT_FALSE(neta.create("-&C5", &testFF));

    fmt::print("Testing: Non-existent type ID...\n");
    EXPECT_FALSE(neta.create("-&8", &testFF));
}

TEST_F(NETATest, Geometry)
{
    NETADefinition neta;

    neta.create("geometry=unbound");
    testNETA("Geometry = unbound", geometric_, neta, {0});

    neta.create("geometry=terminal");
    testNETA("Geometry = terminal [single bond]", geometric_, neta,
             {1, 2, 4, 5, 7, 8, 9, 11, 12, 13, 14, 16, 17, 18, 19, 20, 21, 23, 24, 25, 26, 27, 29, 30, 31, 33, 34, 35, 36});

    neta.create("geometry!=terminal");
    testNETA("Geometry != terminal [single bond]", geometric_, neta, {0, 3, 6, 10, 15, 22, 28, 32});

    neta.create("geometry=linear");
    testNETA("Geometry = linear", geometric_, neta, {3});

    neta.create("geometry=ts");
    testNETA("Geometry = TShape", geometric_, neta, {28});

    neta.create("geometry=tp");
    testNETA("Geometry = trigonal planar", geometric_, neta, {6});

    neta.create("geometry=tet");
    testNETA("Geometry = tetrahedral", geometric_, neta, {10});

    neta.create("geometry=sqp");
    testNETA("Geometry = square planar", geometric_, neta, {32});

    neta.create("geometry=tbp");
    testNETA("Geometry = trigonal bipyramidal", geometric_, neta, {22});

    neta.create("geometry=oct");
    testNETA("Geometry = octahedral", geometric_, neta, {15});
}

} // namespace UnitTest
