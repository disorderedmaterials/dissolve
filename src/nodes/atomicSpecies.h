// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/moleculeSet.h"
#include "classes/species.h"
#include "nodes/node.h"
#include <string>

// AtomicSpecies Node
class AtomicSpeciesNode : public Node
{
    public:
    AtomicSpeciesNode(Graph *parentGraph);
    ~AtomicSpeciesNode() override = default;

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
    std::unique_ptr<Species> species_{std::make_unique<Species>()};
    // Local raw pointer to species
    Species *rawSpecies_{species_.get()};
    // Element symbol
    std::string_view Z_{"Unknown"};
    // Species name
    std::string_view name_;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
