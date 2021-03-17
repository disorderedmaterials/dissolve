// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/lineparser.h"
#include "io/import/data2d.h"
#include "math/data2d.h"
#include "math/filters.h"

// Read cartesian data from specified file
bool Data2DImportFileFormat::importCartesian(LineParser &parser, Data2D &data)
{
    /*
     * Cartesian coordinates assume that there are three values per line: x, y, and value.
     * The x and y values are assumed to be centre-bin values.
     */

    // Must have axis ranges - otherwise tricky to work out axis limits without reading the whole file in twice
    if ((!keywords_.isSet("XRange")) || (!keywords_.isSet("YRange")))
        return Messenger::error("Must supply x and y ranges (e.g. xrange=min,max,delta) when importing 2D cartesian data.\n");

    // Set up our data
    const auto xRange = keywords_.asVec3Double("XRange");
    const auto xMin = xRange.x;
    const auto xMax = xRange.y;
    const auto xDelta = xRange.z;
    const auto yRange = keywords_.asVec3Double("YRange");
    const auto yMin = yRange.x;
    const auto yMax = yRange.y;
    const auto yDelta = yRange.z;
    data.initialise(xMin, xMax, xDelta, yMin, yMax, yDelta);

    // Loop over lines in the file - we expect blocks with three columns (x, y, f(x,y))
    while (!parser.eofOrBlank())
    {
        // Read line
        if (parser.getArgsDelim() != LineParser::Success)
            return false;

        // Check number of arguments
        if (parser.nArgs() != 3)
        {
            Messenger::warn("Skipping bad line: {}...\n", parser.line());
            continue;
        }

        double x = parser.argd(0);
        double y = parser.argd(1);
        auto xBin = (x - xMin) / xDelta;
        if ((xBin < 0) || (xBin >= data.xAxis().size()))
        {
            Messenger::warn("Coordinates x={:e} y={:e} are out-of-range (xBin = {}, nBins = {}).\n", x, y, xBin,
                            data.xAxis().size());
            continue;
        }
        auto yBin = (y - yMin) / yDelta;
        if ((yBin < 0) || (yBin >= data.yAxis().size()))
        {
            Messenger::warn("Coordinates x={:e} y={:e} are out-of-range (yBin = {}, nBins = {}).\n", x, y, yBin,
                            data.yAxis().size());
            continue;
        }

        // Set the value
        data.value(xBin, yBin) = parser.argd(2);
    }

    return true;
}
