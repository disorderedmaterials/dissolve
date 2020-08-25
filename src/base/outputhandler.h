/*
    *** Output Handler
    *** src/base/outputhandler.h
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

#pragma once

#include <string>

// Forward Declarations
/* none */

// Messenger output handler
class OutputHandler
{
    public:
    OutputHandler();
    virtual ~OutputHandler();

    /*
     * Styling
     */
    public:
    // Set styling for warning message
    virtual void styleForWarning() = 0;
    // Set styling for error message
    virtual void styleForError() = 0;
    // Reset styling for normal text
    virtual void resetStyling() = 0;

    /*
     * Output
     */
    public:
    // Output text to device
    virtual void outputText(std::string s) = 0;
};
