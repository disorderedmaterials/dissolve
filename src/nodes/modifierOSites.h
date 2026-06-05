// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/speciesSites.h"
#include "math/histogram1D.h"
#include "math/integerHistogram1D.h"
#include "math/range.h"
#include "nodes/node.h"

// ModifierOSites Node
class ModifierOSitesNode : public Node
{
    public:
    ModifierOSitesNode(Graph *parentGraph);
    ~ModifierOSitesNode() override = default;

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
    SpeciesSites bridgingOxygenSpeciesSites_, networkFormerSpeciesSites_, modifierSpeciesSites_;
    // Whether to exclude correlations between sites on the same molecule
    bool excludeSameMolecule_{false};
    // Distance range for calculation
    Range distanceRange_{0.0, 2.5}, modifierDistanceRange_{0.0, 2.0};

    /*
     * Data
     */
    private:
    // Total O Sites histogram
    std::optional<IntegerHistogram1D> modifierHistogram_;
    Data1D modifiers_;
    // Modifier to oxygen type sites histogram
    std::optional<IntegerHistogram1D> oxygenSitesHistogram_;
    Data1D oxygenSites_;
    // Modifier - Free Oxygens
    std::optional<Histogram1D> histMFO_;
    Data1D distancesMFO_;
    // Modifier - Non-Bridging Oxygens
    std::optional<Histogram1D> histMNBO_;
    Data1D distancesMNBO_;
    // Modifier - Bridging Oxygens
    std::optional<Histogram1D> histMBO_;
    Data1D distancesMBO_;
    // Modifier - Other Oxygens
    std::optional<Histogram1D> histMOtherO_;
    Data1D distancesMOtherO_;

    public:
    // Clear any local data
    void clearData() override;
    // Temporary accessors to data for testing
    const Data1D &rdfBC() const;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
