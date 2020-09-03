/*
    *** GUI Output Handler
    *** src/gui/outputhandler_funcs.cpp
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

#include "gui/outputhandler.hui"
#include <QColor>

GUIOutputHandler::GUIOutputHandler() {}

GUIOutputHandler::~GUIOutputHandler() {}

/*
 * Styling
 */

// Set styling for warning message
void GUIOutputHandler::styleForWarning()
{
    static QColor blue(0, 0, 255);
    emit(setColour(blue));
}

// Set styling for error message
void GUIOutputHandler::styleForError()
{
    static QColor red(251, 0, 0);
    emit(setColour(red));
}

// Reset styling for normal text
void GUIOutputHandler::resetStyling()
{
    static QColor black(0, 0, 0);
    emit(setColour(black));
}

/*
 * Output
 */

// Print text
void GUIOutputHandler::outputText(std::string s) { emit(printText(QStringLiteral("%1\n").arg(s.c_str()))); }
