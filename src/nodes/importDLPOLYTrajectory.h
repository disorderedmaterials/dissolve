// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/applicative.h"
#include "classes/structure.h"
#include "nodes/node.h"

class ImportDLPOLYTrajectoryNode : public Node
{
    public:
    ImportDLPOLYTrajectoryNode(Graph *parentGraph);
    ~ImportDLPOLYTrajectoryNode() override = default;

    /*
     * Definition
     */
    public:
    // Return type of the node
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Data
     */
    private:
    // File path
    std::string filePath_;
    // Last read file position
    std::streampos filePosition_;
    // Structure
    Structure structure_;

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;

    public:
    // Parse file header
    static Parsers::Parser<std::tuple<int, int, int, int, double>> header();
};
