// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/select.h"
#include "classes/configuration.h"
#include "generator/generator.h"
#include "main/dissolve.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <string>

namespace UnitTest
{
class SelectGeneratorNodeTest : public ::testing::Test
{
    public:
    SelectGeneratorNodeTest() : dissolve_(coreData_)
    {
        // Set up species
        alphaSpecies_ = coreData_.copySpecies(&diatomicSpecies());
        alphaSite_ = alphaSpecies_->addSite("NO");
        alphaSite_->setType(SpeciesSite::SiteType::Dynamic);
        alphaSite_->setDynamicElements({Elements::N, Elements::O});
        alphaSiteN_ = alphaSpecies_->addSite("N");
        alphaSiteN_->setType(SpeciesSite::SiteType::Dynamic);
        alphaSiteN_->setDynamicElements({Elements::N});
        alphaSiteO_ = alphaSpecies_->addSite("O");
        alphaSiteO_->setType(SpeciesSite::SiteType::Dynamic);
        alphaSiteO_->setDynamicElements({Elements::N});

        betaSpecies_ = coreData_.copySpecies(&diatomicSpecies());
        betaSite_ = betaSpecies_->addSite("N");
        betaSite_->setType(SpeciesSite::SiteType::Dynamic);
        betaSite_->setDynamicElements({Elements::N, Elements::O});

        argonSpecies_ = coreData_.copySpecies(&argonSpecies());
        argonSite_ = argonSpecies_->addSite("Ar");
        argonSite_->setType(SpeciesSite::SiteType::Static);
        argonSite_->setStaticOriginAtoms({&argonSpecies_->atom(0)});

        /*
         * Setup configuration
         * Our configuration will comprise a contiguous row of N2 molecules of two different "types" along X,
         * and a single argon at the centre of the two types
         *
         * For instance:
         *                   N   N   N   N   N   N   N   N
         *                   |   |   |   |Ar |   |   |   |
         *                   O   O   O   O   O   O   O   O
         *                  (a   a   a   a   b   b   b   b)
         */

        configuration_ = coreData_.addConfiguration();
        configuration_->createBoxAndCells({nType_ * 4.0, 20, 20}, {90, 90, 90}, false, 10.0);
        configuration_->updateCells(10.0);

        // Add our molecules
        for (auto pos = 0; pos < nType_ * 2; ++pos)
        {
            // Add the N2 molecule (alpha if less than nType, beta otherwise)
            auto n2 = configuration_->addMolecule(pos < nType_ ? alphaSpecies_ : betaSpecies_);
            n2->setCentreOfGeometry(configuration_->box(), {pos * 1.0, 0.0, 0.0});
        }
        auto argon = configuration_->addMolecule(argonSpecies_);
        argon->setCentreOfGeometry(configuration_->box(), {nType_ - 0.5, 0.0, 0.0});

        configuration_->updateObjectRelationships();
        configuration_->updateAtomLocations(true);
    };

    protected:
    CoreData coreData_;
    Dissolve dissolve_;
    Species *alphaSpecies_, *betaSpecies_, *argonSpecies_;
    SpeciesSite *alphaSite_, *alphaSiteN_, *alphaSiteO_, *betaSite_, *argonSite_;
    Configuration *configuration_;
    const int nType_{10};

    protected:
    void SetUp() override {}
};

TEST_F(SelectGeneratorNodeTest, Simple)
{
    Generator testGenerator;
    auto selectN =
        testGenerator.createRootNode<SelectGeneratorNode>("SelectN", std::vector<const SpeciesSite *>{alphaSite_, betaSite_});

    testGenerator.execute({dissolve_.worldPool(), configuration_});

    auto N = nType_ * 2;
    EXPECT_EQ(selectN->nSitesInStack(), N * 2);

    auto &sites = selectN->sites();

    // Check indices of selected sites
    for (auto n = 0; n < sites.size(); ++n)
    {
        // Site index, running from 1...N+M
        EXPECT_EQ(std::get<1>(sites[n]), n + 1);
        // Stack index, running from 1...N (twice)
        EXPECT_EQ(std::get<2>(sites[n]), (n % N) + 1);
    }
}

TEST_F(SelectGeneratorNodeTest, All)
{
    Generator testGenerator;
    auto selectAr = testGenerator.createRootNode<SelectGeneratorNode>("SelectAr", std::vector<const SpeciesSite *>{argonSite_});
    auto &forEachAr = selectAr->branch()->get();
    auto selectN = forEachAr.create<SelectGeneratorNode>("SelectN", std::vector<const SpeciesSite *>{alphaSite_, betaSite_});

    testGenerator.execute({dissolve_.worldPool(), configuration_});

    EXPECT_EQ(selectAr->nSitesInStack(), 1);
    EXPECT_EQ(selectN->nSitesInStack(), nType_ * 2 * 2);
}

TEST_F(SelectGeneratorNodeTest, Ranges)
{
    Generator testGenerator;
    auto selectAr = testGenerator.createRootNode<SelectGeneratorNode>("SelectAr", std::vector<const SpeciesSite *>{argonSite_});
    auto &forEachAr = selectAr->branch()->get();
    auto selectN = forEachAr.create<SelectGeneratorNode>("SelectN", std::vector<const SpeciesSite *>{alphaSite_, betaSite_});
    selectN->setDistanceReferenceSite(selectAr);

    for (auto rangeInt = 1; rangeInt <= nType_; ++rangeInt)
    {
        selectN->setInclusiveDistanceRange({0.0, rangeInt * 1.0});

        testGenerator.execute({dissolve_.worldPool(), configuration_});

        EXPECT_EQ(selectAr->nSitesInStack(), 1);
        EXPECT_EQ(selectN->nSitesInStack(), rangeInt * 2 * 2);

        configuration_->incrementContentsVersion();
    }
}

TEST_F(SelectGeneratorNodeTest, Indices)
{
    Generator testGenerator;
    auto selectAr = testGenerator.createRootNode<SelectGeneratorNode>("SelectAr", std::vector<const SpeciesSite *>{argonSite_});
    auto &forEachAr = selectAr->branch()->get();
    auto selectN = forEachAr.create<SelectGeneratorNode>("SelectN", std::vector<const SpeciesSite *>{alphaSite_, betaSite_});
    selectN->setDistanceReferenceSite(selectAr);

    auto N = nType_ * 2;

    for (auto rangeInt = 1; rangeInt <= nType_; ++rangeInt)
    {
        selectN->setInclusiveDistanceRange({0.0, rangeInt * 1.0});

        testGenerator.execute({dissolve_.worldPool(), configuration_});

        EXPECT_EQ(selectAr->nSitesInStack(), 1);
        EXPECT_EQ(selectN->nSitesInStack(), rangeInt * 2 * 2);

        auto &sites = selectN->sites();

        // Check global indices of selected sites
        auto vectorIndex = 0;
        for (auto siteIndex = nType_ * 2 - rangeInt * 2; siteIndex < nType_ * 2 + rangeInt * 2; ++siteIndex)
        {
            // Site index, running from 1...N+M
            EXPECT_EQ(std::get<1>(sites[vectorIndex]), siteIndex + 1);
            // Stack index, running from 1...N (twice)
            EXPECT_EQ(std::get<2>(sites[vectorIndex++]), (siteIndex % N) + 1);
        }

        configuration_->incrementContentsVersion();
    }
}

TEST_F(SelectGeneratorNodeTest, Exclusions1)
{
    Generator testGenerator;
    auto selectN1 =
        testGenerator.createRootNode<SelectGeneratorNode>("SelectN1", std::vector<const SpeciesSite *>{alphaSiteN_});
    auto &forEachN1 = selectN1->branch()->get();
    auto selectN2 = forEachN1.create<SelectGeneratorNode>("SelectN2", std::vector<const SpeciesSite *>{alphaSiteN_});

    // No exclusions
    testGenerator.execute({dissolve_.worldPool(), configuration_});
    EXPECT_EQ(selectN1->nSitesInStack(), nType_);
    EXPECT_EQ(selectN2->nSitesInStack(), nType_);
    EXPECT_DOUBLE_EQ(selectN1->nAvailableSitesAverage(), double(nType_));
    EXPECT_DOUBLE_EQ(selectN2->nAvailableSitesAverage(), double(nType_));
    configuration_->incrementContentsVersion();

    // Exclude same site
    selectN2->keywords().set("ExcludeSameSite", ConstNodeVector<SelectGeneratorNode>{selectN1});
    testGenerator.execute({dissolve_.worldPool(), configuration_});
    EXPECT_EQ(selectN1->nSitesInStack(), nType_);
    EXPECT_EQ(selectN2->nSitesInStack(), nType_ - 1);
    EXPECT_DOUBLE_EQ(selectN1->nAvailableSitesAverage(), double(nType_));
    EXPECT_DOUBLE_EQ(selectN2->nAvailableSitesAverage(), double(nType_ - 1));
    configuration_->incrementContentsVersion();

    // Exclude same molecule as well (no effect)
    selectN2->keywords().set("ExcludeSameMolecule", ConstNodeVector<SelectGeneratorNode>{selectN1});
    testGenerator.execute({dissolve_.worldPool(), configuration_});
    EXPECT_EQ(selectN1->nSitesInStack(), nType_);
    EXPECT_EQ(selectN2->nSitesInStack(), nType_ - 1);
    EXPECT_DOUBLE_EQ(selectN1->nAvailableSitesAverage(), double(nType_));
    EXPECT_DOUBLE_EQ(selectN2->nAvailableSitesAverage(), double(nType_ - 1));
}

TEST_F(SelectGeneratorNodeTest, Exclusions2)
{
    Generator testGenerator;
    auto selectN = testGenerator.createRootNode<SelectGeneratorNode>("SelectN", std::vector<const SpeciesSite *>{alphaSiteN_});
    auto &forEachN = selectN->branch()->get();
    auto selectO = forEachN.create<SelectGeneratorNode>("SelectO", std::vector<const SpeciesSite *>{alphaSiteO_});

    // No exclusions
    testGenerator.execute({dissolve_.worldPool(), configuration_});
    EXPECT_EQ(selectN->nSitesInStack(), nType_);
    EXPECT_EQ(selectO->nSitesInStack(), nType_);
    EXPECT_DOUBLE_EQ(selectN->nAvailableSitesAverage(), double(nType_));
    EXPECT_DOUBLE_EQ(selectO->nAvailableSitesAverage(), double(nType_));
    configuration_->incrementContentsVersion();

    // Exclude same molecule
    selectO->keywords().set("ExcludeSameMolecule", ConstNodeVector<SelectGeneratorNode>{selectN});
    testGenerator.execute({dissolve_.worldPool(), configuration_});
    EXPECT_EQ(selectN->nSitesInStack(), nType_);
    EXPECT_EQ(selectO->nSitesInStack(), nType_ - 1);
    EXPECT_DOUBLE_EQ(selectN->nAvailableSitesAverage(), double(nType_));
    EXPECT_DOUBLE_EQ(selectO->nAvailableSitesAverage(), double(nType_ - 1));
}

TEST_F(SelectGeneratorNodeTest, Inclusions)
{
    Generator testGenerator;
    auto selectN = testGenerator.createRootNode<SelectGeneratorNode>("SelectN", std::vector<const SpeciesSite *>{alphaSiteN_});
    auto &forEachN = selectN->branch()->get();
    auto selectO = forEachN.create<SelectGeneratorNode>("SelectO", std::vector<const SpeciesSite *>{alphaSiteO_});

    // Require same molecule as first site
    selectO->keywords().set("SameMoleculeAsSite", selectN);
    testGenerator.execute({dissolve_.worldPool(), configuration_});
    EXPECT_EQ(selectN->nSitesInStack(), nType_);
    EXPECT_EQ(selectO->nSitesInStack(), 1);
    EXPECT_DOUBLE_EQ(selectN->nAvailableSitesAverage(), double(nType_));
    EXPECT_DOUBLE_EQ(selectO->nAvailableSitesAverage(), 1.0);
}
} // namespace UnitTest
