// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/coordinates.h"
#include "keywords/fileAndFormat.h"
#include "procedure/nodes/node.h"

CoordinatesProcedureNode::CoordinatesProcedureNode() : ProcedureNode(ProcedureNode::NodeType::Coordinates, {ProcedureNode::GenerationContext})
{
    keywords_.setOrganisation("Options", "Source Data");
    keywords_.add<FileAndFormatKeyword>("File", "File / format for coordinate sets to read in", fileSource_, "EndFile"); 

}
