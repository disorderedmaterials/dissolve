// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/importDLPUtilsSurface.h"
#include "base/lineParser.h"

ImportDLPUtilsSurfaceNode::ImportDLPUtilsSurfaceNode(Graph *parentGraph) : Node(parentGraph)
{
    // Options
    addOption<std::string>("FilePath", "File path", filePath_);

    // Outputs
    addOutput<std::optional<Data2D>>("Data", "Imported data", data_);
}

std::string_view ImportDLPUtilsSurfaceNode::type() const { return "ImportDLPUtilsSurf2D"; }

std::string_view ImportDLPUtilsSurfaceNode::summary() const { return "Import 1D data."; }

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
    data.clear();

    // Open file and check that we're OK to proceed importing from it
    LineParser parser;
    if ((!parser.openInput(filePath)) || (!parser.isFileGoodForReading()))
        return false;

    // Data is in blocks of common Y value, three-columns:  x  y  f(x,y)
    std::vector<double> xAxis, yAxis, values;
    auto firstLineOfBlock = true;
    while (!parser.eofOrBlank())
    {
        if (parser.getArgsDelim(LineParser::KeepBlanks) != LineParser::Success)
            return false;

        // Is this a blank line inbetween blocks?
        if (parser.nArgs() == 0)
        {
            firstLineOfBlock = true;
            continue;
        }

        // If this is the first line of the block, re-start x-axis storage and push next y value
        if (firstLineOfBlock)
        {
            xAxis.clear();
            yAxis.push_back(parser.argd(1));
            firstLineOfBlock = false;
        }

        // Store the x-axis value
        xAxis.push_back(parser.argd(0));

        // Store the value
        values.push_back(parser.argd(2));
    }

    parser.closeFiles();

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