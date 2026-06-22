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
    std::string_view type() const override;
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
    // Run main processing
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
