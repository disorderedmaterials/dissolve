// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/rotateFragment.h"
#include "classes/configuration.h"
#include "keywords/enumOptions.h"
#include "keywords/nodeValue.h"
#include "main/dissolve.h"
#include "procedure/nodes/add.h"
#include "procedure/nodes/box.h"
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
    auto *benzene = coreData.copySpecies(&benzeneSpecies());

    // Set up site
    auto site = SpeciesSite(benzene, "COG", SpeciesSite::SiteType::Fragment);
    site.setFragmentDefinitionString(
        "?C,#origin, ring(C(#origin, -H),C(#origin, -H),C(#origin, -H),C(#origin,#x, -H),C(#origin,#y, -H),C(#origin,#y, -H))");
    std::vector<const SpeciesSite *> sites = {&site};

    // Setup configuration
    auto *cfg = coreData.addConfiguration();
    auto &procedure = cfg->generator();
    auto box = procedure.createRootNode<BoxProcedureNode>("Box", Vec3<NodeValue>(20, 20, 20), Vec3<NodeValue>(90, 90, 90));

    // Add a single Benzene molecule
    auto add = procedure.createRootNode<AddProcedureNode>("Benzene", benzene, 1);
    add->keywords().setEnumeration("Positioning", AddProcedureNode::PositioningType::Central);
    add->keywords().setEnumeration("BoxAction", AddProcedureNode::BoxActionStyle::None);
    add->keywords().setEnumeration("Rotate", false);

    // Select the site and set up a RotateFragment node
    auto select = procedure.createRootNode<SelectProcedureNode>("BenzeneSite", sites);
    auto rotate = procedure.createRootNode<RotateFragmentProcedureNode>("RotateBenzene", select);

    // Set up the configuration before the main loop starts
    cfg->generate(ProcedureContext(ProcessPool(), dissolve.potentialMap()));

    // Rotate four times around the x-axis of the site
    for (auto x = 90.0; x <= 360.0; x += 90.0)
    {
        auto molecules_before = cfg->molecules();

        rotate->keywords().set("Rotation", NodeValue{x});
        cfg->generate(ProcedureContext(ProcessPool(), dissolve.potentialMap()));

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
