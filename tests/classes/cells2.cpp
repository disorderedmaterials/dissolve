// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/atomType.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include "math/mathFunc.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class CellsPBCTest : public ::testing::Test
{
    public:
    CellsPBCTest()
    {
        PairPotential::setRange(9.0);
        PairPotential::setChargeSource(PairPotential::ChargeSource::AtomTypes);

        // Set up pseudo-species
        probe_.setName("Probe");
        probe_.addAtom(Elements::H, {0.0, 0.0, 0.0}, 0.0, probe_.addAtomType(Elements::H, "Probe"));
    }

    protected:
    Species probe_;
    Configuration configuration_;

    protected:
    void createConfiguration(const Vector3 &lengths, const Vector3 &angles, const Vector3 &origin, int nMolecules = 1000)
    {
        // Setup Configuration
        configuration_.createBoxAndCells(lengths, angles, false);

        // Add a molecule at the origin
        auto central = configuration_.addMolecule(&probe_);
        central->atom(0)->setCoordinates(origin);

        // Add surrounding molecules on a sphere with radius equal to the inscribed sphere radius
        const auto r = configuration_.box()->inscribedSphereRadius();
        for (auto n = 0; n < nMolecules; ++n)
        {
            auto theta = DissolveMath::random() * M_PI;
            auto phi = DissolveMath::random() * 2.0 * M_PI;
            auto mol = configuration_.addMolecule(&probe_);
            mol->atom(0)->setCoordinates(Vector3(r * sin(theta) * cos(phi), r * sin(theta) * sin(phi), r * cos(theta)) +
                                         origin);
        }

        configuration_.updateObjectRelationships();
    }

    void testCells()
    {
        auto *box = configuration_.box();
        const auto r = configuration_.box()->inscribedSphereRadius();

        // Get reference atom in the central molecule
        const auto &central = configuration_.molecules().front();
        auto ii = central->atom(0);

        for (auto n = 1; n < configuration_.nMolecules(); ++n)
        {
            const auto &mol = configuration_.molecule(n);

            auto jj = mol->atom(0);
            EXPECT_NEAR(box->minimumDistance(ii->r(), jj->r()), r, 1.0e-6);

            // Range check between the central and surrounding atom should always succeed
            EXPECT_TRUE(configuration_.cells().withinMinimumImageRange(ii->cell(), jj->cell(), r));
        }
    }

    void testNeighbours()
    {
        auto &cellArray = configuration_.cells();

        // Every cell must have a set of neighbours containing no duplicate indices
        for (auto n = 0; n < cellArray.nCells(); ++n)
        {
            const auto &nbrs = cellArray.neighbours(*cellArray.cell(n));

            dissolve::for_each_pair(ParallelPolicies::seq, nbrs,
                                    [&](auto i, auto &nbri, auto j, auto &nbrj)
                                    {
                                        if (i != j)
                                            EXPECT_TRUE(nbri.cell.index() != nbrj.cell.index());
                                    });
        }
    }
};

TEST_F(CellsPBCTest, Cubic)
{
    createConfiguration({100, 100, 100}, {90, 90, 90}, {0, 0, 0});
    ASSERT_TRUE(configuration_.box()->type() == Box::BoxType::Cubic);
    testCells();
    testNeighbours();
};

TEST_F(CellsPBCTest, MonoclinicAlpha)
{
    createConfiguration({100, 100, 100}, {100, 90, 90}, {0, 0, 0});
    ASSERT_TRUE(configuration_.box()->type() == Box::BoxType::MonoclinicAlpha);
    testCells();
    testNeighbours();
};

TEST_F(CellsPBCTest, MonoclinicBeta)
{
    createConfiguration({100, 100, 100}, {90, 110, 90}, {0, 0, 0});
    ASSERT_TRUE(configuration_.box()->type() == Box::BoxType::MonoclinicBeta);
    testCells();
    testNeighbours();
};

TEST_F(CellsPBCTest, MonoclinicGamma)
{
    createConfiguration({100, 100, 100}, {90, 90, 120}, {0, 0, 0});
    ASSERT_TRUE(configuration_.box()->type() == Box::BoxType::MonoclinicGamma);
    testCells();
    testNeighbours();
};

} // namespace UnitTest
