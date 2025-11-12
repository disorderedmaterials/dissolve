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
    // TOML Definition
    std::string definition_;
    // Species object
    Species species_;
    // Atom Types owned by the node
    std::vector<std::shared_ptr<AtomType>> atomTypes_;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
