// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "analyser/siteFilter.h"
#include "analyser/siteSelector.h"
#include "classes/speciesSites.h"
#include "io/export/data1D.h"
#include "io/import/data1D.h"
#include "math/range.h"
#include "math/vector3.h"
#include "nodes/node.h"
#include <string_view>

// Calculate RDF Module
class SiteRDFNode : public Node
{
    using Sums = std::map<std::string, std::pair<SampledDouble, std::optional<Data1D>>>;

    public:
    SiteRDFNode(Graph *parentGraph);
    ~SiteRDFNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *configuration_{nullptr};
    // Target SpeciesSite definitions
    SpeciesSites a_, b_;
    // Sums
    Sums sums_;
    // Whether to exclude correlations between sites on the same molecule
    bool excludeSameMolecule_{true};
    // Range (min, max, delta) of distance axis
    Vector3 distanceRange_{0.0, 10.0, 0.05};
    // Ranges for sums
    Range rangeA_{0.0, 3.0};
    Range rangeB_{3.0, 6.0};
    Range rangeC_{6.0, 9.0};
    // Site RDF
    Data1D dataRDF_;
    // Flags for ranges
    bool rangeEnabled_[3] = {true, false, false};
    // Whether to calculate the instantaneous coordination numbers rather than forming an average
    bool instantaneous_{false};
    // Whether to export instantaneous coordination numbers to disk
    bool exportInstantaneous_{false};
    // Export target
    Data1DExportFileFormat exportFileAndFormat_;

    public:
    // Return whether specified coordination number range is enabled
    bool isRangeEnabled(int id) const;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;

    // Getters
    public:
    const Data1D &dataRDF();
    const Sums &sums();
};
