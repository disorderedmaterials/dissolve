// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/structure.h"
#include "nodes/node.h"

class ImportXYZTrajectoryNode : public Node
{
    public:
    ImportXYZTrajectoryNode(Graph *parentGraph);
    ~ImportXYZTrajectoryNode() override = default;

    /*
     * Definition
     */
    public:
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Data
     */
    private:
    // File path
    std::string filePath_;
    // Last read file position
    std::streampos filePosition_{0};
    // Structure
    Structure structure_;

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
