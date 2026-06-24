// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "data/ff/recipe.h"
#include "nodes/node.h"

class ForcefieldNode : public Node
{
    public:
    ForcefieldNode(Graph *parentGraph);
    ~ForcefieldNode() override = default;

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
    // Recipe
    ForcefieldRecipe recipe_;

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;

    /*
     * Serialisation
     */
    protected:
    // Serialise any hidden content
    void serialiseInternal(SerialisedValue &target) const override;
    // Deserialise any hidden content
    void deserialiseInternal(const SerialisedValue &target) override;
};
