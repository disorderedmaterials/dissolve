// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "templates/listitem.h"

// Forward Declarations
/* none */

// Text Formatter
class TextFormat : public ListItem<TextFormat>
{
    public:
    // Constructor / Desctructor
    TextFormat();
    ~TextFormat();
    TextFormat(const TextFormat &source);
    void operator=(const TextFormat &source);

    /*
     * Definition
     */
    private:
    // Vertical (bottom-edge) position
    double y_;
    // Scale
    double scale_;
    // Whether text is italic
    bool italic_;
    // Whether text is bold
    bool bold_;
    // Whether text is actually a symbol
    bool symbol_;

    public:
    // Set vertical (bottom-edge) position
    void setY(double y);
    // Adjust vertical (bottom-edge) position
    void adjustY(double delta);
    // Return vertical (bottom-edge) position
    double y();
    // Set scale
    void setScale(double scale);
    // Return scale
    double scale();
    // Set whether text is italic
    void setItalic(bool italic);
    // Return whether text is italic
    bool italic();
    // Set whether text is bold
    void setBold(bool bold);
    // Return whether text is bold
    bool bold();
    // Set whether text is actually a symbol
    void setSymbol(bool symbol);
    // Return whether text is actually a symbol
    bool symbol();
};
