// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "math/data2D.h"
#include "nodes/node.h"

class ImportDLPUtilsSurfaceNode : public Node
{
    public:
    ImportDLPUtilsSurfaceNode(Graph *parentGraph);
    ~ImportDLPUtilsSurfaceNode() override = default;

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
    // Imported data
    std::optional<Data2D> data_;

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;

    public:
    // Read data specified
    static bool read(Data2D &data, std::string filePath);
};
