// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "io/import/forces.h"
#include "classes/configuration.h"
#include "main/dissolve.h"
#include "modules/forces/forces.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
void testForces(const std::vector<Vec3<double>> &dlpoly, const std::vector<Vec3<double>> &dslve, double tolerance)
{
    ASSERT_EQ(dlpoly.size(), dslve.size());
    for (auto n = 0; n < dlpoly.size(); ++n)
    {
        EXPECT_NEAR(dlpoly[n].x, dslve[n].x * 100.0, tolerance);
        EXPECT_NEAR(dlpoly[n].y, dslve[n].y * 100.0, tolerance);
        EXPECT_NEAR(dlpoly[n].z, dslve[n].z * 100.0, tolerance);
    }
}

TEST(ForcesModuleTest, Full)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    dissolve.clear();
    ASSERT_TRUE(dissolve.loadInput("full.txt"));
    ASSERT_TRUE(dissolve.regeneratePairPotentials());
    dissolve.prepare();
    auto &cfg = coreData.configurations().front();
    ASSERT_TRUE(cfg != nullptr);

    // Load in full reference forces
    ForceImportFileFormat importer("../_data/dlpoly/hexane200/REVCON", ForceImportFileFormat::ForceImportFormat::DLPOLY);
    std::vector<Vec3<double>> fReference(cfg->nAtoms());
    ASSERT_TRUE(importer.importData(fReference));

    // Calculate full forces
    std::vector<Vec3<double>> fCalculated(cfg->nAtoms());
    ForcesModule::totalForces(dissolve.worldPool(), cfg.get(), dissolve.potentialMap(),
                              ForcesModule::ForceCalculationType::Full, fCalculated, fCalculated);
    testForces(fReference, fCalculated, 0.4);
}

TEST(ForcesModuleTest, Unbound)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    dissolve.clear();
    ASSERT_TRUE(dissolve.loadInput("full.txt"));
    ASSERT_TRUE(dissolve.regeneratePairPotentials());
    dissolve.prepare();
    auto &cfg = coreData.configurations().front();
    ASSERT_TRUE(cfg != nullptr);

    // Load in unbound reference forces
    ForceImportFileFormat importer("../_data/dlpoly/hexane200_unbound/REVCON",
                                   ForceImportFileFormat::ForceImportFormat::DLPOLY);
    std::vector<Vec3<double>> fReference(cfg->nAtoms());
    ASSERT_TRUE(importer.importData(fReference));

    // Calculate pair potential forces
    std::vector<Vec3<double>> fCalculated(cfg->nAtoms());
    ForcesModule::totalForces(dissolve.worldPool(), cfg.get(), dissolve.potentialMap(),
                              ForcesModule::ForceCalculationType::PairPotentialOnly, fCalculated, fCalculated);
    testForces(fReference, fCalculated, 0.2);
}

TEST(ForcesModuleTest, Bound)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    dissolve.clear();
    ASSERT_TRUE(dissolve.loadInput("full.txt"));
    ASSERT_TRUE(dissolve.regeneratePairPotentials());
    dissolve.prepare();
    auto &cfg = coreData.configurations().front();
    ASSERT_TRUE(cfg != nullptr);

    // Load in bound reference forces
    ForceImportFileFormat importer("../_data/dlpoly/hexane200_bound/REVCON", ForceImportFileFormat::ForceImportFormat::DLPOLY);
    std::vector<Vec3<double>> fReference(cfg->nAtoms());
    ASSERT_TRUE(importer.importData(fReference));

    // Calculate bound forces
    std::vector<Vec3<double>> fCalculated(cfg->nAtoms());
    ForcesModule::totalForces(dissolve.worldPool(), cfg.get(), dissolve.potentialMap(),
                              ForcesModule::ForceCalculationType::IntraMolecularGeometry, fCalculated, fCalculated);
    testForces(fReference, fCalculated, 1.0e-4);
}

} // namespace UnitTest
