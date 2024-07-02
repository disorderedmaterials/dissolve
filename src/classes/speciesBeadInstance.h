// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <vector>

// Species Bead Instance
class SpeciesBeadInstance
{
    public:
    SpeciesBeadInstance(const std::vector<int> &allIndices = {}, const std::vector<int> &originIndices = {},
                        const std::vector<int> &xAxisIndices = {}, const std::vector<int> &yAxisIndices = {});
    ~SpeciesBeadInstance() = default;

    private:
    // Indices of all atoms that correspond to the bead
    std::vector<int> allIndices_;
    // Indices of atoms which contribute to the origin of the bead
    std::vector<int> originIndices_;
    // Indices of atoms which indicate the x axis with the origin
    std::vector<int> xAxisIndices_;
    // Indices of atoms which indicate the y axis with the origin, after orthogonalisation
    std::vector<int> yAxisIndices_;

    public:
    // Return all atom indices representing the bead
    const std::vector<int> &allIndices() const;
    // Return atom indices contributing to bead origin
    const std::vector<int> &originIndices() const;
    // Return atom indices indicating the x axis with the origin
    const std::vector<int> &xAxisIndices() const;
    // Return atom indices indicating the y axis with the origin
    const std::vector<int> &yAxisIndices() const;
};
