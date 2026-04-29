// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/cif/importCIFStructure.h"
#include <sstream>
#include <string>
#include <vector>

ImportCIFStructureNode::ImportCIFStructureNode(Graph *parentGraph) : Node(parentGraph)
{
    // Outputs
    addPointerOutput<CIFContext>("CIFContext", "CIF handling context derived from parsing of CIF file", context_)
        ->setFlags({ParameterBase::Required});
    addOutput<Structure *>("Structure", "Structure containing atoms and connectivity", structure_);

    // Option
    addOption<std::string>("FilePath", "File path", filePath_);
    addOption<SpaceGroups::SpaceGroupId>("SpaceGroupID", "Set space group from index", spaceGroup_);
}

std::string_view ImportCIFStructureNode::type() const { return "ImportCIFStructure"; }

std::string_view ImportCIFStructureNode::summary() const
{
    return "Load and parse a Crystallographic Information File (CIF) to a structure";
}

// Run main processing
NodeConstants::ProcessResult ImportCIFStructureNode::process()
{
    // Read contents of CIF file
    if (context_.read(filePath_))
    {
        if (spaceGroup_ != SpaceGroups::NoSpaceGroup)
            context_.setSpaceGroup(spaceGroup_);

        structure_ = context_.structure();

        return NodeConstants::ProcessResult::Success;
    }

    error("Failed to read contents of CIF file");

    return NodeConstants::ProcessResult::Failed;
}
