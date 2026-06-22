// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "math/data2D.h"
#include "math/data3D.h"
#include "nodes/node.h"

class ExportBlockDataNode : public Node
{
    public:
    ExportBlockDataNode(Graph *parentGraph);
    ~ExportBlockDataNode() override = default;

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
    template <class DataClass> bool write(const DataClass &data, const std::string &filePath)
    {
        // Open the file
        LineParser parser;
        if (!parser.openOutput(filePath))
        {
            parser.closeFiles();
            return false;
        }

        if (data.valuesHaveErrors())
        {
            for (auto &&[value, error] : zip(data.values(), data.errors()))
                if (!parser.writeLineF("{:16.10e}  {:16.10e}\n", value, error))
                    return false;
        }
        else
            for (auto value : data.values())
                if (!parser.writeLineF("{:16.10e}\n", value))
                    return false;

        return true;
    }
};
