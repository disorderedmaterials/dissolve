// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/lineParser.h"
#include "io/import/data1D.h"
#include "math/data1D.h"
#include "math/filters.h"

// Read simple histogram data using specified parser
bool Data1DImportFileFormat::importHistogram(LineParser &parser, Data1D &data)
{
    // Read in the data first (assuming simple XY format)
    if (!importXY(parser, data))
        return false;

    // Convert bin boundaries to centre-bin values
    Filters::convertBinBoundaries(data);

    return true;
}
