// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/species.h"
#include "nodes/node.h"

class AtomicSpeciesNode : public Node
{
    public:
    AtomicSpeciesNode(Graph *parentGraph);
    ~AtomicSpeciesNode() override = default;

    /*
     * Definition
     */
    public:
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Data
     */
    private:
    // Element
    Elements::Element element_;
    // Species parameter string
    std::string parameters_{""};
    // Species
    Species species_;

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};