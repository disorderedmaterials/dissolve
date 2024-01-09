// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/lineParser.h"
#include "io/import/data1D.h"
#include "math/data1D.h"

// Read simple XY data using specified parser
bool Data1DImportFileFormat::importXY(LineParser &parser, Data1D &data)
{
    // Get zero-indexed columns
    const auto xCol = xColumn_ - 1;
    const auto yCol = yColumn_ - 1;
    const auto errorCol = errorColumn_ - 1;

    // Clear the structure, and initialise error arrays if necessary
    data.clear();
    if (errorCol != -1)
        data.addErrors();

    while (!parser.eofOrBlank())
    {
        if (parser.getArgsDelim() != LineParser::Success)
            return Messenger::error("Failed to read Data1D data from file.\n");

        // Check columns provided
        if ((xCol >= parser.nArgs()) || (yCol >= parser.nArgs()))
        {
            return Messenger::error("Error reading from '{}', as one or both columns specified ({} and {}) are not present.\n",
                                    parser.inputFilename(), xCol + 1, yCol + 1);
        }

        // Are we reading errors too?
        if (errorCol == -1)
            data.addPoint(parser.argd(xCol), parser.argd(yCol));
        else
        {
            if (errorCol >= parser.nArgs())
            {
                return Messenger::error("Error reading from '{}', as the error column specified ({}) is not present.\n",
                                        parser.inputFilename(), errorCol + 1);
            }

            data.addPoint(parser.argd(xCol), parser.argd(yCol), parser.argd(errorCol));
        }
    }

    return true;
}
