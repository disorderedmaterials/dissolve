// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/species.h"
#include "nodes/node.h"

// Species Node
class SpeciesNode : public Node
{
    public:
    SpeciesNode(Graph *parentGraph);
    ~SpeciesNode() override = default;

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
    // Species object
    Species species_;
    // Atom Types owned by the node
    std::vector<std::shared_ptr<AtomType>> atomTypes_;

    /*
     * Accessors
     */
    public:
    // Access the species information (Needed for SpeciesModel)
    Species &species();
    const Species &species() const;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
