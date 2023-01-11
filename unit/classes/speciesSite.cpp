// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/site.h"
#include "classes/species.h"
#include "unit/data/species.h"
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
    EXPECT_TRUE(site.addOriginAtom(0));
    EXPECT_TRUE(site.addXAxisAtom(0));
    EXPECT_TRUE(site.addYAxisAtom(0));

    // Try to add it again...
    EXPECT_FALSE(site.addOriginAtom(0));
    EXPECT_FALSE(site.addXAxisAtom(0));
    EXPECT_FALSE(site.addYAxisAtom(0));

    // Add some hydrogen atom from their pointers
    EXPECT_TRUE(site.addOriginAtom(&methane.atom(1)));
    EXPECT_TRUE(site.addOriginAtom(&methane.atom(2)));
    EXPECT_TRUE(site.addXAxisAtom(&methane.atom(1)));
    EXPECT_TRUE(site.addXAxisAtom(&methane.atom(2)));
    EXPECT_TRUE(site.addYAxisAtom(&methane.atom(1)));
    EXPECT_TRUE(site.addYAxisAtom(&methane.atom(2)));

    // Add one of those again via its index
    EXPECT_FALSE(site.addOriginAtom(2));
    EXPECT_FALSE(site.addXAxisAtom(2));
    EXPECT_FALSE(site.addYAxisAtom(2));

    // Set (overwrite) atoms
    EXPECT_TRUE(site.setOriginAtoms({&methane.atom(0), &methane.atom(1), &methane.atom(2)}));
    EXPECT_TRUE(site.setXAxisAtoms({&methane.atom(0), &methane.atom(1), &methane.atom(2)}));
    EXPECT_TRUE(site.setYAxisAtoms({&methane.atom(0), &methane.atom(1), &methane.atom(2)}));
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
    EXPECT_TRUE(site1.addOriginAtom(0));
    EXPECT_TRUE(site1.addXAxisAtom(1));
    EXPECT_TRUE(site1.addYAxisAtom(2));

    // -- Generate the site from the parent species and check its info
    auto sites = site1.createFromParent();
    ASSERT_EQ(sites.size(), 1);
    testVector(sites.front()->origin(), methane.atom(0).r());
    testAxes(sites.front()->axes(), {0.0, 1.0, 0.0}, {0.0, 0.0, -1.0}, {-1.0, 0.0, 0.0});
    // -- Set origin (centre of geometry) to be all atoms rather than just the carbon (expect same result)
    site1.setOriginAtoms({&methane.atom(0), &methane.atom(1), &methane.atom(2), &methane.atom(3), &methane.atom(4)});
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
    EXPECT_TRUE(site2.addOriginAtom(0));
    EXPECT_TRUE(site2.addXAxisAtom(1));
    EXPECT_TRUE(site2.addYAxisAtom(3));
    EXPECT_TRUE(site2.addYAxisAtom(4));

    // -- Generate the site from the parent species and check its info
    sites = site2.createFromParent();
    ASSERT_EQ(sites.size(), 1);
    testVector(sites.front()->origin(), methane.atom(0).r());
    testAxes(sites.front()->axes(), {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0});
    // -- Set origin (centre of geometry) to be all atoms rather than just the carbon (expect same result)
    site2.setOriginAtoms({&methane.atom(0), &methane.atom(1), &methane.atom(2), &methane.atom(3), &methane.atom(4)});
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
    EXPECT_FALSE(site.addOriginAtom(0));
    EXPECT_FALSE(site.addXAxisAtom(0));
    EXPECT_FALSE(site.addYAxisAtom(0));
    EXPECT_FALSE(site.addOriginAtom(&methane.atom(1)));
    EXPECT_FALSE(site.addXAxisAtom(&methane.atom(1)));
    EXPECT_FALSE(site.addYAxisAtom(&methane.atom(1)));
    EXPECT_FALSE(site.setOriginAtoms({&methane.atom(0), &methane.atom(1), &methane.atom(2)}));
    EXPECT_FALSE(site.setXAxisAtoms({&methane.atom(0), &methane.atom(1), &methane.atom(2)}));
    EXPECT_FALSE(site.setYAxisAtoms({&methane.atom(0), &methane.atom(1), &methane.atom(2)}));
}

} // namespace UnitTest
