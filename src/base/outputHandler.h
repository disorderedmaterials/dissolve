// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <string>

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
