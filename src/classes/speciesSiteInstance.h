// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <vector>

// Species Site Instance
class SpeciesSiteInstance
{
    public:
    SpeciesSiteInstance(const std::vector<int> &allIndices = {}, const std::vector<int> &originIndices = {},
                        const std::vector<int> &xAxisIndices = {}, const std::vector<int> &yAxisIndices = {});
    ~SpeciesSiteInstance() = default;

    private:
    // Indices of all atoms that correspond to the site
    std::vector<int> allIndices_;
    // Indices of atoms which contribute to the origin of the site
    std::vector<int> originIndices_;
    // Indices of atoms which indicate the x axis with the origin
    std::vector<int> xAxisIndices_;
    // Indices of atoms which indicate the y axis with the origin, after orthogonalisation
    std::vector<int> yAxisIndices_;

    public:
    // Return all atom indices representing the site
    const std::vector<int> &allIndices() const;
    // Return atom indices contributing to site origin
    const std::vector<int> &originIndices() const;
    // Return atom indices indicating the x axis with the origin
    const std::vector<int> &xAxisIndices() const;
    // Return atom indices indicating the y axis with the origin
    const std::vector<int> &yAxisIndices() const;
};
