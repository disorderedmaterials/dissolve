// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/cifLoader.h"
#include "nodes/node.h"

// CIFLoader Node
class CIFAtomicOptionsNode : public Node
{
    public:
    CIFAtomicOptionsNode(Graph *parentGraph);
    ~CIFAtomicOptionsNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // CIF handler context
    CIFLoaderNode::CIFContext *context_{nullptr};
    // Tolerance for removal of overlapping atoms
    Number overlapTolerance_{0.1};
    // Whether to remove free atomic moieties in clean-up
    bool removeAtomics_{false};
    // Whether to remove water and coordinated oxygen atoms in clean-up
    bool removeWaterAndCoordinateOxygens_{false};

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
