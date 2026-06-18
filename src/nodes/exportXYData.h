// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"

class ExportXYDataNode : public Node
{
    public:
    ExportXYDataNode(Graph *parentGraph);
    ~ExportXYDataNode() override = default;

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
    // Data to export
    std::optional<Data1D> data_;
    // File path
    std::string filePath_;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;

    public:
    // Write data specified
    static bool write(const Data1D &data, const std::string &filePath);
};
