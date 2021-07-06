// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/pickcylinder.h"
#include "base/lineparser.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "keywords/types.h"

PickCylinderProcedureNode::PickCylinderProcedureNode(Vec3<double> originFrac, Vec3<double> direction, double radius)
    : PickProcedureNodeBase(ProcedureNode::NodeType::PickCylinder)
{
    keywords_.add("Control", new Vec3DoubleKeyword({0.0, 0.0, 0.0}, Vec3Labels::XYZLabels), "OriginFrac",
                  "Origin of vector in fractional coordinates");
    keywords_.add("Control", new DoubleKeyword(5.0, 1.0e-3), "Radius", "Cylinder radius");
    keywords_.add("Control", new Vec3DoubleKeyword({0.0, 0.0, 1.0}, Vec3Labels::XYZLabels), "Vector", "Cylinder vector");
}

/*
 * Execute
 */

// Execute node, targetting the supplied Configuration
bool PickCylinderProcedureNode::execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                        GenericList &targetList)
{
    // Create our molecules vector
    pickedMolecules_.clear();

    // Get definitions
    auto l0 = cfg->box()->axes() * keywords_.asVec3Double("OriginFrac");
    auto l1 = l0 + keywords_.asVec3Double("Vector");
    auto denom = (l1 - l0).magnitude();
    auto r = keywords_.asDouble("Radius");
    const auto *box = cfg->box();

    // Loop over all molecules in supplied Configuration
    for (const auto &mol : moleculePool(cfg))
    {
        // Calculate distance of molecule center-of-geometry from line
        auto p0 = box->minimumImage(mol->centreOfGeometry(cfg->box()), l0);
        auto num = ((p0 - l0) * (p0 - l1)).magnitude();
        auto d = num / denom;

        // Check distance vs cylinder radius
        if (d <= r)
            pickedMolecules_.push_back(mol);
    }

    Messenger::print("[Pick] Total molecules selected = {}.\n", pickedMolecules_.size());

    return true;
}
