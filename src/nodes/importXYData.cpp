// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/importXYData.h"
#include "base/lineParser.h"
#include "math/data1D.h"
#include "math/filters.h"

ImportXYDataNode::ImportXYDataNode(Graph *parentGraph) : Node(parentGraph)
{
    // Options
    addOption<std::string>("FilePath", "File path", filePath_);
    addOption<bool>("Histogram", "Whether the data's x-values represent histogram bin boundaries (e.g. Gudrun mint files)",
                    histogram_);
    addOption<std::optional<Number>>("RemoveAverageFromX", "Remove average from X values", removeAverageFromX_);
    addOption<std::optional<Number>>("XMin", "Minimum X value", xMin_);
    addOption<std::optional<Number>>("XMax", "Maximum X value", xMax_);
    addOption<Number>("NPointsToRemove", "Number of points to remove", nPointsToRemove_);
    addOption<Number>("XColumn", "Column index of data X values", xColumn_);
    addOption<Number>("YColumn", "Column index of data Y values", yColumn_);
    addOption<std::optional<Number>>("ErrorColumn", "Column index of data error values", errorColumn_);

    // Outputs
    addOutput<std::optional<Data1D>>("Data", "Imported data", data_);
}

std::string_view ImportXYDataNode::type() const { return "Data1DImport"; }

std::string_view ImportXYDataNode::summary() const { return "Import 1D data."; }

NodeConstants::ProcessResult ImportXYDataNode::process()
{
    // Clear the structure, and initialise error arrays if necessary
    data_.emplace();
    if (errorColumn_)
        data_->addErrors();

    // Parse the file
    if (!read(*data_, filePath_, xColumn_.asInteger(), yColumn_.asInteger(), errorColumn_.value_or(Number(0)).asInteger(),
              histogram_))
        return error("Failed to read Data1D from file '{}'\n", filePath_);

    // Handle any additional options
    // -- Remove points from the start of the data?
    for (auto n = 0; n < nPointsToRemove_.asInteger(); ++n)
        data_->removeFirstPoint();
    // -- Trim range?
    if (xMin_ || xMax_)
    {
        auto xMin = xMin_ ? xMin_->asDouble() : data_->xAxis().front() - 1.0;
        auto xMax = xMax_ ? xMax_->asDouble() : data_->xAxis().back() + 1.0;
        Filters::trim(*data_, xMin, xMax);
    }
    // -- Subtract average level from data?
    if (removeAverageFromX_)
    {
        double level = Filters::subtractAverage(*data_, removeAverageFromX_->asDouble());
        message("Removed average level of {} from data, forming average over x >= {}.\n", level,
                removeAverageFromX_->asDouble());
    }

    // Validity check on number of points in loaded file
    if (data_->nValues() == 0)
        return error("File '{}' contains no data.\n", filePath_);

    return NodeConstants::ProcessResult::Success;
}

// Read data specified
bool ImportXYDataNode::read(Data1D &data, std::string filePath, int xColumn, int yColumn, int errorColumn, bool histogram)
{
    // Clear the data
    data.clear();

    // Convert human-indexed columns into zero-indexed ones
    --xColumn;
    --yColumn;
    --errorColumn;

    // Open file and check that we're OK to proceed importing from it
    LineParser parser;
    if ((!parser.openInput(filePath)) || (!parser.isFileGoodForReading()))
        return false;

    while (!parser.eofOrBlank())
    {
        if (parser.getArgsDelim() != LineParser::Success)
            return false;

        // Check columns provided
        if ((xColumn >= parser.nArgs()) || (yColumn >= parser.nArgs()))
            return false;

        // Are we reading errors too?
        if (errorColumn == -1)
            data.addPoint(parser.argd(xColumn), parser.argd(yColumn));
        else
        {
            if (errorColumn >= parser.nArgs())
                return false;

            data.addPoint(parser.argd(xColumn), parser.argd(yColumn), parser.argd(errorColumn));
        }
    }

    parser.closeFiles();

    // If we have a histogram, convert bin boundaries to centre-bin values
    if (histogram)
        Filters::convertBinBoundaries(data);

    return true;
}