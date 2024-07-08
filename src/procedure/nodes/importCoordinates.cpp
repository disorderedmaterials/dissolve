// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/importCoordinates.h"
#include "classes/configuration.h"
#include "keywords/fileAndFormat.h"

ImportCoordinatesProcedureNode::ImportCoordinatesProcedureNode() : ProcedureNode(NodeType::ImportCoordinates)
{
    keywords_.setOrganisation("Options", "Source Data");
    keywords_.add<FileAndFormatKeyword>("File", "File / format for coordinates to read in", fileSource_, "EndFile");
}

/*
 * Identity
 */

// Return whether a name for the node must be provided
bool ImportCoordinatesProcedureNode::mustBeNamed() const { return false; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool ImportCoordinatesProcedureNode::prepare(const ProcedureContext &procedureContext)
{
    if (!fileSource_.hasFilename())
        return Messenger::error("A suitable coordinate file and format must be supplied.\n");
    if (!fileSource_.fileExists())
        return Messenger::error("Specified coordinate file '{}' doesn't exist.\n", fileSource_.filename());
    return true;
}

// Execute node
bool ImportCoordinatesProcedureNode::execute(const ProcedureContext &procedureContext)
{
    return fileSource_.importData(procedureContext.configuration(), &procedureContext.processPool());
}
