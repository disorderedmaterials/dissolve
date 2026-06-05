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
    std::optional<IntegerHistogram1D> totalOxygensHistogram_;
    Data1D totalOxygens_;
    // Modifier to oxygen type sites histogram
    std::optional<IntegerHistogram1D> oxygenSitesHistogram_;
    Data1D oxygenSites_;
    // Modifier - Free Oxygens
    std::optional<Histogram1D> histMFO_;
    Data1D distanceMFO_;
    // Modifier - Non-Bridging Oxygens
    std::optional<Histogram1D> histMNBO_;
    Data1D distanceMNBO_;
    // Modifier - Bridging Oxygens
    std::optional<Histogram1D> histMBO_;
    Data1D distanceMBO_;
    // Modifier - Other Oxygens
    std::optional<Histogram1D> histMOtherO_;
    Data1D distanceMOtherO_;

    public:
    // Clear any local data
    void clearData() override;
    // Temporary accessors to data for testing
    const IntegerHistogram1D &oxygenSitesHistogram() const;
    const Data1D &oxygenSites() const;
    const IntegerHistogram1D &totalOxygensHistogram() const;
    const Data1D &totalOxygens() const;
    const Data1D &distanceMFO() const;
    const Data1D &distanceMNBO() const;
    const Data1D &distanceMBO() const;
    const Data1D &distanceMOtherO() const;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
