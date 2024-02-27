// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "neta/neta.h"
#include "classes/species.h"
#include "data/elements.h"
#include "data/ff/ff.h"
#include "tests/testData.h"
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <gtest/gtest.h>

namespace UnitTest
{
class NETATest : public ::testing::Test
{
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
            if (score != -1)
                fmt::print("-- NETA score for atom {} is {}.\n", i.index(), score);
            if (std::find(matchingIndices.begin(), matchingIndices.end(), i.index()) != matchingIndices.end())
                EXPECT_NE(score, NETANode::NoMatch);
            else
                EXPECT_EQ(score, NETANode::NoMatch);
        }
    }
    // Test NETA description on specific atom in molecule, expecting the match path to contain the supplied atom indices
    NETAMatchedGroup testNETAMatchPath(std::string_view title, const Species &sp, const NETADefinition &neta,
                                       int targetAtomIndex, const std::vector<int> &matchingIndices)
    {
        fmt::print("Path Test: {}, atom {}...\n", title, targetAtomIndex);
        fmt::print("-- Species '{}', expected matched atom set : {}\n", sp.name(), matchingIndices);

        auto matchedPath = neta.matchedPath(&sp.atom(targetAtomIndex));
        fmt::print("-- Actual matched atom set : {}\n",
                   joinStrings(matchedPath.set(), " ", [](const auto *i) { return i->index(); }));
        EXPECT_EQ(matchedPath.set().size(), matchingIndices.size());

        for (auto *i : matchedPath.set())
            EXPECT_TRUE(std::find(matchingIndices.begin(), matchingIndices.end(), i->index()) != matchingIndices.end());

        return matchedPath;
    }
    // Test identifier group specified
    void testIdentifiers(const NETAMatchedGroup &matchedPath, std::string idName, const std::vector<int> &matchingIndices)
    {
        fmt::print("Identifier Test: {}\n", idName);
        fmt::print("-- Expected identified atoms : {}\n", matchingIndices);
        auto &ids = matchedPath.identifiers();
        auto it = ids.find(idName);
        EXPECT_TRUE(it != ids.end());
        if (it == ids.end())
        {
            fmt::print("Identifier group '{}' not present in matched data.\n", idName);
            return;
        }

        auto &group = it->second;

        fmt::print("-- Actual identified atoms : {}\n", joinStrings(group, " ", [](const auto *i) { return i->index(); }));

        EXPECT_EQ(group.size(), matchingIndices.size());

        for (auto *i : group)
            EXPECT_TRUE(std::find(matchingIndices.begin(), matchingIndices.end(), i->index()) != matchingIndices.end());
    }
};

TEST_F(NETATest, Syntax)
{
    NETADefinition neta;

    // Empty NETA string
    EXPECT_TRUE(neta.create(""));
    // Simple Connectivity
    EXPECT_TRUE(neta.create("-H,-C,-O"));
    // Or'd Connectivity
    EXPECT_TRUE(neta.create("-[Sc,Ti,V,Cr,Mn,Fe,Co,Ni,Cu]"));
    // Brackets
    EXPECT_TRUE(neta.create("(-H,(-C,(-O)))"));
    // Nested Connectivity
    EXPECT_TRUE(neta.create("-H(-He(-Li(-Be(-B(-C)))))"));
    // Modifiers
    EXPECT_TRUE(neta.create("-C(n>=2,-H(n=3),-N(n<4,nbonds=1,root)),nbonds<8"));
    // Rings
    EXPECT_TRUE(neta.create("ring(size=6),-C(ring(size>=5,Sc,Ti,V,Cr,Mn))"));
    // Or'd Nodes
    EXPECT_TRUE(neta.create("-H|-C|ring()"));
    // Or'd Node Sequence
    EXPECT_TRUE(neta.create("-H|-C(nh=4),ring()|-Zn"));
    // Or'd Node Sequence with brackets
    EXPECT_TRUE(neta.create("-H|(-C(nh=4),ring()|(-Zn))"));
    // Or'd Node Sequence in bracketed part
    EXPECT_TRUE(neta.create("-V(-H|-C(nh=4),ring()|-Zn)"));
    // Error - Dot not Comma
    EXPECT_FALSE(neta.create("-H.-C,-O"));
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
    // Error - Bad syntax
    EXPECT_FALSE(neta.create("?O,"));
    // Error - Double ','
    EXPECT_FALSE(neta.create("-H,-C,,-O"));
}

TEST_F(NETATest, Matching)
{
    NETADefinition neta;

    EXPECT_TRUE(neta.create("-C"));
    testNETA("Any atom bound to carbon", UnitTest::methaneSpecies(), neta, {1, 2, 3, 4});

    EXPECT_TRUE(neta.create("nbonds=4"));
    testNETA("Any atom with four bonds", UnitTest::methaneSpecies(), neta, {0});

    EXPECT_TRUE(neta.create("-C"));
    testNETA("Any atom bound to carbon", UnitTest::ethaneSpecies(), neta, {0, 1, 2, 3, 4, 5, 6, 7});

    EXPECT_TRUE(neta.create("nh=3"));
    testNETA("Any atom with three hydrogens attached", UnitTest::ethaneSpecies(), neta, {0, 1});

    EXPECT_TRUE(neta.create("ring()"));
    testNETA("Atom in any ring", UnitTest::ringsSpecies(), neta, {0, 1, 2, 3, 4, 5, 10, 11});

    EXPECT_TRUE(neta.create("ring(size=6)"));
    testNETA("Atom in six-membered ring", UnitTest::ringsSpecies(), neta, {0, 1, 2, 3, 4, 5});

    EXPECT_TRUE(neta.create("ring(n=0)"));
    testNETA("Any atom not in a ring", UnitTest::ringsSpecies(), neta, {6, 7, 8, 9, 12, 13, 14, 15, 16, 17});

    EXPECT_TRUE(neta.create("nbonds=1,-C(ring(size=6))"));
    testNETA("Hydrogen atom attached to atom within a six-membered ring", UnitTest::ringsSpecies(), neta, {6, 7, 8, 9});

    EXPECT_TRUE(neta.create("ring(size=6), ring(size=4)"));
    testNETA("Atom at junction of four and six-membered ring", UnitTest::ringsSpecies(), neta, {0, 1});

    EXPECT_TRUE(neta.create("-N, ring(size=6), ring(size=4)"));
    testNETA("Atom at junction of four and six-membered ring, adjacent to nitrogen", UnitTest::ringsSpecies(), neta, {1});

    EXPECT_TRUE(neta.create("-N,ring(n=0)"));
    testNETA("Atom adjacent to nitrogen, but not in a ring ", UnitTest::ringsSpecies(), neta, {12});

    EXPECT_TRUE(neta.create("-N,ring(n=0) | -N,ring(size=6), ring(size=4)"));
    testNETA("Either of the previous atoms", UnitTest::ringsSpecies(), neta, {1, 12});

    EXPECT_TRUE(neta.create("-C(nh=3),nbonds=1"));
    testNETA("Hydrogen atoms present in CH3 group", UnitTest::ringsSpecies(), neta, {13, 14, 15});

    EXPECT_TRUE(neta.create("?C,!ring(size=6)"));
    testNETA("Any carbon except one in a 6-membered ring", UnitTest::ringsSpecies(), neta, {11, 12});

    EXPECT_TRUE(neta.create("?C,!(nh=2 | -C(-N))"));
    testNETA("Any carbon except one with two hydrogens or which is two bonds away from a nitrogen", UnitTest::ringsSpecies(),
             neta, {1, 3, 4, 5, 12});
}

TEST_F(NETATest, Creation)
{
    // Carbon atom - full description
    NETADefinition neta(&UnitTest::methaneSpecies().atom(0));
    EXPECT_EQ(neta.definitionString(), "nbonds=4,nh=4");
    testNETA("Carbon atom in methane", UnitTest::methaneSpecies(), neta, {0});

    // Methane hydrogen atom
    // -- Basic connectivity
    neta.create(&UnitTest::methaneSpecies().atom(1), 0);
    EXPECT_EQ(neta.definitionString(), "nbonds=1,-C");
    testNETA("Hydrogen atom in methane", UnitTest::methaneSpecies(), neta, {1, 2, 3, 4});
    // -- Primary neighbour connectivity
    neta.create(&UnitTest::methaneSpecies().atom(1), 1);
    EXPECT_EQ(neta.definitionString(), "nbonds=1,-C(nbonds=4,nh=4)");
    testNETA("Hydrogen atom in methane", UnitTest::methaneSpecies(), neta, {1, 2, 3, 4});
    // -- Secondary neighbour connectivity (equivalent to primary)
    neta.create(&UnitTest::methaneSpecies().atom(1), 2);
    EXPECT_EQ(neta.definitionString(), "nbonds=1,-C(nbonds=4,nh=4)");
    testNETA("Hydrogen atom in methane", UnitTest::methaneSpecies(), neta, {1, 2, 3, 4});
    // -- Basic connectivity including the root element
    neta.create(&UnitTest::methaneSpecies().atom(1), 0,
                Flags<NETADefinition::NETACreationFlags>(NETADefinition::NETACreationFlags::IncludeRootElement));
    EXPECT_EQ(neta.definitionString(), "?H, nbonds=1,-C");
    testNETA("Hydrogen atom in methane", UnitTest::methaneSpecies(), neta, {1, 2, 3, 4});
    neta.create(&UnitTest::methaneSpecies().atom(1), 1,
                Flags<NETADefinition::NETACreationFlags>(NETADefinition::NETACreationFlags::IncludeRootElement));
    EXPECT_EQ(neta.definitionString(), "?H, nbonds=1,-C(nbonds=4,nh=4)");
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
        ShortRangeFunctions::Form shortRangeForm() const override { return ShortRangeFunctions::Form::LennardJones; }
    };

    Forcefield_TEST testFF;
    testFF.prepare();
    testFF.createNETADefinitions();

    NETADefinition neta;

    EXPECT_TRUE(neta.create("-&C6,nbonds=1", &testFF));
    testNETA("Reference to type by name", UnitTest::ringsSpecies(), neta, {6, 7, 8, 9});

    EXPECT_TRUE(neta.create("-&1,nbonds=1", &testFF));
    testNETA("Reference to type by ID", UnitTest::ringsSpecies(), neta, {6, 7, 8, 9});

    fmt::print("Testing: Non-existent type name...\n");
    EXPECT_FALSE(neta.create("-&C5", &testFF));

    fmt::print("Testing: Non-existent type ID...\n");
    EXPECT_FALSE(neta.create("-&8", &testFF));
}

TEST_F(NETATest, Geometry)
{
    NETADefinition neta;

    neta.create("geometry=unbound");
    testNETA("Geometry = unbound", UnitTest::geometricSpecies(), neta, {0});

    neta.create("geometry=terminal");
    testNETA("Geometry = terminal [single bond]", UnitTest::geometricSpecies(), neta,
             {1, 2, 4, 5, 7, 8, 9, 11, 12, 13, 14, 16, 17, 18, 19, 20, 21, 23, 24, 25, 26, 27, 29, 30, 31, 33, 34, 35, 36});

    neta.create("geometry!=terminal");
    testNETA("Geometry != terminal [single bond]", UnitTest::geometricSpecies(), neta, {0, 3, 6, 10, 15, 22, 28, 32});

    neta.create("geometry=linear");
    testNETA("Geometry = linear", UnitTest::geometricSpecies(), neta, {3});

    neta.create("geometry=ts");
    testNETA("Geometry = TShape", UnitTest::geometricSpecies(), neta, {28});

    neta.create("geometry=tp");
    testNETA("Geometry = trigonal planar", UnitTest::geometricSpecies(), neta, {6});

    neta.create("geometry=tet");
    testNETA("Geometry = tetrahedral", UnitTest::geometricSpecies(), neta, {10});

    neta.create("geometry=sqp");
    testNETA("Geometry = square planar", UnitTest::geometricSpecies(), neta, {32});

    neta.create("geometry=tbp");
    testNETA("Geometry = trigonal bipyramidal", UnitTest::geometricSpecies(), neta, {22});

    neta.create("geometry=oct");
    testNETA("Geometry = octahedral", UnitTest::geometricSpecies(), neta, {15});
}

TEST_F(NETATest, FragmentMatching)
{
    NETADefinition neta;

    EXPECT_TRUE(neta.create("?C,-H(n=3)"));
    testNETAMatchPath("Methyl group C(0)", UnitTest::ethaneSpecies(), neta, 0, {0, 2, 3, 4});

    EXPECT_TRUE(neta.create("?C,-C(-H(n=3))"));
    testNETAMatchPath("C(1) methyl plus C(0)", UnitTest::ethaneSpecies(), neta, 0, {0, 1, 5, 6, 7});

    EXPECT_TRUE(neta.create("ring(size=4,C,C,C,N)"));
    testNETA("Any atom in a four-membered ring (explicit ring definition)", UnitTest::ringsSpecies(), neta, {0, 1, 10, 11});

    EXPECT_TRUE(neta.create("ring(size=4,C(n=3),N)"));
    testNETA("Any atom in a four-membered ring (shortest ring definition)", UnitTest::ringsSpecies(), neta, {0, 1, 10, 11});

    EXPECT_TRUE(neta.create("ring(size=4,C,C(n=2),N)"));
    testNETA("Any atom in a four-membered ring (unnecessary ring definition)", UnitTest::ringsSpecies(), neta, {0, 1, 10, 11});

    EXPECT_TRUE(neta.create("ring(size=4,N)"));
    testNETA("Any atom in a four-membered ring (only one atom specified)", UnitTest::ringsSpecies(), neta, {0, 1, 10, 11});

    EXPECT_TRUE(neta.create("ring(size=4,C(n=2))"));
    testNETA("Any atom in a four-membered ring (only one atom specified)", UnitTest::ringsSpecies(), neta, {0, 1, 10, 11});

    EXPECT_TRUE(neta.create("ring(size=8,C(n=4),N,C(n=3))"));
    testNETA("Any atom in an eight-membered ring (split definition)", UnitTest::ringsSpecies(), neta,
             {0, 1, 2, 3, 4, 5, 10, 11});

    EXPECT_TRUE(neta.create("ring(size=4,N(n=2),C(n=2))"));
    testNETA("No atoms - too many of atom type requested", UnitTest::ringsSpecies(), neta, {});

    EXPECT_TRUE(neta.create("ring(size=8,C(n=4),N,C(n=4))"));
    testNETA("No atoms - too many for ring (9 vs 8)", UnitTest::ringsSpecies(), neta, {});

    EXPECT_TRUE(neta.create("ring(size=6,C(n=8))"));
    testNETA("No atoms - too many for ring (8 vs 6)", UnitTest::ringsSpecies(), neta, {});
}

TEST_F(NETATest, IdentifierMatching)
{
    NETADefinition neta;

    // Methanol identifying cog at the oxygen, x on the carbon, and y on the hydroxyl hydrogen
    EXPECT_TRUE(neta.create("?O,#cog,-C(#x),-H(-O(root),#y)"));
    auto matchedPath = testNETAMatchPath("Carbon and hydroxyl", UnitTest::methanolSpecies(), neta, 4, {0, 4, 5});
    for (auto &&[key, ids] : matchedPath.identifiers())
        fmt::print("ID '{}' : {}\n", key, joinStrings(ids, " ", [](const auto *i) { return i->index(); }));
    testIdentifiers(matchedPath, "cog", {4});
    testIdentifiers(matchedPath, "x", {0});
    testIdentifiers(matchedPath, "y", {5});

    // Difluorobenzene, identify carbon atoms attached to fluorines as 'cog'
    EXPECT_TRUE(neta.create("?C,ring(size=6,C(-H),C(-H),C(#cog,-F),C(-H),C(-H),C(#cog,-F))"));
    matchedPath = testNETAMatchPath("Whole molecule", UnitTest::difluorobenzeneSpecies(), neta, 0,
                                    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11});
    testIdentifiers(matchedPath, "cog", {2, 5});

    // Difluorobenzene with full axis definition
    EXPECT_TRUE(neta.create("?C,ring(size=6,C(-H),C(-H),C(#cog,-F),C(-H),C(#y,-H),C(#[cog,x],-F))"));
    matchedPath = testNETAMatchPath("Whole molecule", UnitTest::difluorobenzeneSpecies(), neta, 0,
                                    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11});
    testIdentifiers(matchedPath, "cog", {2, 5});
    testIdentifiers(matchedPath, "x", {5});
    testIdentifiers(matchedPath, "y", {4});

    // Difluorobenzene with full axis definition (separate cog and x specification for one carbon
    EXPECT_TRUE(neta.create("?C,ring(size=6,C(-H),C(-H),C(#cog,-F),C(-H),C(#y,-H),C(#cog,#x,-F))"));
    matchedPath = testNETAMatchPath("Whole molecule", UnitTest::difluorobenzeneSpecies(), neta, 0,
                                    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11});
    testIdentifiers(matchedPath, "cog", {2, 5});
    testIdentifiers(matchedPath, "x", {5});
    testIdentifiers(matchedPath, "y", {4});
}

} // namespace UnitTest
