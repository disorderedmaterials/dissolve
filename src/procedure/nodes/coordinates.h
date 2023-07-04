// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once


#include "io/import/coordinates.h"
#include "procedure/nodes/node.h"
class CoordinatesProcedureNode : public ProcedureNode
{
    public:
    CoordinatesProcedureNode();
    ~CoordinatesProcedureNode() override = default;

    private:
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
