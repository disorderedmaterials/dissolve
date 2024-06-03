// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/site.h"
#include "classes/species.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{

class SpeciesSiteTest : public ::testing::Test
{
    public:
    SpeciesSiteTest(){};

    void testVector(const Vec3<double> &u, const Vec3<double> &v)
    {
        EXPECT_NEAR(u.x, v.x, 1.0e-6);
        EXPECT_NEAR(u.y, v.y, 1.0e-6);
        EXPECT_NEAR(u.z, v.z, 1.0e-6);
    }
    void testAxes(const Matrix3 &axes, const Vec3<double> &expectedX, const Vec3<double> &expectedY,
                  const Vec3<double> &expectedZ)
    {
        testVector(axes.columnAsVec3(0), expectedX);
        testVector(axes.columnAsVec3(1), expectedY);
        testVector(axes.columnAsVec3(2), expectedZ);
    }
};

TEST_F(SpeciesSiteTest, StaticBasic)
{
    auto &methane = methaneSpecies();

    SpeciesSite site(&methane, SpeciesSite::SiteType::Static);

    // Add carbon atom
    EXPECT_TRUE(site.addStaticOriginAtom(0));
    EXPECT_TRUE(site.addStaticXAxisAtom(0));
    EXPECT_TRUE(site.addStaticYAxisAtom(0));

    // Try to add it again...
    EXPECT_FALSE(site.addStaticOriginAtom(0));
    EXPECT_FALSE(site.addStaticXAxisAtom(0));
    EXPECT_FALSE(site.addStaticYAxisAtom(0));

    // Add some hydrogen atom from their pointers
    EXPECT_TRUE(site.addStaticOriginAtom(&methane.atom(1)));
    EXPECT_TRUE(site.addStaticOriginAtom(&methane.atom(2)));
    EXPECT_TRUE(site.addStaticXAxisAtom(&methane.atom(1)));
    EXPECT_TRUE(site.addStaticXAxisAtom(&methane.atom(2)));
    EXPECT_TRUE(site.addStaticYAxisAtom(&methane.atom(1)));
    EXPECT_TRUE(site.addStaticYAxisAtom(&methane.atom(2)));

    // Add one of those again via its index
    EXPECT_FALSE(site.addStaticOriginAtom(2));
    EXPECT_FALSE(site.addStaticXAxisAtom(2));
    EXPECT_FALSE(site.addStaticYAxisAtom(2));

    // Set (overwrite) atoms
    EXPECT_TRUE(site.setStaticOriginAtoms({&methane.atom(0), &methane.atom(1), &methane.atom(2)}));
    EXPECT_TRUE(site.setStaticXAxisAtoms({&methane.atom(0), &methane.atom(1), &methane.atom(2)}));
    EXPECT_TRUE(site.setStaticYAxisAtoms({&methane.atom(0), &methane.atom(1), &methane.atom(2)}));
}

TEST_F(SpeciesSiteTest, StaticAxes)
{
    auto &methane = methaneSpecies();

    // Set up sites:
    // -- Central atom is the carbon (0)
    // -- H directly up along Y is 1
    // -- H in YZ plane (-ve) is 2
    // -- Along positive X (and negative Y, positive Z) is 3
    // -- Along negative X (and negative Y, positive Z) is 4

    // Site 1 - O(C), X=+Y, Y=-Z, Z=-X
    SpeciesSite site1(&methane, SpeciesSite::SiteType::Static);
    EXPECT_TRUE(site1.addStaticOriginAtom(0));
    EXPECT_TRUE(site1.addStaticXAxisAtom(1));
    EXPECT_TRUE(site1.addStaticYAxisAtom(2));

    // -- Generate the site from the parent species and check its info
    auto sites = site1.createFromParent();
    ASSERT_EQ(sites.size(), 1);
    testVector(sites.front()->origin(), methane.atom(0).r());
    testAxes(sites.front()->axes(), {0.0, 1.0, 0.0}, {0.0, 0.0, -1.0}, {-1.0, 0.0, 0.0});
    // -- Set origin (centre of geometry) to be all atoms rather than just the carbon (expect same result)
    site1.setStaticOriginAtoms({&methane.atom(0), &methane.atom(1), &methane.atom(2), &methane.atom(3), &methane.atom(4)});
    site1.setOriginMassWeighted(false);
    sites = site1.createFromParent();
    ASSERT_EQ(sites.size(), 1);
    testVector(sites.front()->origin(), methane.atom(0).r());
    testAxes(sites.front()->axes(), {0.0, 1.0, 0.0}, {0.0, 0.0, -1.0}, {-1.0, 0.0, 0.0});
    // -- Centre of mass should also be the same
    site1.setOriginMassWeighted(true);
    sites = site1.createFromParent();
    ASSERT_EQ(sites.size(), 1);
    testVector(sites.front()->origin(), methane.atom(0).r());
    testAxes(sites.front()->axes(), {0.0, 1.0, 0.0}, {0.0, 0.0, -1.0}, {-1.0, 0.0, 0.0});

    // Site 2 - O(C), X=+Y, Y=+Z, Z=+X
    SpeciesSite site2(&methane, SpeciesSite::SiteType::Static);
    EXPECT_TRUE(site2.addStaticOriginAtom(0));
    EXPECT_TRUE(site2.addStaticXAxisAtom(1));
    EXPECT_TRUE(site2.addStaticYAxisAtom(3));
    EXPECT_TRUE(site2.addStaticYAxisAtom(4));

    // -- Generate the site from the parent species and check its info
    sites = site2.createFromParent();
    ASSERT_EQ(sites.size(), 1);
    testVector(sites.front()->origin(), methane.atom(0).r());
    testAxes(sites.front()->axes(), {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0});
    // -- Set origin (centre of geometry) to be all atoms rather than just the carbon (expect same result)
    site2.setStaticOriginAtoms({&methane.atom(0), &methane.atom(1), &methane.atom(2), &methane.atom(3), &methane.atom(4)});
    site2.setOriginMassWeighted(false);
    sites = site2.createFromParent();
    ASSERT_EQ(sites.size(), 1);
    testVector(sites.front()->origin(), methane.atom(0).r());
    testAxes(sites.front()->axes(), {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0});
    // -- Centre of mass should also be the same
    site2.setOriginMassWeighted(true);
    sites = site2.createFromParent();
    ASSERT_EQ(sites.size(), 1);
    testVector(sites.front()->origin(), methane.atom(0).r());
    testAxes(sites.front()->axes(), {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0});
}

TEST_F(SpeciesSiteTest, DynamicBasic)
{
    auto &methane = methaneSpecies();

    SpeciesSite site(&methane, SpeciesSite::SiteType::Dynamic);

    // Try adding atoms...
    EXPECT_FALSE(site.addStaticOriginAtom(0));
    EXPECT_FALSE(site.addStaticXAxisAtom(0));
    EXPECT_FALSE(site.addStaticYAxisAtom(0));
    EXPECT_FALSE(site.addStaticOriginAtom(&methane.atom(1)));
    EXPECT_FALSE(site.addStaticXAxisAtom(&methane.atom(1)));
    EXPECT_FALSE(site.addStaticYAxisAtom(&methane.atom(1)));
    EXPECT_FALSE(site.setStaticOriginAtoms({&methane.atom(0), &methane.atom(1), &methane.atom(2)}));
    EXPECT_FALSE(site.setStaticXAxisAtoms({&methane.atom(0), &methane.atom(1), &methane.atom(2)}));
    EXPECT_FALSE(site.setStaticYAxisAtoms({&methane.atom(0), &methane.atom(1), &methane.atom(2)}));

    // Set an element
    EXPECT_TRUE(site.addDynamicElement(Elements::C));
    EXPECT_FALSE(site.addDynamicElement(Elements::C));
    EXPECT_TRUE(site.addDynamicElement(Elements::Zn));
    auto sites = site.createFromParent();
    ASSERT_EQ(sites.size(), 1);
    testVector(sites.front()->origin(), methane.atom(0).r());

    // Set elements vector
    EXPECT_TRUE(site.setDynamicElements({Elements::H}));
    sites = site.createFromParent();
    ASSERT_EQ(sites.size(), 4);
    for (auto n = 1; n < 5; ++n)
        testVector(sites[n - 1]->origin(), methane.atom(n).r());
}

TEST_F(SpeciesSiteTest, FragmentBasic)
{
    auto &benzene = benzeneSpecies();

    SpeciesSite site(&benzene, SpeciesSite::SiteType::Fragment);

    // Set the NETA definition string - no origin
    EXPECT_TRUE(site.setFragmentDefinitionString("?C"));

    // No origin specified, so expect no instances
    EXPECT_EQ(site.instances().size(), 0);

    // Re-set with actual origin on the root match
    EXPECT_TRUE(site.setFragmentDefinitionString("?C,#origin"));

    // Expect six instances, one for each carbon
    EXPECT_EQ(site.instances().size(), 6);
    for (auto &instance : site.instances())
    {
        // There should be 1 atom in the site overall
        EXPECT_EQ(instance.allIndices().size(), 1);
        // There should be a single defined origin atom
        EXPECT_EQ(instance.originIndices().size(), 1);
        // There should be no defined x or y axis atoms
        EXPECT_EQ(instance.xAxisIndices().size(), 0);
        EXPECT_EQ(instance.yAxisIndices().size(), 0);
    }
}

TEST_F(SpeciesSiteTest, FragmentAdvanced)
{
    auto &benzene = benzeneSpecies();

    SpeciesSite site(&benzene, SpeciesSite::SiteType::Fragment);

    // Set the NETA definition string
    EXPECT_TRUE(site.setFragmentDefinitionString("?C, #origin, ring(C(-H),C(-H),C(-H),C(-H),C(-H),C(-H))"));

    // Expect a single instance
    EXPECT_EQ(site.instances().size(), 1);
    const auto &instance = site.instances().front();

    // There should be 12 atoms in the site overall
    EXPECT_EQ(instance.allIndices().size(), 12);
    // There should be a single defined origin atom
    EXPECT_EQ(instance.originIndices().size(), 1);
    // There should be no defined x or y axis atoms
    EXPECT_EQ(instance.xAxisIndices().size(), 0);
    EXPECT_EQ(instance.yAxisIndices().size(), 0);
}

TEST_F(SpeciesSiteTest, FragmentAdvancedAxes)
{
    auto &benzene = benzeneSpecies();

    SpeciesSite site(&benzene, SpeciesSite::SiteType::Fragment);

    // Set the NETA definition string
    EXPECT_TRUE(site.setFragmentDefinitionString("?C, #origin, ring(C(-H),C(-H,#x),C(-H,#x),C(-H),C(-H(#y)),C(-H))"));

    // Expect a single instance
    EXPECT_EQ(site.instances().size(), 1);
    const auto &instance = site.instances().front();

    // There should be 12 atoms in the site overall
    EXPECT_EQ(instance.allIndices().size(), 12);
    // There should be a single defined origin atom
    EXPECT_EQ(instance.originIndices().size(), 1);
    // There should be two atoms defined for the x axis and one for the y
    EXPECT_EQ(instance.xAxisIndices().size(), 2);
    EXPECT_EQ(instance.yAxisIndices().size(), 1);
}

} // namespace UnitTest
