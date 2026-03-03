// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/box.h"
#include "classes/configuration.h"
#include "nodes/node.h"
#include <memory>

class SetCellNode : public Node
{
    public:
    SetCellNode(Graph *parentGraph);
    ~SetCellNode() override = default;

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
    // Configuration object
    Configuration *targetConfiguration_;
    // Box side length dimensions
    Vector3 lengths_{1.0, 1.0, 1.0};
    // Box angles
    Vector3 angles_{90, 90, 90};
    // Box type
    bool nonPeriodic_{false};

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};