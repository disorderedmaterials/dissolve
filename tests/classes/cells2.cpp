// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/atomType.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class CellsPBCTest : public ::testing::Test
{
    public:
    CellsPBCTest() : dissolve_(coreData_)
    {
        dissolve_.setPairPotentialRange(9.0);
        dissolve_.setAutomaticChargeSource(false);
        dissolve_.setForceChargeSource(true);

        // Add atom types and LJ pair potentials (only one real one - between Ar and Ar)
        auto arType = coreData_.addAtomType(Elements::Ar);
        arType->setName("Ar");
        arType->interactionPotential().setForm(ShortRangeFunctions::Form::Undefined);
        dissolve_.addPairPotential(arType, arType)->setInteractionPotential(Functions1D::Form::None, "");

        // Set up pseudo-species
        argon_ = coreData_.addSpecies();
        argon_->setName("Argon");
        argon_->addAtom(Elements::Ar, {0.0, 0.0, 0.0}, 0.0);
        argon_->atom(0).setAtomType(arType);
    }

    protected:
    CoreData coreData_;
    Dissolve dissolve_;
    Species *argon_;

    protected:
    Configuration *createConfiguration(const Vec3<double> &lengths, const Vec3<double> &angles, const Vec3<double> &origin,
                                       int nMolecules = 1000)
    {
        // Setup Configuration
        auto *cfg = coreData_.addConfiguration();
        cfg->createBoxAndCells(lengths, angles, false, 12.0);

        // Add a molecule at the origin
        auto central = cfg->addMolecule(argon_);
        central->atom(0)->setCoordinates(origin);

        // Add surrounding molecules on a sphere with radius equal to the inscribed sphere radius
        const auto r = cfg->box()->inscribedSphereRadius();
        for (auto n = 0; n < nMolecules; ++n)
        {
            auto theta = DissolveMath::random() * M_PI;
            auto phi = DissolveMath::random() * 2.0 * M_PI;
            auto mol = cfg->addMolecule(argon_);
            mol->atom(0)->setCoordinates(Vec3<double>(r * sin(theta) * cos(phi), r * sin(theta) * sin(phi), r * cos(theta)) +
                                         origin);
        }

        cfg->updateObjectRelationships();

        return cfg;
    }

    void testCells(Configuration *cfg)
    {
        auto *box = cfg->box();
        const auto r = cfg->box()->inscribedSphereRadius();

        // Get reference atom in the central molecule
        const auto &central = cfg->molecules().front();
        auto ii = central->atom(0);

        for (auto n = 1; n < cfg->nMolecules(); ++n)
        {
            const auto &mol = cfg->molecule(n);

            auto jj = mol->atom(0);
            EXPECT_NEAR(box->minimumDistance(ii->r(), jj->r()), r, 1.0e-6);

            // Range check between the central and surrounding atom should always succeed
            EXPECT_TRUE(cfg->cells().withinMinimumImageRange(ii->cell(), jj->cell(), r));
        }
    }

    void testNeighbours(Configuration *cfg)
    {
        auto &cellArray = cfg->cells();

        // Every cell must have a set of neighbours containing no duplicate indices
        for (auto n = 0; n < cellArray.nCells(); ++n)
        {
            const auto &nbrs = cellArray.neighbours(*cellArray.cell(n));

            dissolve::for_each_pair(ParallelPolicies::seq, nbrs.begin(), nbrs.end(),
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
    auto *cfg = createConfiguration({100, 100, 100}, {90, 90, 90}, {0, 0, 0});
    ASSERT_TRUE(cfg->box()->type() == Box::BoxType::Cubic);
    testCells(cfg);
    testNeighbours(cfg);
};

TEST_F(CellsPBCTest, MonoclinicAlpha)
{
    auto *cfg = createConfiguration({100, 100, 100}, {100, 90, 90}, {0, 0, 0});
    ASSERT_TRUE(cfg->box()->type() == Box::BoxType::MonoclinicAlpha);
    testCells(cfg);
    testNeighbours(cfg);
};

TEST_F(CellsPBCTest, MonoclinicBeta)
{
    auto *cfg = createConfiguration({100, 100, 100}, {90, 110, 90}, {0, 0, 0});
    ASSERT_TRUE(cfg->box()->type() == Box::BoxType::MonoclinicBeta);
    testCells(cfg);
    testNeighbours(cfg);
};

TEST_F(CellsPBCTest, MonoclinicGamma)
{
    auto *cfg = createConfiguration({100, 100, 100}, {90, 90, 120}, {0, 0, 0});
    ASSERT_TRUE(cfg->box()->type() == Box::BoxType::MonoclinicGamma);
    testCells(cfg);
    testNeighbours(cfg);
};

} // namespace UnitTest
