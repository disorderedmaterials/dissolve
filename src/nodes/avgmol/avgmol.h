// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"
#include "nodes/parameter.h"

class AvgMolNode : public Node
{
    public:
    AvgMolNode(Graph *parentGraph);
    ~AvgMolNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Processing
     */
    public:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
