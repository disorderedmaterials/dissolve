// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/cifLoader.h"
#include "nodes/node.h"

// CIFLoader Node
class CIFConfigurationNode : public Node
{
    public:
    CIFConfigurationNode(Graph *parentGraph);
    ~CIFConfigurationNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // CIF handler context
    CIFLoaderNode::CIFContext *context_{nullptr};
    // Supercell configuration
    Configuration *supercellConfiguration_{nullptr};
    // Unit cell species
    const Species *unitCellSpecies_{nullptr};
    // Supercell species
    const Species *supercellSpecies_{nullptr};
    // Detected molecular species
    std::vector<CIFMolecularSpecies> molecularSpecies_;
    // Supercell repeat
    Vector3i supercellRepeat_{1, 1, 1};

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
