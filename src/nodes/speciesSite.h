// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/speciesSite.h"
#include "nodes/node.h"

// SpeciesSite Node
class SpeciesSiteNode : public Node
{
    public:
    SpeciesSiteNode(Graph *parentGraph);
    ~SpeciesSiteNode() override = default;

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
    // SpeciesSite object
    SpeciesSite speciesSite_;
    // Species source
    Species *species_;
    // Species type
    SpeciesSite::SiteType type_;

    /*
     * Accessors
     */
    public:
    // Access the species information (Needed for SpeciesModel)
    SpeciesSite &speciesSite();
    const SpeciesSite &speciesSite() const;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
