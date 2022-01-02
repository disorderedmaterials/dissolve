// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "base/lineparser.h"
#include "io/import/data1d.h"
#include "math/data1d.h"
#include "math/filters.h"

// Read Gudrun merged interference cross-section (mint) data using specified parser
bool Data1DImportFileFormat::importGudrunMint(LineParser &parser, Data1D &data)
{
    // Read in the data first (assuming simple XY format)
    if (!importXY(parser, data))
        return false;

    // Gudrun mint01 files are in histogram format, so convert bin boundaries to centre-bin values
    Filters::convertBinBoundaries(data);

    return true;
}
