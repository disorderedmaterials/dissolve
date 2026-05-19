// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "io/import/coordinates.h"
#include "kernels/producer.h"
#include "templates/algorithms.h"
#include "tests/graphData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class CellsMIMTest : public ::testing::Test
{
    public:
    CellsMIMTest()
    {
        PairPotential::setRange(20.0);
        PairPotential::setChargeSource(PairPotential::ChargeSource::AtomTypes);
        PairPotential::setShortRangeTruncationScheme(PairPotential::NoShortRangeTruncation);
    }

    protected:
    TestGraph testGraph_;

    protected:
    // Set up graph
    Configuration *setUp(const Vector3 &lengths, const Vector3 &angles, int nMolecules, std::string referenceCoordinates)
    {
        EXPECT_TRUE(
            testGraph_.createConfiguration("Box", {{"Ar|epsilon=0.774040 sigma=3.445996", nMolecules}}, lengths, angles));
        EXPECT_TRUE(testGraph_.appendSetCoordinates("ImportDLPOLYStructure", referenceCoordinates));

        // Run the graph from the head node to set up the configuration
        EXPECT_EQ(testGraph_.fetchHead()->run(), NodeConstants::ProcessResult::Success);
        EXPECT_EQ(testGraph_.fetchHead()->versionIndex(), 0);

        return testGraph_.fetchHead()->getOutputValue<Configuration *>("Configuration");
    }
    // Count number of atoms within range of a target atom in the box without using cells
    int atomsWithRangeNoCells(Configuration *cfg, int fromIndex, double cutoff)
    {
        auto *box = cfg->box();
        const auto &i = cfg->atom(fromIndex);
        return std::count_if(cfg->atoms().begin(), cfg->atoms().end(),
                             [&](const auto &j)
                             {
                                 if (&i == &j)
                                     return false;
                                 return (box->minimumDistance(i.r(), j.r()) <= cutoff);
                             });
    }
    // Count number of atoms within range of a target atom in the box using cells
    int atomsWithRangeWithCells(Configuration *cfg, int fromIndex, double cutoff)
    {
        auto *box = cfg->box();
        const auto &i = cfg->atom(fromIndex);
        const auto *iCell = i.cell();
        auto count = 0;
        for (auto &jCell : cfg->cells().cells())
        {
            // Check whether this cell is within range of the central one
            if (cfg->cells().withinMinimumImageRange(iCell, &jCell, cutoff))
            {
                count += std::count_if(jCell.atoms().begin(), jCell.atoms().end(),
                                       [&](const auto &j)
                                       {
                                           if (&i == j)
                                               return false;
                                           return (box->minimumDistance(i.r(), j->r()) <= cutoff);
                                       });
            }
        }
        return count;
    }
};

TEST_F(CellsMIMTest, Cubic)
{
    auto *cfg = setUp({100, 100, 100}, {90, 90, 90}, 6755, "dlpoly/argon/cubic/big_argon.CONFIG");
    ASSERT_TRUE(cfg);

    // Regenerate cells to new size spec and re-assign atoms
    cfg->cells().generate(cfg->box(), 7.0);
    cfg->updateAtomLocations(true);

    auto cutoff = cfg->box()->inscribedSphereRadius();
    for (auto id = 0; id < cfg->nAtoms(); ++id)
        EXPECT_EQ(atomsWithRangeNoCells(cfg, id, cutoff), atomsWithRangeWithCells(cfg, id, cutoff));
}

TEST_F(CellsMIMTest, Monoclinic)
{
    auto *cfg = setUp({100, 100, 100}, {90, 90, 120}, 6802, "dlpoly/argon/monoclinic/big_argon.CONFIG");
    ASSERT_TRUE(cfg);

    // Regenerate cells to new size spec and re-assign atoms
    cfg->cells().generate(cfg->box(), 7.0);
    cfg->updateAtomLocations(true);

    auto cutoff = cfg->box()->inscribedSphereRadius();
    for (auto id = 0; id < cfg->nAtoms(); ++id)
        EXPECT_EQ(atomsWithRangeNoCells(cfg, id, cutoff), atomsWithRangeWithCells(cfg, id, cutoff));
}

TEST_F(CellsMIMTest, TriclinicBox)
{
    auto *cfg = setUp({100, 100, 100}, {80, 70, 60}, 6528, "dlpoly/argon/triclinic/big_argon.CONFIG");
    ASSERT_TRUE(cfg);

    // Regenerate cells to new size spec and re-assign atoms
    cfg->cells().generate(cfg->box(), 7.0);
    cfg->updateAtomLocations(true);

    auto cutoff = cfg->box()->inscribedSphereRadius();
    for (auto id = 0; id < cfg->nAtoms(); ++id)
        EXPECT_EQ(atomsWithRangeNoCells(cfg, id, cutoff), atomsWithRangeWithCells(cfg, id, cutoff));
}
} // namespace UnitTest
