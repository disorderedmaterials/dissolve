// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/structure.h"
#include "math/sampledVector.h"
#include "nodes/node.h"

// Forward Declarations
class SpeciesSite;

class AverageMoleculeNode : public Node
{
    public:
    AverageMoleculeNode(Graph *parentGraph);
    ~AverageMoleculeNode() override = default;

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
    // Target configuration
    Configuration *configuration_{nullptr};
    // Target site
    const SpeciesSite *targetSite_{nullptr};
    // Averaged coordinates / structure
    std::optional<SampledVector> sampledX_, sampledY_, sampledZ_;
    Structure structure_;

    public:
    // Clear any local data
    void clearData() override;
    // Temporary accessors to data for testing
    const Structure &structure() const;

    /*
     * Processing
     */
    private:
    // Update the average structure from sampled coordinate vectors
    void updateStructure();

    protected:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
