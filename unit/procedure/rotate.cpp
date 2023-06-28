// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/rotate.h"
#include "classes/configuration.h"
#include "keywords/enumoptions.h"
#include "keywords/nodevalue.h"
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

    auto *cfg = dissolve.addConfiguration();
    auto procedure = cfg->generator();
    cfg->createBoxAndCells({20, 20, 20}, {90, 90, 90}, false, dissolve.pairPotentialRange());
    cfg->cells().generate(cfg->box(), 7.0, dissolve.pairPotentialRange());

    auto add = procedure.createRootNode<AddProcedureNode>("Benzene", benzene, 1);
    add->keywords().setEnumeration("Positioning", AddProcedureNode::PositioningType::Central);

    auto select = procedure.createRootNode<SelectProcedureNode>("BenzeneSite", sites);
    auto rotate = procedure.createRootNode<RotateProcedureNode>("RotateBenzene", select);

    for (auto x = 0.0; x <= 360.0; ++x)
    {
        rotate->keywords().set("Rotation", NodeValue{x});
        cfg->generate(ProcedureContext(ProcessPool(), dissolve.potentialMap()));
        for (auto &mol : cfg->molecules())
            for (auto atom : mol->atoms())
                ASSERT_FLOAT_EQ(atom->r().z, 0.0);
    }

    for (auto y = 0.0; y <= 360.0; ++y)
    {
        rotate->keywords().set("Rotation", NodeValue{y});
        cfg->generate(ProcedureContext(ProcessPool(), dissolve.potentialMap()));
        for (auto &mol : cfg->molecules())
            for (auto atom : mol->atoms())
                ASSERT_FLOAT_EQ(atom->r().z, 0.0);
    }
}
} // namespace UnitTest
