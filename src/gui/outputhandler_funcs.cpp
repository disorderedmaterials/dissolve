// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
