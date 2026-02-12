// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "analyser/siteFilter.h"
#include "analyser/siteSelector.h"
#include "io/export/data1D.h"
#include "math/range.h"
#include "nodes/node.h"
#include "classes/speciesSites.h"
#include "math/vector3.h"
#include <string_view>
#include "io/import/data1D.h"

// Calculate RDF Module
class SiteRDFNode : public Node
{
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
    Configuration *targetConfiguration_{nullptr};
    // Target SpeciesSite definitions
    SpeciesSites a_, b_;
    // Whether to exclude correlations between sites on the same molecule
    bool excludeSameMolecule_{true};
    // Range (min, max, delta) of distance axis
    Vector3 distanceRange_{0.0, 10.0, 0.05};
    // Ranges for sums
    Range range_[3] = {{0.0, 3.0}, {3.0, 6.0}, {6.0, 9.0}};
    // Flags for ranges
    bool rangeEnabled_[3] = {true, false, false};
    // Whether to calculate the instantaneous coordination numbers rather than forming an average
    bool instantaneous_{false};
    // Whether to export instantaneous coordination numbers to disk
    bool exportInstantaneous_{false};
    // Export target
    Data1DImportFileFormat::Data1DImportFormat exportFileAndFormat_;

    public:
    // Return whether specified coordination number range is enabled
    bool isRangeEnabled(int id) const;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
