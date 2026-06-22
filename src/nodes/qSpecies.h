// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/speciesSites.h"
#include "math/histogram1D.h"
#include "math/integerHistogram1D.h"
#include "math/range.h"
#include "nodes/node.h"

// Q-Species
class QSpeciesNode : public Node
{
    public:
    QSpeciesNode(Graph *parentGraph);
    ~QSpeciesNode() override = default;

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
    // Target configuration
    Configuration *configuration_{nullptr};
    // Target SpeciesSite definitions
    SpeciesSites bridgingOxygenSpeciesSites_, networkFormerSpeciesSites_;
    // Whether to exclude correlations between sites on the same molecule
    bool excludeSameMolecule_{false};
    // Distance range for calculation
    Range distanceRange_{0.0, 2.5};
    // Q species histogram
    std::optional<IntegerHistogram1D> qSpeciesHistogram_;
    Data1D qSpecies_;
    // Oxygen sites histogram
    std::optional<IntegerHistogram1D> oxygenSitesHistogram_;
    Data1D oxygenSites_;

    public:
    // Clear any local data
    void clearData() override;
    // Temporary accessors to data for testing
    const Data1D &qSpecies() const;

    /*
     * Processing
     */
    protected:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
