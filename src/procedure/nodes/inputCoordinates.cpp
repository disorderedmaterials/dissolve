// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/inputCoordinates.h"
#include "classes/configuration.h"
#include "keywords/fileAndFormat.h"

InputCoordinatesProcedureNode::InputCoordinatesProcedureNode()
    : ProcedureNode(ProcedureNode::NodeType::InputCoordinates, {ProcedureNode::GenerationContext})
{
    keywords_.setOrganisation("Options", "Source Data");
    keywords_.add<FileAndFormatKeyword>("File", "File / format for initial coordinate to read in", fileSource_, "EndFile");
}

/*
 * Identity
 */

// Return whether a name for the node must be provided
bool InputCoordinatesProcedureNode::mustBeNamed() const { return false; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool InputCoordinatesProcedureNode::prepare(const ProcedureContext &procedureContext)
{
    if (!fileSource_.hasFilename())
        return Messenger::error("A suitable coordinate file and format must be supplied.\n");
    return true;
}

// Execute node
bool InputCoordinatesProcedureNode::execute(const ProcedureContext &procedureContext)
{
    auto *cfg = procedureContext.configuration();
    cfg->inputCoordinates().setFilename(fileSource_.filename());
    cfg->inputCoordinates().setFormatByIndex(fileSource_.formatIndex());
    return true;
}
