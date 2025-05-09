// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/species.h"
#include "math/vector3.h"
#include "nodes/node.h"
#include <string>

// AtomicSpecies Node
class AtomicSpeciesNode : public Node
{
    public:
    AtomicSpeciesNode(Graph *parentGraph, Elements::Element Z);
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
    Species species_;
    // AtomTypes owned by the node
    std::vector<std::shared_ptr<AtomType>> atomTypes_;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
