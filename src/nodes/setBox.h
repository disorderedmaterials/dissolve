// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/structure.h"
#include "nodes/node.h"

class SetBoxNode : public Node
{
    public:
    SetBoxNode(Graph *parentGraph);
    ~SetBoxNode() override = default;

    /*
     * Definition
     */
    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Data
     */
    private:
    // Typedef for allowed Box-containing objects
    using BoxContainingVariant = VariantParameterData<Configuration *, Structure>;
    // Box-containing input and output
    BoxContainingVariant inputVariant_, outputVariant_;
    // Box side length dimensions
    Vector3 lengths_{20.0, 20.0, 20.0};
    // Box angles
    Vector3 angles_{90, 90, 90};
    // Whether the box should be nonperiodic
    bool nonPeriodic_{false};

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};