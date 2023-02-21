// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/species.h"
#include <gtest/gtest.h>

namespace UnitTest
{

TEST(SpeciesTest, Atomic)
{
    Species sp;
    sp.addAtom(Elements::C, {});
    sp.addMissingBonds();
    sp.removeAtom(0);
    EXPECT_EQ(sp.nAtoms(), 0);
}

TEST(SpeciesTest, Molecule1)
{
    Species sp;
    sp.addAtom(Elements::H, {3.924, 5.424, 0.000});
    sp.addAtom(Elements::O, {5.139, 5.968, 0.000});
    sp.addAtom(Elements::H, {6.088, 5.120, 0.000});
    sp.addMissingBonds(1.5);
    sp.updateIntramolecularTerms();
    EXPECT_EQ(sp.nBonds(), 2);
    EXPECT_EQ(sp.nAngles(), 1);
    EXPECT_EQ(sp.nTorsions(), 0);
    sp.removeAtom(1);
    EXPECT_EQ(sp.nBonds(), 0);
    EXPECT_EQ(sp.nAngles(), 0);
    EXPECT_EQ(sp.nTorsions(), 0);
    EXPECT_EQ(sp.nAtoms(), 2);
}

TEST(SpeciesTest, Molecule1Alt)
{
    Species sp;
    sp.addAtom(Elements::H, {3.924, 5.424, 0.000});
    sp.addAtom(Elements::H, {6.088, 5.120, 0.000});
    sp.addAtom(Elements::O, {5.139, 5.968, 0.000});
    sp.addMissingBonds(1.5);
    sp.updateIntramolecularTerms();
    EXPECT_EQ(sp.nBonds(), 2);
    EXPECT_EQ(sp.nAngles(), 1);
    EXPECT_EQ(sp.nTorsions(), 0);
    EXPECT_EQ(sp.atom(0).nBonds(), 1);
    EXPECT_EQ(sp.atom(1).nBonds(), 1);
    EXPECT_EQ(sp.atom(2).nBonds(), 2);
    EXPECT_EQ(sp.atom(0).nAngles(), 1);
    EXPECT_EQ(sp.atom(1).nAngles(), 1);
    EXPECT_EQ(sp.atom(2).nAngles(), 1);
    EXPECT_EQ(sp.atom(0).nTorsions(), 0);
    EXPECT_EQ(sp.atom(1).nTorsions(), 0);
    EXPECT_EQ(sp.atom(2).nTorsions(), 0);
}

TEST(SpeciesTest, Molecule2)
{
    Species sp;
    sp.addAtom(Elements::C, {-2.512458e+00, -4.297086e-01, 1.129543e-01});
    sp.addAtom(Elements::C, {-1.296323e+00, 4.456275e-01, 9.428744e-02});
    sp.addAtom(Elements::C, {1.464787e-02, -2.798742e-01, -8.091826e-03});
    sp.addAtom(Elements::C, {1.241877e+00, 5.832093e-01, -3.044739e-02});
    sp.addMissingBonds();
    sp.updateIntramolecularTerms();
    EXPECT_EQ(sp.nBonds(), 3);
    EXPECT_EQ(sp.nAngles(), 2);
    EXPECT_EQ(sp.nTorsions(), 1);
    sp.removeAtom(1);
    EXPECT_EQ(sp.nBonds(), 1);
    EXPECT_EQ(sp.nAngles(), 0);
    EXPECT_EQ(sp.nTorsions(), 0);
    EXPECT_EQ(sp.nAtoms(), 3);
    sp.removeAtom(0);
    EXPECT_EQ(sp.nBonds(), 1);
    EXPECT_EQ(sp.nAngles(), 0);
    EXPECT_EQ(sp.nTorsions(), 0);
    EXPECT_EQ(sp.nAtoms(), 2);
    sp.removeAtom(0);
    EXPECT_EQ(sp.nBonds(), 0);
    EXPECT_EQ(sp.nAngles(), 0);
    EXPECT_EQ(sp.nTorsions(), 0);
    EXPECT_EQ(sp.nAtoms(), 1);
    sp.removeAtom(0);
    EXPECT_EQ(sp.nBonds(), 0);
    EXPECT_EQ(sp.nAngles(), 0);
    EXPECT_EQ(sp.nTorsions(), 0);
    EXPECT_EQ(sp.nAtoms(), 0);
}

} // namespace UnitTest
