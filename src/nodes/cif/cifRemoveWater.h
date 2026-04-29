// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/cif/io/cifContext.h"
#include "nodes/node.h"

// ImportCIFStructure Node
class CIFRemoveWaterNode : public Node
{
    public:
    CIFRemoveWaterNode(Graph *parentGraph);
    ~CIFRemoveWaterNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // CIF handler context
    CIFContext *context_{nullptr};
    // Whether to remove water and coordinated oxygen atoms in clean-up
    bool removeWaterAndCoordinatedOxygens_{false};

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
