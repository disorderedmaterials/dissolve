// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"
#include "procedure/nodes/pickbase.h"
#include "templates/array.h"
#include <memory>

// Forward Declarations
class Molecule;

// Pick Cylinder Node
class PickCylinderProcedureNode : public PickProcedureNodeBase
{
    public:
    explicit PickCylinderProcedureNode(Vec3<double> originFrac = {0, 0, 0}, Vec3<double> direction = {0, 0, 1},
                                       double radius = 5.0);
    ~PickCylinderProcedureNode() override = default;

    /*
     * Execute
     */
    public:
    // Execute node, targetting the supplied Configuration
    bool execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};
