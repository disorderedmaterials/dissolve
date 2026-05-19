// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/data1DImport.h"
#include "base/lineParser.h"
#include "io/import/data1D.h"
#include "math/data1D.h"
#include "math/filters.h"
#include "nodes/dissolve.h"

Data1DImportNode::Data1DImportNode(Graph *parentGraph) : Node(parentGraph)
{
    // Options
    addOption<std::string>("FilePath", "File path", filePath_);
    addOption<bool>("Histogram", "Whether format of imported data is histogram", histogram_);
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

std::string_view Data1DImportNode::type() const { return "Data1DImport"; }

std::string_view Data1DImportNode::summary() const { return "Import 1D data."; }

NodeConstants::ProcessResult Data1DImportNode::process()
{
    data_.emplace();
    auto xMin = xMin_ ? std::make_optional<double>(xMin_->asDouble()) : std::nullopt;
    auto xMax = xMax_ ? std::make_optional<double>(xMax_->asDouble()) : std::nullopt;
    auto removeAverageFromX = removeAverageFromX_ ? std::make_optional<double>(removeAverageFromX_->asDouble()) : std::nullopt;
    auto nPointsToRemove = nPointsToRemove_.asInteger();
    auto xCol = xColumn_.asInteger(), yCol = yColumn_.asInteger(), errorCol = errorColumn_.asInteger();

    // Open file and check that we're OK to proceed importing from it
    LineParser parser;
    if ((!parser.openInput(filePath_)) || (!parser.isFileGoodForReading()))
        return error("Couldn't open file '{}' for loading Data1D data.\n", filePath_);

    // Import Data1D using supplied parser and current format
    if (!importXY(parser, *data_, xCol, yCol, errorCol))
        return NodeConstants::ProcessResult::Failed;

    // If we have a histogram, convert bin boundaries to centre-bin values
    if (!histogram_)
        Filters::convertBinBoundaries(*data_);

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

    parser.closeFiles();

    // Validity check on number of points in loaded file
    if (data_->nValues() == 0)
        return error("File '{}' contains no data.\n", filePath_);

    return NodeConstants::ProcessResult::Success;
}

// Read simple XY data using specified parser
bool Data1DImportNode::importXY(LineParser &parser, Data1D &data, int xColumn, int yColumn, int errorColumn)
{
    // Get zero-indexed columns
    const auto xCol = xColumn - 1;
    const auto yCol = yColumn - 1;
    const auto errorCol = errorColumn - 1;

    // Clear the structure, and initialise error arrays if necessary
    data.clear();
    if (errorCol != -1)
        data.addErrors();

    while (!parser.eofOrBlank())
    {
        if (parser.getArgsDelim() != LineParser::Success)
        {
            error("Failed to read Data1D data from file.\n");
            return false;
        }

        // Check columns provided
        if ((xCol >= parser.nArgs()) || (yCol >= parser.nArgs()))
        {
            error("Error reading from '{}', as one or both columns specified ({} and {}) are not present.\n",
                  parser.inputFilename(), xCol + 1, yCol + 1);
            return false;
        }

        // Are we reading errors too?
        if (errorCol == -1)
            data.addPoint(parser.argd(xCol), parser.argd(yCol));
        else
        {
            if (errorCol >= parser.nArgs())
            {
                error("Error reading from '{}', as the error column specified ({}) is not present.\n", parser.inputFilename(),
                      errorCol + 1);
                return false;
            }

            data.addPoint(parser.argd(xCol), parser.argd(yCol), parser.argd(errorCol));
        }
    }

    return true;
}
