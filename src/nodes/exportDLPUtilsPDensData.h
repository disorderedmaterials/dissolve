// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "math/data3D.h"
#include "nodes/node.h"

class ExportDLPUtilsPDensDataNode : public Node
{
    public:
    ExportDLPUtilsPDensDataNode(Graph *parentGraph);
    ~ExportDLPUtilsPDensDataNode() override = default;

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
    Data3D data_;
    // File path
    std::string filePath_;

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;

    public:
    // Write data
    static bool write(const Data3D &data, const std::string &filePath);
};
