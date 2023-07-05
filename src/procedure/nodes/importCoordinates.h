// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "io/import/coordinates.h"
#include "procedure/nodes/node.h"

class ImportCoordinatesProcedureNode : public ProcedureNode
{
    public:
    ImportCoordinatesProcedureNode();
    ~ImportCoordinatesProcedureNode() override = default;

    private:
    CoordinateImportFileFormat fileSource_;

    /*
     * Identity
     */
    public:
    // Return whether a name for the node must be provided
    bool mustBeNamed() const override;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
