// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "io/export/species.h"
#include "math/sampledVector.h"
#include "nodes/node.h"
#include "nodes/parameter.h"

class Configuration;
class SpeciesSite;

class AvgMolNode : public Node
{
    public:
    AvgMolNode(Graph *parentGraph);
    ~AvgMolNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Target site
    const SpeciesSite *targetSite_{nullptr};
    // Whether to save average coordinates to disk
    SpeciesExportFileFormat exportFileAndFormat_;
    // Species targeted by module (derived from selected site)
    const Species *targetSpecies_{nullptr};
    // Local Species representing average of targeted Species
    Species averageSpecies_;

    private:
    SampledVector x_, y_, z_;
    // Ensure arrays are the correct size for the current target Species
    void updateArrays();
    // Update the local species with the coordinates from the supplied arrays
    void updateSpecies();

    /*
     * Processing
     */
    public:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
