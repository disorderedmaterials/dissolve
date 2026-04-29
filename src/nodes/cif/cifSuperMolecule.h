// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/cif/io/cifContext.h"
#include "nodes/node.h"

// ImportCIFStructure Node
class CIFSuperMoleculeNode : public Node
{
    public:
    CIFSuperMoleculeNode(Graph *parentGraph);
    ~CIFSuperMoleculeNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // CIF handler context
    CIFContext *context_{nullptr};
    // Non-periodic species
    const Species *nonPeriodicSpecies_{nullptr};
    // Supercell species
    std::unique_ptr<Species> supercellSpecies_;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
