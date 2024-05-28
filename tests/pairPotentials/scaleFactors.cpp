// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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
        atC1_ = coreData_.addAtomType(Elements::C);
        atC1_->setName("C1");
        atC1_->setCharge(-0.1);
        atC1_->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::LennardJones, ljParameters);
        atC2_ = coreData_.addAtomType(Elements::C);
        atC2_->setCharge(0.1);
        atC2_->setName("C2");
        atC2_->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::LennardJones, ljParameters);

        // Create Species
        fourSixthsBenzene_.addAtom(Elements::C, {-1.390000, 0.000000, 0.000000}, -0.2, atC1_);
        fourSixthsBenzene_.addAtom(Elements::C, {-0.695000, 1.203775, 0.000000}, 0.2, atC2_);
        fourSixthsBenzene_.addAtom(Elements::C, {0.695000, 1.203775, 0.000000}, 0.2, atC2_);
        fourSixthsBenzene_.addAtom(Elements::C, {1.390000, 0.000000, 0.000000}, -0.2, atC1_);
        fourSixthsBenzene_.addMissingBonds();
        torsion_ = fourSixthsBenzene_.addTorsion(0, 1, 2, 3);
        fourSixthsBenzene_.setUpScaledInteractions();

        // Create a molecule based on the species
        molecule_.setSpecies(&fourSixthsBenzene_);
        for (auto &&[molAtom, spAtom] : zip(molecule_.localAtoms(), fourSixthsBenzene_.atoms()))
        {
            molAtom.setCoordinates(spAtom.r());
            molAtom.setMasterTypeIndex(spAtom.atomType()->index());
        }

        // Set up the function wrapper
        potentialWrapper_.setFormAndParameters(interactionPotential_.form(), interactionPotential_.parameters());
    }
    // Set up potentials
    void setUpPotentials(bool useAtomTypeCharges)
    {
        const auto ppRange = 15.0, ppDelta = 0.001;

        potentialMap_.clear();
        pairPotentials_.clear();
        PairPotential::setIncludeAtomTypeCharges(useAtomTypeCharges);
        PairPotential::setShortRangeTruncationScheme(PairPotential::NoShortRangeTruncation);
        PairPotential::setCoulombTruncationScheme(PairPotential::NoCoulombTruncation);

        // Set up pair potentials
        auto *pp11 = std::get<2>(pairPotentials_.emplace_back(atC1_, atC1_,
                                                              std::make_unique<PairPotential>(atC1_->name(), atC1_->name())))
                         .get();
        pp11->setInteractionPotential(interactionPotential_);
        pp11->tabulate(ppRange, ppDelta, atC1_->charge(), atC1_->charge());
        auto *pp12 = std::get<2>(pairPotentials_.emplace_back(atC1_, atC2_,
                                                              std::make_unique<PairPotential>(atC1_->name(), atC2_->name())))
                         .get();
        pp12->setInteractionPotential(interactionPotential_);
        pp12->tabulate(ppRange, ppDelta, atC1_->charge(), atC2_->charge());
        auto *pp22 = std::get<2>(pairPotentials_.emplace_back(atC2_, atC2_,
                                                              std::make_unique<PairPotential>(atC2_->name(), atC2_->name())))
                         .get();
        pp22->setInteractionPotential(interactionPotential_);
        pp22->tabulate(ppRange, ppDelta, atC2_->charge(), atC2_->charge());

        // Create PotentialMap
        potentialMap_.initialise(coreData_.atomTypes(), pairPotentials_, ppRange);
    }
    // Return reference energy at distance r given specified charge product and scalings
    double referenceEnergy(double r, double chargeProduct, double eScale = 1.0, double vScale = 1.0)
    {
        return vScale * potentialWrapper_.y(r) + eScale * COULCONVERT * chargeProduct / r;
    }
    // Return reference force at distance r given specified charge product and scalings
    double referenceForce(double r, double chargeProduct, double eScale = 1.0, double vScale = 1.0)
    {
        return -vScale * potentialWrapper_.dYdX(r) + eScale * COULCONVERT * chargeProduct / (r * r);
    }
    // Perform trio of scaling tests
    template <class Particle> void testScalings(const Particle &i, const Particle &j, double r, double refChargeProduct)
    {
        // No scaling
        EXPECT_NEAR(potentialMap_.energy(i, j, r), referenceEnergy(r, refChargeProduct), testTolerance_);
        EXPECT_NEAR(potentialMap_.force(i, j, r), referenceForce(r, refChargeProduct), testTolerance_);

        auto eScale = 0.5, vScale = 0.5;

        // Equal scaling for short-range and electrostatics
        EXPECT_NEAR(potentialMap_.energy(i, j, r, eScale, vScale), referenceEnergy(r, refChargeProduct, eScale, vScale),
                    testTolerance_);
        EXPECT_NEAR(potentialMap_.force(i, j, r, eScale, vScale), referenceForce(r, refChargeProduct, eScale, vScale),
                    testTolerance_);

        // Unlike scalings
        eScale = 0.25;
        vScale = 0.75;
        EXPECT_NEAR(potentialMap_.energy(i, j, r, eScale, vScale), referenceEnergy(r, refChargeProduct, eScale, vScale),
                    testTolerance_);
        EXPECT_NEAR(potentialMap_.force(i, j, r, eScale, vScale), referenceForce(r, refChargeProduct, eScale, vScale),
                    testTolerance_);
    }

    protected:
    // Lennard-Jones parameters for test
    static constexpr auto ljParameters = "epsilon=1.0 sigma=3.0";
    // Double value test tolerance
    static constexpr auto testTolerance_ = 1.0e-8;

    CoreData coreData_;
    std::shared_ptr<AtomType> atC1_, atC2_;
    std::vector<PairPotential::Definition> pairPotentials_;
    PotentialMap potentialMap_;
    Species fourSixthsBenzene_;
    SpeciesTorsion torsion_;
    LocalMolecule molecule_;

    // Test potential function and wrapper, equivalent to the one defined in the potential map
    InteractionPotential<Functions1D> interactionPotential_{Functions1D::Form::LennardJones126, ljParameters};
    Function1DWrapper potentialWrapper_;
};

TEST_F(PairPotentialsScaleFactorsTest, SpeciesEnergyWithAtomTypeCharges)
{
    setUpPotentials(true);

    auto &i = fourSixthsBenzene_.atom(0);
    auto &j = fourSixthsBenzene_.atom(3);

    testScalings(&i, &j, (j.r() - i.r()).magnitude(), atC1_->charge() * atC1_->charge());
}

TEST_F(PairPotentialsScaleFactorsTest, SpeciesEnergyWithSpeciesCharges)
{
    setUpPotentials(false);

    auto &i = fourSixthsBenzene_.atom(0);
    auto &j = fourSixthsBenzene_.atom(3);

    testScalings(&i, &j, (j.r() - i.r()).magnitude(), i.charge() * j.charge());
}

TEST_F(PairPotentialsScaleFactorsTest, MoleculeEnergyWithAtomTypeCharges)
{
    setUpPotentials(true);

    auto &i = molecule_.localAtoms()[0];
    auto &j = molecule_.localAtoms()[3];

    testScalings(i, j, (j.r() - i.r()).magnitude(), atC1_->charge() * atC1_->charge());
}

TEST_F(PairPotentialsScaleFactorsTest, MoleculeEnergyWithSpeciesCharges)
{
    setUpPotentials(false);

    auto &i = molecule_.localAtoms()[0];
    auto &j = molecule_.localAtoms()[3];

    testScalings(i, j, (j.r() - i.r()).magnitude(), fourSixthsBenzene_.atom(0).charge() * fourSixthsBenzene_.atom(3).charge());
}

} // namespace UnitTest
