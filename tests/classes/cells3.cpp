// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/atomType.h"
#include "classes/species.h"
#include "kernels/producer.h"
#include "main/dissolve.h"
#include "templates/algorithms.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class CellsEnergyTest : public ::testing::Test
{
    public:
    CellsEnergyTest() : dissolve_(coreData_)
    {
        dissolve_.setPairPotentialRange(20.0);
        dissolve_.setAutomaticChargeSource(false);
        dissolve_.setForceChargeSource(true);
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
    Configuration *createConfiguration(const Vec3<double> &lengths, const Vec3<double> &angles, int nMolecules)
    {
        // Setup Configuration
        auto *cfg = coreData_.addConfiguration();
        cfg->createBoxAndCells(lengths, angles, false, dissolve_.pairPotentialRange());

        // Add molecules
        for (auto n = 0; n < nMolecules; ++n)
            cfg->addMolecule(argon_);
        cfg->updateObjectRelationships();

        return cfg;
    }
    // Calculate tabulated energy directly (without using Cells)
    double tabulatedEnergyNoCells(Configuration *cfg, double cutoff)
    {
        auto *box = cfg->box();
        auto *pp = dissolve_.pairPotential("Ar", "Ar");
        auto energy = 0.0;
        dissolve::for_each_pair(ParallelPolicies::seq, cfg->molecules().begin(), cfg->molecules().end(),
                                [&](int i, const auto &molI, int j, const auto &molJ)
                                {
                                    if (i == j)
                                        return;
                                    auto ii = molI->atom(0);
                                    auto jj = molJ->atom(0);

                                    auto r = box->minimumDistance(ii->r(), jj->r());
                                    if (r <= cutoff)
                                        energy += pp->energy(r);
                                });
        return energy;
    }
    // Calculate analytic energy directly (without using Cells)
    double analyticEnergyNoCells(Configuration *cfg, double cutoff)
    {
        auto *box = cfg->box();
        auto *pp = dissolve_.pairPotential("Ar", "Ar");
        auto energy = 0.0;
        dissolve::for_each_pair(ParallelPolicies::seq, cfg->molecules().begin(), cfg->molecules().end(),
                                [&](int i, const auto &molI, int j, const auto &molJ)
                                {
                                    if (i == j)
                                        return;
                                    auto ii = molI->atom(0);
                                    auto jj = molJ->atom(0);

                                    auto r = box->minimumDistance(ii->r(), jj->r());
                                    if (r <= cutoff)
                                        energy += pp->analyticEnergy(r, 0.0, 1.0);
                                });
        return energy;
    }
    // Test energy for configuration
    void testEnergy(Configuration *cfg, const std::tuple<double, double, double, double> &state)
    {
        auto [rCut, cellSize, refEnergy, lrc] = state;

        // Prepare the main simulation to regenerate pair potentials etc.
        ASSERT_TRUE(dissolve_.prepare());

        // Initialise an EnergyKernel with the specified cutoff
        auto kernel = KernelProducer::energyKernel(cfg, dissolve_.worldPool(), dissolve_.potentialMap(), rCut);

        // Regenerate cells to new size spec and re-assign atoms
        cfg->cells().generate(cfg->box(), cellSize, dissolve_.pairPotentialRange());
        cfg->updateAtomLocations(true);

        // Calculate total Cell-based energy
        EXPECT_NEAR(analyticEnergyNoCells(cfg, rCut), tabulatedEnergyNoCells(cfg, rCut), 1.0e-2);
        EXPECT_NEAR(tabulatedEnergyNoCells(cfg, rCut),
                    kernel->totalPairPotentialEnergy(false, ProcessPool::PoolStrategy).total(), 1.0e-6);
        EXPECT_NEAR(refEnergy - lrc, kernel->totalPairPotentialEnergy(false, ProcessPool::PoolStrategy).total(), 1.65e-2);
    }
};

TEST_F(CellsEnergyTest, Cubic)
{
    auto *cfg = createConfiguration({100, 100, 100}, {90, 90, 90}, 6755);

    // Load the test coordinates
    CoordinateImportFileFormat importer("dlpoly/argon/cubic/big_argon.CONFIG",
                                        CoordinateImportFileFormat::CoordinateImportFormat::DLPOLY);
    ASSERT_TRUE(importer.importData(cfg));

    // Test range of cell sizes for various cutoffs - should make no difference to total energy
    std::vector<std::tuple<double, double, double, double>> states = {
        {9.0, 5.0, -8.077004e3, -0.678949e3},  {9.0, 7.0, -8.077004e3, -0.678949e3},   {9.0, 9.0, -8.077004e3, -0.678949e3},
        {9.0, 12.0, -8.077004e3, -0.678949e3}, {12.0, 5.0, -8.075795e3, -0.286679e3},  {12.0, 7.0, -8.075795e3, -0.286679e3},
        {12.0, 9.0, -8.075795e3, -0.286679e3}, {12.0, 12.0, -8.075795e3, -0.286679e3}, {15.0, 5.0, -8.075748e3, -0.146800e3},
        {15.0, 7.0, -8.075748e3, -0.146800e3}, {15.0, 9.0, -8.075748e3, -0.146800e3},  {15.0, 15.0, -8.075748e3, -0.146800e3}};

    for (auto &state : states)
        testEnergy(cfg, state);
}

TEST_F(CellsEnergyTest, Monoclinic)
{
    auto *cfg = createConfiguration({100, 100, 100}, {90, 90, 120}, 6802);

    // Load the test coordinates
    CoordinateImportFileFormat importer("dlpoly/argon/monoclinic/big_argon.CONFIG",
                                        CoordinateImportFileFormat::CoordinateImportFormat::DLPOLY);
    ASSERT_TRUE(importer.importData(cfg));

    // Test range of cell sizes for various cutoffs - should make no difference to total energy
    std::vector<std::tuple<double, double, double, double>> states = {
        {9.0, 5.0, -9.747556e3, -0.794931e3},  {9.0, 7.0, -9.747556e3, -0.794931e3},   {9.0, 9.0, -9.747556e3, -0.794931e3},
        {9.0, 12.0, -9.747556e3, -0.794931e3}, {12.0, 5.0, -9.748103e3, -0.335651e3},  {12.0, 7.0, -9.748103e3, -0.335651e3},
        {12.0, 9.0, -9.748103e3, -0.335651e3}, {12.0, 12.0, -9.748103e3, -0.335651e3}, {15.0, 5.0, -9.747836e3, -0.171877e3},
        {15.0, 7.0, -9.747836e3, -0.171877e3}, {15.0, 9.0, -9.747836e3, -0.171877e3},  {15.0, 15.0, -9.747836e3, -0.171877e3}};

    for (auto &state : states)
        testEnergy(cfg, state);
}

TEST_F(CellsEnergyTest, TriclinicBox)
{
    auto *cfg = createConfiguration({100, 100, 100}, {80, 70, 60}, 6528);

    // Load the test coordinates
    CoordinateImportFileFormat importer("dlpoly/argon/triclinic/big_argon.CONFIG",
                                        CoordinateImportFileFormat::CoordinateImportFormat::DLPOLY);
    ASSERT_TRUE(importer.importData(cfg));

    // Test range of cell sizes for various cutoffs - should make no difference to total energy
    std::vector<std::tuple<double, double, double, double>> states = {
        {9.0, 5.0, -9.471742e3, -0.779171e3},  {9.0, 7.0, -9.471742e3, -0.779171e3},   {9.0, 9.0, -9.471742e3, -0.779171e3},
        {9.0, 12.0, -9.471742e3, -0.779171e3}, {12.0, 5.0, -9.471045e3, -0.328997e3},  {12.0, 7.0, -9.471045e3, -0.328997e3},
        {12.0, 9.0, -9.471045e3, -0.328997e3}, {12.0, 12.0, -9.471045e3, -0.328997e3}, {15.0, 5.0, -9.470832e3, -0.168470e3},
        {15.0, 7.0, -9.470832e3, -0.168470e3}, {15.0, 9.0, -9.470832e3, -0.168470e3},  {15.0, 15.0, -9.470832e3, -0.168470e3}};

    for (auto &state : states)
        testEnergy(cfg, state);
}
} // namespace UnitTest
