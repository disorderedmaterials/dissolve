// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/rotateFragment.h"
#include "classes/atom.h"
#include "classes/configuration.h"
#include "classes/molecule.h"
#include "classes/site.h"
#include "classes/speciesSite.h"
#include "keywords/enumOptions.h"
#include "keywords/node.h"
#include "keywords/nodeValue.h"
#include "keywords/nodeValueEnumOptions.h"
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
    assert(site_->currentSite());

    auto &site = site_->currentSite()->get();
    auto parent = site.parent();
    auto molecule = site.molecule();
    auto box = procedureContext.configuration()->box();

    if (!site.uniqueSiteIndex().has_value())
    {
        Messenger::warn("Parent index not set for for site generated from FragmentSite '{}', so cannot rotate.",
                        parent->name());
        return false;
    }

    auto parentIndex = site.uniqueSiteIndex().value();

    if (!site.hasAxes())
    {
        Messenger::warn("FragmentSite '{}' has no axes to rotate about.", parent->name());
        return false;
    }

    Matrix3 rotationMatrix;
    switch (axis_)
    {
        case (OrientedSite::SiteAxis::XAxis):
            rotationMatrix.createRotationAxis(site.axes().columnAsVec3(0), rotation_.asDouble(), false);
            break;
        case (OrientedSite::SiteAxis::YAxis):
            rotationMatrix.createRotationAxis(site.axes().columnAsVec3(1), rotation_.asDouble(), false);
            break;
        case (OrientedSite::SiteAxis::ZAxis):
            rotationMatrix.createRotationAxis(site.axes().columnAsVec3(2), rotation_.asDouble(), false);
            break;
    }

    for (auto index : parent->sitesAllAtomsIndices().at(parentIndex))
    {
        auto atom = molecule->atom(index);
        atom->set(rotationMatrix.transform(box->minimumVector(site.origin(), atom->r())) + site.origin());
    }

    return true;
}
