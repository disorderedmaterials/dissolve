// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/cif/io/cifContext.h"
#include "nodes/node.h"

// ImportCIFStructure Node
class CIFMolecularSpeciesNode : public Node
{
    public:
    CIFMolecularSpeciesNode(Graph *parentGraph);
    ~CIFMolecularSpeciesNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // CIF handler context
    CIFContext *context_{nullptr};
    // Supercell configuration
    Configuration *supercellConfiguration_{nullptr};
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

    /*
     * Getters
     */
    public:
    // Get cleaned unit cell species
    const Species &cleanedUnitCellSpecies() const;
};
