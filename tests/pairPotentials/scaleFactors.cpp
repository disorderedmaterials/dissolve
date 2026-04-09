// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/coreData.h"
#include "classes/localMolecule.h"
#include "classes/potentialMap.h"
#include "classes/species.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class PairPotentialsScaleFactorsTest : public ::testing::Test
{
    public:
    PairPotentialsScaleFactorsTest()
    {
        // Set up atom types
        atC1_ = species_.addAtomType(Elements::C);
        atC1_->setName("C1");
        atC1_->setCharge(-0.1);
        atC1_->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::LennardJones, ljParameters);
        atC2_ = species_.addAtomType(Elements::C);
        atC2_->setCharge(0.1);
        atC2_->setName("C2");
        atC2_->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::LennardJones, ljParameters);

        // Create Species
        species_.addAtom(Elements::C, {-1.390000, 0.000000, 0.000000}, -0.2, atC1_);
        species_.addAtom(Elements::C, {-0.695000, 1.203775, 0.000000}, 0.2, atC2_);
        species_.addAtom(Elements::C, {0.695000, 1.203775, 0.000000}, 0.2, atC2_);
        species_.addAtom(Elements::C, {1.390000, 0.000000, 0.000000}, -0.2, atC1_);
        species_.addMissingBonds();
        torsion_ = species_.addTorsion(0, 1, 2, 3);
        species_.setUpScaledInteractions();

        // Create a molecule based on the species
        molecule_.setSpecies(&species_);
        for (auto &&[molAtom, spAtom] : zip(molecule_.localAtoms(), species_.atoms()))
        {
            molAtom.setCoordinates(spAtom.r());
            molAtom.setConfigurationTypeIndex(spAtom.atomType()->index());
        }

        // Set up the function wrapper
        potentialWrapper_.setFormAndParameters(interactionPotential_.form(), interactionPotential_.parameters());
    }
    // Set up potentials
    void setUpPotentials(bool useAtomTypeCharges)
    {
        PairPotential::setRange(15.0, 0.001);

        potentialMap_.clear();
        pairPotentials_.clear();
        PairPotential::setIncludeCoulombPotential(useAtomTypeCharges);
        PairPotential::setShortRangeTruncationScheme(PairPotential::NoShortRangeTruncation);
        PairPotential::setCoulombTruncationScheme(PairPotential::NoCoulombTruncation);

        // Set up pair potentials
        // auto *pp11 = std::get<2>(pairPotentials_.emplace_back(atC1_, atC1_,
        //                                                       std::make_unique<PairPotential>(atC1_->name(), atC1_->name())))
        //                  .get();
        // pp11->setInteractionPotential(interactionPotential_);
        pairPotentials_.set(atC1_->name(), atC1_->name(), {atC1_->name(), atC1_->name(), interactionPotential_});
        pairPotentials_.get(atC1_->name(), atC1_->name()).setLocalChargeProduct(atC1_->charge() * atC1_->charge());

        pairPotentials_.set(atC1_->name(), atC2_->name(), {atC1_->name(), atC2_->name(), interactionPotential_});
        pairPotentials_.get(atC1_->name(), atC2_->name()).setLocalChargeProduct(atC1_->charge() * atC2_->charge());

        pairPotentials_.set(atC2_->name(), atC2_->name(), {atC2_->name(), atC2_->name(), interactionPotential_});
        pairPotentials_.get(atC2_->name(), atC2_->name()).setLocalChargeProduct(atC2_->charge() * atC2_->charge());

        for (auto &potential : std::views::values(pairPotentials_))
            potential.tabulate();

        // Create PotentialMap
        potentialMap_ = PotentialMap(species_.atomTypesRaw(), pairPotentials_);
    }
    // Return reference energy at distance r given specified charge product and scalings
    double referenceEnergy(double r, double chargeProduct, double elecScale = 1.0, double srScale = 1.0)
    {
        return srScale * potentialWrapper_.y(r) + elecScale * PairPotential::CoulConvert * chargeProduct / r;
    }
    // Return reference force at distance r given specified charge product and scalings
    double referenceForce(double r, double chargeProduct, double elecScale = 1.0, double srScale = 1.0)
    {
        return -srScale * potentialWrapper_.dYdX(r) + elecScale * PairPotential::CoulConvert * chargeProduct / (r * r);
    }
    // Perform scaling tests on production routines
    template <class Particle> void testScalings(const Particle &i, const Particle &j, double r, double refChargeProduct)
    {
        // No scaling
        EXPECT_NEAR(potentialMap_.energy(i, j, r), referenceEnergy(r, refChargeProduct), testTolerance_);
        EXPECT_NEAR(potentialMap_.force(i, j, r), referenceForce(r, refChargeProduct), testTolerance_);

        auto elecScale = 0.5, srScale = 0.5;

        // Equal scaling for short-range and electrostatics
        EXPECT_NEAR(potentialMap_.energy(i, j, r, elecScale, srScale), referenceEnergy(r, refChargeProduct, elecScale, srScale),
                    testTolerance_);
        EXPECT_NEAR(potentialMap_.force(i, j, r, elecScale, srScale), referenceForce(r, refChargeProduct, elecScale, srScale),
                    testTolerance_);

        // Unlike scalings
        elecScale = 0.25;
        srScale = 0.75;
        EXPECT_NEAR(potentialMap_.energy(i, j, r, elecScale, srScale), referenceEnergy(r, refChargeProduct, elecScale, srScale),
                    testTolerance_);
        EXPECT_NEAR(potentialMap_.force(i, j, r, elecScale, srScale), referenceForce(r, refChargeProduct, elecScale, srScale),
                    testTolerance_);
    }
    // Perform scaling tests on analytic routines
    void testAnalyticScalings(const Atom &i, const Atom &j, double r, double refChargeProduct)
    {
        // No scaling
        EXPECT_NEAR(potentialMap_.analyticEnergy(i, j, r), referenceEnergy(r, refChargeProduct), testTolerance_);
        EXPECT_NEAR(potentialMap_.analyticForce(i, j, r), referenceForce(r, refChargeProduct), testTolerance_);
        auto elecScale = 0.5, srScale = 0.5;

        // Equal scaling for short-range and electrostatics
        EXPECT_NEAR(potentialMap_.analyticEnergy(i, j, r, elecScale, srScale),
                    referenceEnergy(r, refChargeProduct, elecScale, srScale), testTolerance_);
        EXPECT_NEAR(potentialMap_.analyticForce(i, j, r, elecScale, srScale),
                    referenceForce(r, refChargeProduct, elecScale, srScale), testTolerance_);

        // Unlike scalings
        elecScale = 0.25;
        srScale = 0.75;
        EXPECT_NEAR(potentialMap_.analyticEnergy(i, j, r, elecScale, srScale),
                    referenceEnergy(r, refChargeProduct, elecScale, srScale), testTolerance_);
        EXPECT_NEAR(potentialMap_.analyticForce(i, j, r, elecScale, srScale),
                    referenceForce(r, refChargeProduct, elecScale, srScale), testTolerance_);
    }

    protected:
    // Lennard-Jones parameters for test
    static constexpr auto ljParameters = "epsilon=1.0 sigma=3.0";
    // Double value test tolerance
    static constexpr auto testTolerance_ = 1.0e-8;

    AtomType *atC1_{nullptr}, *atC2_{nullptr};
    DoubleKeyedMap<PairPotential> pairPotentials_;
    PotentialMap potentialMap_;
    Species species_;
    SpeciesTorsion torsion_;
    LocalMolecule molecule_;

    // Test potential function and wrapper, equivalent to the one defined in the potential map
    InteractionPotential<Functions1D> interactionPotential_{Functions1D::Form::LennardJones126, ljParameters};
    Function1DWrapper potentialWrapper_;
};

TEST_F(PairPotentialsScaleFactorsTest, SpeciesEnergyWithAtomTypeCharges)
{
    setUpPotentials(true);

    auto &i = species_.atom(0);
    auto &j = species_.atom(3);

    testScalings(&i, &j, (j.r() - i.r()).magnitude(), atC1_->charge() * atC1_->charge());
}

TEST_F(PairPotentialsScaleFactorsTest, SpeciesEnergyWithSpeciesCharges)
{
    setUpPotentials(false);

    auto &i = species_.atom(0);
    auto &j = species_.atom(3);

    testScalings(&i, &j, (j.r() - i.r()).magnitude(), i.charge() * j.charge());
}

TEST_F(PairPotentialsScaleFactorsTest, MoleculeEnergyWithAtomTypeCharges)
{
    setUpPotentials(true);

    auto &i = molecule_.localAtoms()[0];
    auto &j = molecule_.localAtoms()[3];

    testScalings(i, j, (j.r() - i.r()).magnitude(), atC1_->charge() * atC1_->charge());
    testAnalyticScalings(i, j, (j.r() - i.r()).magnitude(), atC1_->charge() * atC1_->charge());
}

TEST_F(PairPotentialsScaleFactorsTest, MoleculeEnergyWithSpeciesCharges)
{
    setUpPotentials(false);

    auto &i = molecule_.localAtoms()[0];
    auto &j = molecule_.localAtoms()[3];

    testScalings(i, j, (j.r() - i.r()).magnitude(), species_.atom(0).charge() * species_.atom(3).charge());
    testAnalyticScalings(i, j, (j.r() - i.r()).magnitude(), species_.atom(0).charge() * species_.atom(3).charge());
}

} // namespace UnitTest
