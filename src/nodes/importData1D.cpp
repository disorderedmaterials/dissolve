// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/importData1D.h"
#include "base/lineParser.h"
#include "math/data1D.h"
#include "math/filters.h"
#include "nodes/dissolve.h"

ImportData1DNode::ImportData1DNode(Graph *parentGraph) : Node(parentGraph)
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
    addOption<Number>("ErrorColumn", "Column index of data error values", errorColumn_);

    // Outputs
    addOutput<std::optional<Data1D>>("Data", "Imported data", data_);
}

std::string_view ImportData1DNode::type() const { return "Data1DImport"; }

std::string_view ImportData1DNode::summary() const { return "Import 1D data."; }

NodeConstants::ProcessResult ImportData1DNode::process()
{
    auto xMin = xMin_ ? std::make_optional<double>(xMin_->asDouble()) : std::nullopt;
    auto xMax = xMax_ ? std::make_optional<double>(xMax_->asDouble()) : std::nullopt;
    auto removeAverageFromX = removeAverageFromX_ ? std::make_optional<double>(removeAverageFromX_->asDouble()) : std::nullopt;
    auto nPointsToRemove = nPointsToRemove_.asInteger();
    auto xCol = xColumn_.asInteger() - 1, yCol = yColumn_.asInteger() - 1, errorCol = errorColumn_.asInteger() - 1;

    // Clear the structure, and initialise error arrays if necessary
    data_.emplace();
    if (errorCol != -1)
        data_->addErrors();

    // Parse the file
    if (!read(*data_, filePath_, xCol, yCol, errorCol, histogram_))
        return error("Failed to read Data1D from file '{}'\n", filePath_);

    // Handle any additional options
    // -- Remove points from the start of the data?
    for (auto n = 0; n < nPointsToRemove; ++n)
        data_->removeFirstPoint();
    // -- Trim range?
    if (xMin || xMax)
        Filters::trim(*data_, xMin.value_or(data_->xAxis().front() - 1.0), xMax.value_or(data_->xAxis().back() + 1.0));

    // -- Subtract average level from data?
    if (removeAverageFromX)
    {
        double level = Filters::subtractAverage(*data_, removeAverageFromX.value());
        message("Removed average level of {} from data, forming average over x >= {}.\n", level, removeAverageFromX.value());
    }

    // Validity check on number of points in loaded file
    if (data_->nValues() == 0)
        return error("File '{}' contains no data.\n", filePath_);

    return NodeConstants::ProcessResult::Success;
}

// Read data specified
bool ImportData1DNode::read(Data1D &data, std::string filePath, int xColumn, int yColumn, int errorColumn, bool histogram)
{
    // Clear the data
    data.clear();

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