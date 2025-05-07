// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/moleculeSet.h"
#include "nodes/insertBase.h"

// InsertRandom Node
class InsertRandomNode : public InsertNodeBase
{
    public:
    InsertRandomNode(Graph *parentGraph);
    ~InsertRandomNode() override = default;

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
    // Species to be added (if no MoleculeSet is given)
    const Species *species_{nullptr};
    // MoleculeSet to be added (if no Species is given)
    const MoleculeSet *moleculeSet_{nullptr};

    /*
     * Processing
     */
    public:
    // Run main processing
    NodeConstants::ProcessResult process();
};
