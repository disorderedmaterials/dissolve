// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "math/histogram1D.h"
#include "nodes/node.h"

class MoleculeTorsionNode : public Node
{
    public:
    MoleculeTorsionNode(Graph *parentGraph);
    ~MoleculeTorsionNode() override = default;

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
    // Target Species
    const Species *species_{nullptr};
    // Target atom indices
    Number i_{0}, j_{1}, k_{2}, l_{3};
    // Torsion histogram
    std::optional<Histogram1D> histogram_;
    Data1D frequency_;

    public:
    // Clear any local data
    void clearData() override;
    // Temporary accessors to data for testing
    const Data1D &frequency() const;

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
