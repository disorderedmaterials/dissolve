// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/atomType.h"
#include "classes/species.h"
#include "io/import/coordinates.h"
#include "kernels/producer.h"
#include "main/dissolve.h"
#include "templates/algorithms.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class CellsMIMTest : public ::testing::Test
{
    public:
    CellsMIMTest() : dissolve_(coreData_)
    {
        PairPotential::setRange(20.0);
        PairPotential::setChargeSource(PairPotential::ChargeSource::AtomTypes);
        PairPotential::setShortRangeTruncationScheme(PairPotential::NoShortRangeTruncation);

        // Add atom type
        auto arType = coreData_.addAtomType(Elements::Ar);
        arType->setName("Ar");
        arType->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::LennardJones,
                                                            "epsilon=0.77404 sigma=3.445996");

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
    // Create skeletal target Configuration
    Configuration *createConfiguration(const Vector3 &lengths, const Vector3 &angles, int nMolecules)
    {
        // Setup Configuration
        auto *cfg = coreData_.addConfiguration();
        cfg->createBoxAndCells(lengths, angles, false);

        // Add molecules
        for (auto n = 0; n < nMolecules; ++n)
            cfg->addMolecule(argon_);
        cfg->updateObjectRelationships();

        return cfg;
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
    auto *cfg = createConfiguration({100, 100, 100}, {90, 90, 90}, 6755);

    // Load the test coordinates
    CoordinateImportFileFormat importer("dlpoly/argon/cubic/big_argon.CONFIG",
                                        CoordinateImportFileFormat::CoordinateImportFormat::DLPOLY);
    ASSERT_TRUE(importer.importData(cfg));

    auto cutoff = cfg->box()->inscribedSphereRadius();
    for (auto id = 0; id < cfg->nAtoms(); ++id)
        EXPECT_EQ(atomsWithRangeNoCells(cfg, id, cutoff), atomsWithRangeWithCells(cfg, id, cutoff));
}

TEST_F(CellsMIMTest, Monoclinic)
{
    auto *cfg = createConfiguration({100, 100, 100}, {90, 90, 120}, 6802);

    // Load the test coordinates
    CoordinateImportFileFormat importer("dlpoly/argon/monoclinic/big_argon.CONFIG",
                                        CoordinateImportFileFormat::CoordinateImportFormat::DLPOLY);
    ASSERT_TRUE(importer.importData(cfg));

    auto cutoff = cfg->box()->inscribedSphereRadius();
    for (auto id = 0; id < cfg->nAtoms(); ++id)
        EXPECT_EQ(atomsWithRangeNoCells(cfg, id, cutoff), atomsWithRangeWithCells(cfg, id, cutoff));
}

TEST_F(CellsMIMTest, TriclinicBox)
{
    auto *cfg = createConfiguration({100, 100, 100}, {80, 70, 60}, 6528);

    // Load the test coordinates
    CoordinateImportFileFormat importer("dlpoly/argon/triclinic/big_argon.CONFIG",
                                        CoordinateImportFileFormat::CoordinateImportFormat::DLPOLY);
    ASSERT_TRUE(importer.importData(cfg));

    auto cutoff = cfg->box()->inscribedSphereRadius();
    for (auto id = 0; id < cfg->nAtoms(); ++id)
        EXPECT_EQ(atomsWithRangeNoCells(cfg, id, cutoff), atomsWithRangeWithCells(cfg, id, cutoff));
}
} // namespace UnitTest
