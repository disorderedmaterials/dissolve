// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/cifLoader.h"
#include "nodes/node.h"

// CIFLoader Node
class CIFCleanupNode : public Node
{
    public:
    CIFCleanupNode(Graph *parentGraph);
    ~CIFCleanupNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // CIF handler context
    CIFLoaderNode::CIFContext *context_{nullptr};
    // Whether to remove by NETA definition in clean-up
    bool removeNETA_{false};
    // Whether to expand NETA matches to fragments when removing in clean-up
    bool removeNETAByFragment_{false};
    // NETA for moiety removal, if specified
    std::string moietyRemovalNETA_;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
