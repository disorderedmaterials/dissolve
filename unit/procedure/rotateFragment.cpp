// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/rotateFragment.h"
#include "classes/configuration.h"
#include "keywords/enumOptions.h"
#include "keywords/nodeValue.h"
#include "main/dissolve.h"
#include "procedure/nodes/add.h"
#include "procedure/nodes/select.h"
#include "procedure/procedure.h"
#include "unit/data/species.h"
#include <gtest/gtest.h>
#include <string>

namespace UnitTest
{

TEST(RotateTest, Benzene)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    // Set up species
    auto *benzene = dissolve.copySpecies(&benzeneSpecies());

    // Set up site
    auto site = SpeciesSite(benzene, SpeciesSite::SiteType::Fragment);
    site.setFragmentDefinitionString("?C, #origin, ring(C(-H),C(-H),C(-H),C(-H),C(-H),C(-H))");
    std::vector<const SpeciesSite *> sites = {&site};

    // Setup configuration
    auto *cfg = dissolve.addConfiguration();
    auto procedure = cfg->generator();
    cfg->createBoxAndCells({20, 20, 20}, {90, 90, 90}, false, dissolve.pairPotentialRange());
    cfg->cells().generate(cfg->box(), 7.0, dissolve.pairPotentialRange());

    // Add a single Benzene molecule
    auto add = procedure.createRootNode<AddProcedureNode>("Benzene", benzene, 1);
    add->keywords().setEnumeration("Positioning", AddProcedureNode::PositioningType::Central);

    // Select the site and set up a RotateFragment node
    auto select = procedure.createRootNode<SelectProcedureNode>("BenzeneSite", sites);
    auto rotate = procedure.createRootNode<RotateFragmentProcedureNode>("RotateBenzene", select);

    // Rotate four times around the x-axis of the site
    for (auto x = 90.0; x <= 360.0; x += 90.0)
    {
        auto molecules_before = cfg->molecules();

        rotate->keywords().set("Rotation", NodeValue{x});
        cfg->generate(ProcedureContext(ProcessPool(), dissolve));

        for (const auto &[mol1, mol2] : zip(molecules_before, cfg->molecules()))
        {
            for (const auto &[atom1, atom2] : zip(mol1->atoms(), mol2->atoms()))
            {
                // Check that
                // x' = x
                // y' = z
                // z' = -y
                ASSERT_EQ(atom2->r().x, atom1->r().x);
                ASSERT_EQ(atom2->r().y, atom2->r().z);
                ASSERT_EQ(atom1->r().z, -atom2->r().y);
            }
        }
    }
}
} // namespace UnitTest
