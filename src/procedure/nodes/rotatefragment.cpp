// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/rotatefragment.h"
#include "classes/atom.h"
#include "classes/molecule.h"
#include "classes/site.h"
#include "classes/speciessite.h"
#include "keywords/enumoptions.h"
#include "keywords/node.h"
#include "keywords/nodevalue.h"
#include "keywords/nodevalueenumoptions.h"
#include "keywords/vec3nodevalue.h"
#include "math/matrix3.h"
#include "procedure/nodes/select.h"

RotateFragmentProcedureNode::RotateFragmentProcedureNode(std::shared_ptr<SelectProcedureNode> site)
    : ProcedureNode(ProcedureNode::NodeType::RotateFragment,
                    {ProcedureNode::GenerationContext, ProcedureNode::AnalysisContext}),
      site_(site)
{
    keywords_.setOrganisation("Options", "Site");
    keywords_.add<NodeKeyword<SelectProcedureNode>>("Site", "Site to be rotated", site_, this, ProcedureNode::NodeType::Select,
                                                    true);
    keywords_.add<NodeValueKeyword>("Rotation", "Rotation to perform", rotation_, this);
    keywords_.add<EnumOptionsKeyword<OrientedSite::SiteAxis>>("Axis", "Axis for rotation", axis_, OrientedSite::siteAxis());
}

/*
 * Identity
 */

// Return whether a name for the node must be provided
bool RotateFragmentProcedureNode::mustBeNamed() const { return false; }

/*
 * Execute
 */

bool RotateFragmentProcedureNode::execute(const ProcedureContext &procedureContext)
{
    auto site = site_->currentSite();
    auto parent = site->parent();
    auto molecule = site->molecule();

    if (!site->uniqueSiteIndex().has_value())
    {
        Messenger::warn("Parent index not set for for site generated from FragmentSite '{}', so cannot rotate.",
                        parent->name());
        return false;
    }

    auto parentIndex = site->uniqueSiteIndex().value();

    if (!site->hasAxes())
    {
        Messenger::warn("FragmentSite '{}' has no axes to rotate about.", parent->name());
        return false;
    }

    Matrix3 rotationMatrix;
    switch (axis_)
    {
        case (OrientedSite::SiteAxis::XAxis):
            rotationMatrix.createRotationX(rotation_);
            break;
        case (OrientedSite::SiteAxis::YAxis):
            rotationMatrix.createRotationY(rotation_);
            break;
        case (OrientedSite::SiteAxis::ZAxis):
            rotationMatrix.createRotationZ(rotation_);
            break;
    }
    rotationMatrix *= site->axes();

    for (auto index : parent->sitesAllAtomsIndices().at(parentIndex))
    {
        auto atom = molecule->atom(index);
        atom->set(rotationMatrix.transform(atom->r()));
    }

    return true;
}
