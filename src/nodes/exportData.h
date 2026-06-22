// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "math/data2D.h"
#include "math/data3D.h"
#include "nodes/node.h"

class ExportDataNode : public Node
{
    public:
    ExportDataNode(Graph *parentGraph);
    ~ExportDataNode() override = default;

    /*
     * Definition
     */
    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Data
     */
    public:
    // Typedef for allowed data objects
    using DataContainingVariant = VariantParameterData<Data1D, Data2D, Data3D>;

    private:
    // Data to export
    DataContainingVariant data_;
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
    static bool write(const Data1D &data, const std::string &filePath);
    static bool write(const Data2D &data, const std::string &filePath);
    static bool write(const Data3D &data, const std::string &filePath);
};
