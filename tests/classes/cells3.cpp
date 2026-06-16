// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "kernels/producer.h"
#include "templates/algorithms.h"
#include "tests/graphData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class CellsEnergyTest : public ::testing::Test
{
    public:
    CellsEnergyTest()
    {
        PairPotential::setRange(20.0);
        PairPotential::setChargeSource(PairPotential::ChargeSource::AtomTypes);
        PairPotential::setShortRangeTruncationScheme(PairPotential::NoShortRangeTruncation);
    }

    protected:
    TestGraph testGraph_;
    const AtomType *atomType_{nullptr};

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

        // Get the species atom type for pair potential generation
        auto arSpeciesNode = dynamic_cast<SpeciesNode *>(testGraph_.findNode("Ar"));
        EXPECT_TRUE(arSpeciesNode);
        atomType_ = arSpeciesNode->species().atom(0).atomType();

        return testGraph_.fetchHead()->getOutputValue<Configuration *>("Configuration");
    }
    // Calculate tabulated energy directly (without using Cells)
    double tabulatedEnergyNoCells(Configuration *cfg, const PairPotential &pairPotential, double cutoffSq)
    {
        auto &box = cfg->box();
        auto energy = 0.0;
        dissolve::for_each_pair(ParallelPolicies::seq, cfg->molecules(),
                                [&](int i, const auto &molI, int j, const auto &molJ)
                                {
                                    if (i == j)
                                        return;
                                    auto ii = molI->atom(0);
                                    auto jj = molJ->atom(0);

                                    auto rSq = box.minimumDistanceSquared(ii->r(), jj->r());
                                    if (rSq <= cutoffSq)
                                        energy += pairPotential.energy(sqrt(rSq));
                                });
        return energy;
    }
    // Calculate analytic energy directly (without using Cells)
    double analyticEnergyNoCells(Configuration *cfg, const PairPotential &pairPotential, double cutoffSq)
    {
        auto &box = cfg->box();
        auto energy = 0.0;
        dissolve::for_each_pair(ParallelPolicies::seq, cfg->molecules(),
                                [&](int i, const auto &molI, int j, const auto &molJ)
                                {
                                    if (i == j)
                                        return;
                                    auto ii = molI->atom(0);
                                    auto jj = molJ->atom(0);

                                    auto rSq = box.minimumDistanceSquared(ii->r(), jj->r());
                                    if (rSq <= cutoffSq)
                                        energy += pairPotential.analyticEnergy(sqrt(rSq), 0.0, 1.0);
                                });
        return energy;
    }
    // Test energy for configuration
    void testEnergy(Configuration *cfg, const std::tuple<double, double, double, double> &state)
    {
        auto [rCut, cellSize, refEnergy, lrc] = state;

        // Regenerate cells to new size spec and re-assign atoms
        cfg->cells().generate(cfg->box(), cellSize);
        cfg->updateAtomLocations(true);

        // Set pair potential range and initialise an EnergyKernel
        PairPotential::setRange(rCut);
        auto kernel = testGraph_.createEnergyKernel(cfg);

        // Create a reference potential
        auto optPotential = ShortRangeFunctions::combine(atomType_->interactionPotential(), atomType_->interactionPotential());
        EXPECT_TRUE(optPotential);
        auto pairPotential = PairPotential(atomType_->name(), atomType_->name(), *optPotential);
        pairPotential.tabulate();

        // Calculate total Cell-based energy
        auto tabulated = tabulatedEnergyNoCells(cfg, pairPotential, rCut * rCut);
        auto production = kernel->totalPairPotentialEnergy(true, false);
        EXPECT_NEAR(analyticEnergyNoCells(cfg, pairPotential, rCut * rCut), tabulated, 1.0e-2);
        EXPECT_NEAR(tabulated, production.total(), 1.0e-6);
        EXPECT_NEAR(refEnergy - lrc, production.total(), 1.65e-2);
    }
};

TEST_F(CellsEnergyTest, Cubic)
{
    auto *cfg = setUp({100, 100, 100}, {90, 90, 90}, 6755, "dlpoly/argon/cubic/big_argon.CONFIG");
    ASSERT_TRUE(cfg);

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
    auto *cfg = setUp({100, 100, 100}, {90, 90, 120}, 6802, "dlpoly/argon/monoclinic/big_argon.CONFIG");
    ASSERT_TRUE(cfg);

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
    auto *cfg = setUp({100, 100, 100}, {80, 70, 60}, 6528, "dlpoly/argon/triclinic/big_argon.CONFIG");
    ASSERT_TRUE(cfg);

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
