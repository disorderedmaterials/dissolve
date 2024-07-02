// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/speciesBeadInstance.h"

SpeciesBeadInstance::SpeciesBeadInstance(const std::vector<int> &allIndices, const std::vector<int> &originIndices,
                                         const std::vector<int> &xAxisIndices, const std::vector<int> &yAxisIndices)
    : allIndices_(allIndices), originIndices_(originIndices), xAxisIndices_(xAxisIndices), yAxisIndices_(yAxisIndices)
{
}

// Return all atom indices representing the bead
const std::vector<int> &SpeciesBeadInstance::allIndices() const { return allIndices_; }

// Return atom indices contributing to bead origin
const std::vector<int> &SpeciesBeadInstance::originIndices() const { return originIndices_; }

// Return atom indices indicating the x axis with the origin
const std::vector<int> &SpeciesBeadInstance::xAxisIndices() const { return xAxisIndices_; }

// Return atom indices indicating the y axis with the origin
const std::vector<int> &SpeciesBeadInstance::yAxisIndices() const { return yAxisIndices_; }
