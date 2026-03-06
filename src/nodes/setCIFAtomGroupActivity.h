// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/cifLoader.h"
#include "nodes/node.h"

// CIFLoader Node
class SetCIFAtomGroupActivityNode : public Node
{
    public:
    SetCIFAtomGroupActivityNode(Graph *parentGraph);
    ~SetCIFAtomGroupActivityNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // CIF handler context
    CIFLoaderNode::CIFContext *context_{nullptr};
    // Selected CIF assembly atom group name
    std::string atomGroupName_;
    // Selected CIF assembly name
    std::string assemblyName_;
    // Activity status of CIF atom group
    bool active_;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
