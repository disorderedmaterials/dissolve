// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/customregion.h"
#include "classes/configuration.h"
#include "expression/variable.h"
#include "keywords/double.h"
#include "keywords/nodevalue.h"

CustomRegionProcedureNode::CustomRegionProcedureNode() : RegionProcedureNodeBase(ProcedureNode::NodeType::CustomRegion)
{
    keywords_.setOrganisation("Options", "Definition");
    keywords_.add<NodeValueKeyword>("Expression", "Expression describing region", expression_, this);
    keywords_.add<DoubleKeyword>("Minimum", "Minimum value for descriptive function defining region", minimumValue_);
    keywords_.add<DoubleKeyword>("Maximum", "Maximum value for descriptive function defining region", maximumValue_);

    x_ = expression_.addLocalVariable("x");
    y_ = expression_.addLocalVariable("y");
    z_ = expression_.addLocalVariable("z");
    xFrac_ = expression_.addLocalVariable("xFrac");
    yFrac_ = expression_.addLocalVariable("yFrac");
    zFrac_ = expression_.addLocalVariable("zFrac");
}

/*
 * Region Data
 */

// Return whether voxel centred at supplied real coordinates is valid
bool CustomRegionProcedureNode::isVoxelValid(const Configuration *cfg, const Vec3<double> &r) const
{
    // Poke values into our variables
    x_->setValue(r.x);
    y_->setValue(r.y);
    z_->setValue(r.z);
    auto rFrac = cfg->box()->getFractional(r);
    xFrac_->setValue(rFrac.x);
    yFrac_->setValue(rFrac.y);
    zFrac_->setValue(rFrac.z);

    // Assess expression
    auto x = expression_.asDouble();

    if (x < minimumValue_ || x > maximumValue_)
        return false;

    return true;
}