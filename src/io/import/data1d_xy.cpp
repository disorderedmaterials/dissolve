/*
    *** Import - XY Data1D
    *** src/io/import/data1d_xy.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "base/lineparser.h"
#include "io/import/data1d.h"
#include "math/data1d.h"

// Read simple XY data using specified parser
bool Data1DImportFileFormat::importXY(LineParser &parser, Data1D &data)
{
    // Grab column indices
    const auto xCol = keywords_.asInt("X") - 1;
    const auto yCol = keywords_.asInt("Y") - 1;
    const auto errorCol = keywords_.asInt("Error") - 1;

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
