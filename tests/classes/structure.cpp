// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/structure.h"
#include "nodes/calculateBonding.h"
#include <gtest/gtest.h>

namespace UnitTest
{

TEST(StructureTest, Atomic)
{
    Structure s;
    s.addAtom(Elements::C, {});
    CalculateBondingNode::calculate(s);
    s.removeAtom(0);
    EXPECT_EQ(s.nAtoms(), 0);
}

TEST(StructureTest, Molecule1)
{
    Structure s;
    s.addAtom(Elements::H, {3.924, 5.424, 0.000});
    s.addAtom(Elements::O, {5.139, 5.968, 0.000});
    s.addAtom(Elements::H, {6.088, 5.120, 0.000});
    CalculateBondingNode::calculate(s, 1.5);
    EXPECT_EQ(s.bonds().size(), 2);
    s.removeAtom(1);
    EXPECT_EQ(s.bonds().size(), 0);
    EXPECT_EQ(s.nAtoms(), 2);
}

TEST(StructureTest, Molecule1Alt)
{
    Structure s;
    s.addAtom(Elements::H, {3.924, 5.424, 0.000});
    s.addAtom(Elements::H, {6.088, 5.120, 0.000});
    s.addAtom(Elements::O, {5.139, 5.968, 0.000});
    CalculateBondingNode::calculate(s, 1.5);

    EXPECT_EQ(s.bonds().size(), 2);
    EXPECT_EQ(s.atom(0)->bonds().size(), 1);
    EXPECT_EQ(s.atom(1)->bonds().size(), 1);
    EXPECT_EQ(s.atom(2)->bonds().size(), 2);
}

TEST(StructureTest, Molecule2)
{
    Structure s;
    s.addAtom(Elements::C, {-2.512458e+00, -4.297086e-01, 1.129543e-01});
    s.addAtom(Elements::C, {-1.296323e+00, 4.456275e-01, 9.428744e-02});
    s.addAtom(Elements::C, {1.464787e-02, -2.798742e-01, -8.091826e-03});
    s.addAtom(Elements::C, {1.241877e+00, 5.832093e-01, -3.044739e-02});
    CalculateBondingNode::calculate(s);

    std::cout << "Type: " << (int)s.box()->type() << std::endl;
    EXPECT_EQ(s.bonds().size(), 3);
    s.removeAtom(1);
    EXPECT_EQ(s.bonds().size(), 1);
    EXPECT_EQ(s.nAtoms(), 3);
    s.removeAtom(0);
    EXPECT_EQ(s.bonds().size(), 1);
    EXPECT_EQ(s.nAtoms(), 2);
    s.removeAtom(0);
    EXPECT_EQ(s.bonds().size(), 0);
    EXPECT_EQ(s.nAtoms(), 1);
    s.removeAtom(0);
    EXPECT_EQ(s.bonds().size(), 0);
    EXPECT_EQ(s.nAtoms(), 0);
}

} // namespace UnitTest
