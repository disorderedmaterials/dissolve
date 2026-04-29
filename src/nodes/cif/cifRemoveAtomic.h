// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/cif/io/cifContext.h"
#include "nodes/node.h"

// ImportCIFStructure Node
class CIFRemoveAtomicNode : public Node
{
    public:
    CIFRemoveAtomicNode(Graph *parentGraph);
    ~CIFRemoveAtomicNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // CIF handler context
    CIFContext *context_{nullptr};
    // Whether to remove free atomic moieties in clean-up
    bool removeAtomics_{false};

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
