// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "math/data3D.h"
#include "nodes/node.h"

class ImportDLPUtilsPDensNode : public Node
{
    public:
    ImportDLPUtilsPDensNode(Graph *parentGraph);
    ~ImportDLPUtilsPDensNode() override = default;

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
    // File path
    std::string filePath_;
    // Imported data
    std::optional<Data3D> data_;

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;

    public:
    // Read data specified
    static bool read(Data3D &data, std::string filePath);
};
