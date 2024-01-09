// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/rotateFragment.h"
#include "classes/configuration.h"
#include "keywords/enumOptions.h"
#include "keywords/nodeValue.h"
#include "main/dissolve.h"
#include "procedure/nodes/add.h"
#include "procedure/nodes/box.h"
#include "procedure/nodes/select.h"
#include "procedure/procedure.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <string>

namespace UnitTest
{

TEST(RotateFragmentProcedureNodeTest, Benzene)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    // Set up species
    auto *benzene = coreData.copySpecies(&benzeneSpecies());

    // Set up site
    auto site = SpeciesSite(benzene, "COG", SpeciesSite::SiteType::Fragment);
    site.setFragmentDefinitionString(
        "?C,#origin, ring(C(#origin, -H),C(#origin,#y, -H),C(#origin,#y, -H),C(#origin,#x, -H),C(#origin, -H),C(#origin, -H))");
    std::vector<const SpeciesSite *> sites = {&site};

    // Setup configuration
    auto *cfg = coreData.addConfiguration();
    auto &procedure = cfg->generator();
    auto boxLength = 20.0;
    const Vec3<double> cellCentre(boxLength / 2, boxLength / 2, boxLength / 2);
    auto box = procedure.createRootNode<BoxProcedureNode>("Box", Vec3<NodeValue>(boxLength, boxLength, boxLength),
                                                          Vec3<NodeValue>(90, 90, 90));

    // Add a single Benzene molecule at the centre of the cell - this way, rotations will not break it across box boundaries
    auto add = procedure.createRootNode<AddProcedureNode>("Benzene", benzene, 1);
    add->keywords().setEnumeration("Positioning", AddProcedureNode::PositioningType::Central);
    add->keywords().setEnumeration("BoxAction", AddProcedureNode::BoxActionStyle::None);
    add->keywords().set("Rotate", false);

    // Set up the prior configuration
    cfg->generate({ProcessPool(), dissolve});

    // Grab the first (and only) molecule
    auto mol = cfg->molecule(0);

    // Atom coordinates prior and posterior to rotations, with cell centre removed so as to give "local" coordinates
    std::vector<Vec3<double>> coordinatesBefore(mol->nAtoms()), coordinatesAfter(mol->nAtoms());
    std::transform(mol->atoms().begin(), mol->atoms().end(), coordinatesBefore.begin(),
                   [&cellCentre](const auto &at) { return at->r() - cellCentre; });

    // Select the site
    auto select = procedure.createRootNode<SelectProcedureNode>("BenzeneSite", sites);
    // Rotate Benzene around the Z-Axis of the site (which is defined at the COG, which is at the origin)
    auto &forEachB = select->branch()->get();
    auto rotate = forEachB.create<RotateFragmentProcedureNode>("RotateBenzene", select);
    rotate->keywords().setEnumeration("Axis", OrientedSite::SiteAxis::ZAxis);

    for (auto x = 90.0; x <= 360.0; x += 90.0)
    {
        // Rotate by 'x' degrees
        rotate->keywords().set("Rotation", NodeValueProxy(x));

        // Re-generate the configuration
        cfg->generate({ProcessPool(), dissolve});

        // Grab the first (and only) molecule
        mol = cfg->molecule(0);

        // Posterior atom coordinates
        std::transform(mol->atoms().begin(), mol->atoms().end(), coordinatesAfter.begin(),
                       [&cellCentre](const auto &at) { return at->r() - cellCentre; });
        for (auto &&[r1, r2] : zip(coordinatesBefore, coordinatesAfter))
        {
            // Check that
            // x' = -y
            // y' = x
            // z' = z = 0
            ASSERT_NEAR(r2.x, r1.y, 1e-5);
            ASSERT_NEAR(r2.y, -r1.x, 1e-5);
            ASSERT_EQ(r2.z, r1.z);
            ASSERT_EQ(r2.z, 0.0);
        }
        coordinatesBefore = coordinatesAfter;
    }
}
} // namespace UnitTest
