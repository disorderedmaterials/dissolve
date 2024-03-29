// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"
#include <QObject>

class GUIOutputHandler : public QObject, public OutputHandler
{
    Q_OBJECT

    public:
    GUIOutputHandler() = default;
    ~GUIOutputHandler() = default;

    /*
     * Styling
     */
    public:
    // Set styling for warning message
    void styleForWarning() override;
    // Set styling for error message
    void styleForError() override;
    // Reset styling for normal text
    void resetStyling() override;

    Q_SIGNALS:
    void setColour(const QColor &);

    /*
     * Output
     */
    public:
    // Print text
    void outputText(std::string s) override;

    Q_SIGNALS:
    void printText(const QString &s);
};
