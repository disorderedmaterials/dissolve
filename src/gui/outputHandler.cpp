// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/outputHandler.h"
#include <QColor>
#include <QGuiApplication>
#include <QPalette>

/*
 * Styling
 */

// Set styling for warning message
void GUIOutputHandler::styleForWarning() { Q_EMIT(setColour(Qt::blue)); }

// Set styling for error message
void GUIOutputHandler::styleForError() { Q_EMIT(setColour(Qt::red)); }

// Reset styling for normal text
void GUIOutputHandler::resetStyling() { Q_EMIT(setColour(QGuiApplication::palette().text().color())); }

/*
 * Output
 */

// Print text
void GUIOutputHandler::outputText(std::string s) { Q_EMIT(printText(QStringLiteral("%1\n").arg(s.c_str()))); }
