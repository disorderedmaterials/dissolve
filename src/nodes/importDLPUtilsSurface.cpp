// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/importDLPUtilsSurface.h"
#include "base/applicative.h"
#include "base/parserLibrary.h"

ImportDLPUtilsSurfaceNode::ImportDLPUtilsSurfaceNode(Graph *parentGraph) : Node(parentGraph)
{
    // Options
    addOption("FilePath", "File path", filePath_);

    // Outputs
    addOutput("Data", "Imported data", data_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view ImportDLPUtilsSurfaceNode::type() const { return "ImportDLPUtilsSurface"; }

// Return short summary of the node's purpose
std::string_view ImportDLPUtilsSurfaceNode::summary() const { return "Import DLPUtils 2D surface data (.surf)"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult ImportDLPUtilsSurfaceNode::process()
{
    // Create the data
    data_.emplace();

    if (!read(*data_, filePath_))
        return error("Failed to read DLPUtils Surface data from file '{}'.\n", filePath_);

    return NodeConstants::ProcessResult::Success;
}

// Read data specified
bool ImportDLPUtilsSurfaceNode::read(Data2D &data, std::string filePath)
{
    using namespace Parsers;
    data.clear();

    // Open file and check that we're OK to proceed importing from it
    std::ifstream infile(filePath);
    if (!infile)
        return false;

    // Data is in blocks of common Y value, three-columns:  x  y  f(x,y)
    auto block = some(maybe(inlineSpaces()) >> vector3() << newlines());
    auto blocks = some(block << spaces());
    std::vector<double> xAxis, yAxis, values;

    auto result = blocks.exact(infile);
    if (!result)
        return false;
    for (auto group : *result)
    {
        xAxis.clear();
        yAxis.push_back(group[0].y);
        for (auto point : group)
        {
            xAxis.push_back(point.x);
            values.push_back(point.z);
        }
    }

    infile.close();

    // Validity check on number of points in loaded file
    if (xAxis.size() * yAxis.size() != values.size())
        return false;

    // Initialise the array and poke values back in the correct order

    data.initialise(xAxis.size(), yAxis.size(), false);
    data.xAxis() = xAxis;
    data.yAxis() = yAxis;
    auto index = 0;
    for (auto y = 0; y < yAxis.size(); ++y)
        for (auto x = 0; x < xAxis.size(); ++x)
            data.values()[{x, y}] = values[index++];

    return true;
}
