// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "data/ff/ff.h"
#include "nodes/node.h"

// Forcefield Node
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
    // Forcefield object
    std::shared_ptr<Forcefield> ff_;

    /*
     * Accessors
     */
    public:
    // Access the species information (Needed for ForcefieldModel)
    std::shared_ptr<Forcefield> &forcefield();
    const std::shared_ptr<Forcefield> &forcefield() const;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;

    // Serialise any hidden content
    void serialiseInternal(SerialisedValue &target) const override;
    // Deserialise any hidden content
    void deserialiseInternal(const SerialisedValue &target) override;
};
