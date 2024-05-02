// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/import/coordinates.h"
#include "procedure/nodes/node.h"

class ImportCoordinatesProcedureNode : public ProcedureNode
{
    public:
    ImportCoordinatesProcedureNode();
    ~ImportCoordinatesProcedureNode() override = default;

    /*
     * Identity
     */
    public:
    // Return whether a name for the node must be provided
    bool mustBeNamed() const override;

    /*
     * Node Data
     */
    private:
    // Import Coordinates source file and format
    CoordinateImportFileFormat fileSource_;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
