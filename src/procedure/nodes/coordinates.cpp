// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/coordinates.h"
#include "keywords/fileAndFormat.h"
#include "classes/configuration.h"

CoordinatesProcedureNode::CoordinatesProcedureNode() : ProcedureNode(ProcedureNode::NodeType::Coordinates, {ProcedureNode::GenerationContext})
{
    keywords_.setOrganisation("Options", "Source Data");
    keywords_.add<FileAndFormatKeyword>("File", "File / format for coordinate sets to read in", fileSource_, "EndFile");
}

bool CoordinatesProcedureNode::execute(const ProcedureContext &procedureContext)
{
    if (!fileSource_.fileExists() || !fileSource_.hasFilename())
    {
        Messenger::error("Couldn't import coordinates from '{}', file does not exist!", fileSource_.filename());
        return false;
    }
    auto *cfg = procedureContext.configuration(); 
    cfg->inputCoordinates().setFilename(fileSource_.filename());
    cfg->inputCoordinates().setFormatByIndex(fileSource_.formatIndex());
    return true;
}
