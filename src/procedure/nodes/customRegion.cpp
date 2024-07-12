// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/customRegion.h"
#include "classes/configuration.h"
#include "expression/variable.h"
#include "keywords/double.h"
#include "keywords/nodeValue.h"

/*
 * Custom Region Voxel Kernel
 */

CustomRegionVoxelKernel::CustomRegionVoxelKernel(std::string_view expressionString,
                                                 std::vector<std::shared_ptr<ExpressionVariable>> parameters,
                                                 double minimumValue, double maximumValue)
{
    // Create our local variables
    x_ = expression_.addLocalVariable("x");
    y_ = expression_.addLocalVariable("y");
    z_ = expression_.addLocalVariable("z");
    xFrac_ = expression_.addLocalVariable("xFrac");
    yFrac_ = expression_.addLocalVariable("yFrac");
    zFrac_ = expression_.addLocalVariable("zFrac");

    // Set the expression
    expression_.set(expressionString, std::move(parameters));

    // Set limits
    minimumValue_ = minimumValue;
    maximumValue_ = maximumValue;
}

// Return whether voxel centred at supplied real coordinates is valid
bool CustomRegionVoxelKernel::isVoxelValid(const Configuration *cfg, const Vec3<double> &r) const
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

    return (x >= minimumValue_ && x <= maximumValue_);
}

/*
 * Custom Region
 */

CustomRegionProcedureNode::CustomRegionProcedureNode() : RegionProcedureNodeBase(ProcedureNode::NodeType::CustomRegion)
{
    keywords_.setOrganisation("Options", "Definition");
    keywords_.add<NodeValueKeyword>("Expression", "Expression describing region", expression_, this);
    keywords_.add<DoubleKeyword>("Minimum", "Minimum value for descriptive function defining region", minimumValue_);
    keywords_.add<DoubleKeyword>("Maximum", "Maximum value for descriptive function defining region", maximumValue_);
}

std::shared_ptr<VoxelKernel> CustomRegionProcedureNode::createVoxelKernel()
{
    return std::make_shared<CustomRegionVoxelKernel>(expression_.asString(), getParametersInScope(), minimumValue_,
                                                     maximumValue_);
}
