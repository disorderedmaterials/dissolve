// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/cifLoader.h"
#include "nodes/node.h"

// CIFLoader Node
class CIFBondingOptionsNode : public Node
{
    public:
    CIFBondingOptionsNode(Graph *parentGraph);
    ~CIFBondingOptionsNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // CIF handler context
    CIFLoaderNode::CIFContext *context_{nullptr};
    // Bonding tolerance, if calculating bonding rather than using CIF definitions
    Number bondingTolerance_{1.1};
    // Whether to use CIF bonding definitions
    bool useCIFBondingDefinitions_{false};
    // Whether to prevent metallic bonding
    bool preventMetallicBonds_{true};

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
