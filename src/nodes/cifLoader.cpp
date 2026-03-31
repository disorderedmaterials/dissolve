// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/cifLoader.h"
#include <sstream>
#include <string>
#include <vector>

CIFLoaderNode::CIFLoaderNode(Graph *parentGraph) : Node(parentGraph)
{
    // Outputs
    addPointerOutput<CIFContext>("CIFContext", "CIF handling context derived from parsing of CIF file", context_)
        ->setFlags({ParameterBase::Required});

    // Option
    addOption<std::string>("FilePath", "File path", filePath_);
    addOption<SpaceGroups::SpaceGroupId>("SpaceGroupID", "Set space group from index", spaceGroup_);
}

std::string_view CIFLoaderNode::type() const { return "CIFLoader"; }

std::string_view CIFLoaderNode::summary() const
{
    return "Load a CIF file and apply contained crystallographic data to a target configuration";
}

// Run main processing
NodeConstants::ProcessResult CIFLoaderNode::process()
{
    // Read contents of CIF file
    if (context_.read(filePath_))
    {
        if (spaceGroup_ != SpaceGroups::NoSpaceGroup)
            context_.setSpaceGroup(spaceGroup_);
        return NodeConstants::ProcessResult::Success;
    }

    error("Failed to read contents of CIF file");

    return NodeConstants::ProcessResult::Failed;
}
