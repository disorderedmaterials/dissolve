// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/configuration.h"
#include "procedure/procedure.h"
#include "procedure/nodes/add.h"
#include "procedure/nodes/box.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <string>

namespace UnitTest
{

TEST(ArtificialAtomsTest, Basic)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    // Set up species
    auto *artAr = coreData.copySpecies(&tetrahedralArgonSpecies());

    // Create a configuration
    auto *cfg = coreData.addConfiguration();
    auto &procedure = cfg->generator();
    auto boxLength = 20.0;
    auto box = procedure.createRootNode<BoxProcedureNode>("Box", Vec3<NodeValue>(boxLength, boxLength, boxLength),
                                                          Vec3<NodeValue>(90, 90, 90));

    // Add in some artificial argon atoms
    const auto nMolecules = 100;
    procedure.createRootNode<AddProcedureNode>("ArtAr", artAr, nMolecules);

    // Set up the prior configuration
    cfg->generate({ProcessPool(), dissolve});

    // Basic checks
    EXPECT_EQ(cfg->nMolecules(), nMolecules);
    EXPECT_EQ(cfg->nAtoms(), nMolecules * artAr->nAtoms());
}
} // namespace UnitTest
