// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/inputCoordinates.h"
#include "keywords/fileAndFormat.h"
#include "classes/configuration.h"

InputCoordinatesProcedureNode::InputCoordinatesProcedureNode() : ProcedureNode(ProcedureNode::NodeType::InputCoordinates, {ProcedureNode::GenerationContext})
{
    keywords_.setOrganisation("Options", "Source Data");
    keywords_.add<FileAndFormatKeyword>("File", "File / format for coordinate sets to read in", fileSource_, "EndFile");
}

bool InputCoordinatesProcedureNode::prepare(const ProcedureContext &procedureContext)
{
    if (!fileSource_.hasFilename())
        return Messenger::error("A suitable coordinate file and format must be supplied.\n");
    return true;
}

bool InputCoordinatesProcedureNode::execute(const ProcedureContext &procedureContext)
{
    auto *cfg = procedureContext.configuration(); 
    cfg->inputCoordinates().setFilename(fileSource_.filename());
    cfg->inputCoordinates().setFormatByIndex(fileSource_.formatIndex());
    return true;
}
