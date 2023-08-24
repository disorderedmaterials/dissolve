// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/calculateAxisAngle.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "expression/variable.h"
#include "keywords/bool.h"
#include "keywords/enumOptions.h"
#include "procedure/nodes/select.h"

CalculateAxisAngleProcedureNode::CalculateAxisAngleProcedureNode(std::shared_ptr<SelectProcedureNode> site0,
                                                                 OrientedSite::SiteAxis axis0,
                                                                 std::shared_ptr<SelectProcedureNode> site1,
                                                                 OrientedSite::SiteAxis axis1)
    : CalculateProcedureNodeBase(ProcedureNode::NodeType::CalculateAxisAngle, site0, site1), axes_{axis0, axis1}
{
    keywords_.setOrganisation("Options", "Sites");
    keywords_.add<NodeKeyword<SelectProcedureNode>>("I", "Site that contains the first set of axes", sites_[0], this,
                                                    ProcedureNode::NodeType::Select, true);
    keywords_.add<NodeKeyword<SelectProcedureNode>>("J", "Site that contains the second set of axes", sites_[1], this,
                                                    ProcedureNode::NodeType::Select, true);
    keywords_.add<EnumOptionsKeyword<OrientedSite::SiteAxis>>("AxisI", "Axis to use from site I", axes_[0],
                                                              OrientedSite::siteAxis());
    keywords_.add<EnumOptionsKeyword<OrientedSite::SiteAxis>>("AxisJ", "Axis to use from site J", axes_[1],
                                                              OrientedSite::siteAxis());

    keywords_.setOrganisation("Options", "Control");
    keywords_.add<BoolKeyword>(
        "Symmetric", "Whether to consider angles as symmetric about 90, mapping all angles to the range 0 - 90", symmetric_);

    // Create parameters
    angleParameter_ = parameters_.emplace_back(std::make_shared<ExpressionVariable>("theta"));
}

/*
 * Identity
 */

// Set node name
void CalculateAxisAngleProcedureNode::setName(std::string_view name)
{
    name_ = DissolveSys::niceName(name);

    // Update parameter names to match
    angleParameter_->setName(fmt::format("{}.theta", name_));
}

/*
 * Parameters
 */

// Return the named parameter (if it exists)
std::shared_ptr<ExpressionVariable>
CalculateAxisAngleProcedureNode::getParameter(std::string_view name, std::shared_ptr<ExpressionVariable> excludeParameter)
{
    for (auto var : parameters_)
        if ((var != excludeParameter) && (DissolveSys::sameString(var->name(), name)))
            return var;

    return nullptr;
}

// Return vector of all parameters for this node
OptionalReferenceWrapper<const std::vector<std::shared_ptr<ExpressionVariable>>>
CalculateAxisAngleProcedureNode::parameters() const
{
    return parameters_;
}

/*
 * Data
 */

// Return axis specified
OrientedSite::SiteAxis &CalculateAxisAngleProcedureNode::axis(int n)
{
    assert(n >= 0 && n < 2);
    return axes_[n];
}

/*
 * Observable Target
 */

// Return number of sites required to calculate observable
int CalculateAxisAngleProcedureNode::nSitesRequired() const { return 2; }

// Return dimensionality of calculated observable
int CalculateAxisAngleProcedureNode::dimensionality() const { return 1; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool CalculateAxisAngleProcedureNode::prepare(const ProcedureContext &procedureContext)
{
    // Call the base class function
    if (!CalculateProcedureNodeBase::prepare(procedureContext))
        return false;

    return true;
}

// Execute node
bool CalculateAxisAngleProcedureNode::execute(const ProcedureContext &procedureContext)
{
    assert(sites_[0] && sites_[0]->currentSite());
    assert(sites_[1] && sites_[1]->currentSite());

    value_.x = Box::angleInDegrees(sites_[0]->currentSite()->get().axes().columnAsVec3(axes_[0]),
                                   sites_[1]->currentSite()->get().axes().columnAsVec3(axes_[1]));

    if (symmetric_ && value_.x > 90.0)
        value_.x = 180.0 - value_.x;

    angleParameter_->setValue(value_.x);

    return true;
}
