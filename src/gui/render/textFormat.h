// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

// Text Formatting
class TextFormat
{
    public:
    TextFormat();
    ~TextFormat() = default;
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
    bool isItalic_;
    // Whether text is bold
    bool isBold_;
    // Whether text is actually a symbol
    bool isSymbol_;

    public:
    // Set vertical (bottom-edge) position
    void setY(double y);
    // Adjust vertical (bottom-edge) position
    void adjustY(double delta);
    // Return vertical (bottom-edge) position
    double y() const;
    // Set scale
    void setScale(double scale);
    // Return scale
    double scale() const;
    // Set whether text is italic
    void setItalic(bool italic);
    // Return whether text is italic
    bool isItalic() const;
    // Set whether text is bold
    void setBold(bool bold);
    // Return whether text is bold
    bool isBold() const;
    // Set whether text is actually a symbol
    void setSymbol(bool symbol);
    // Return whether text is actually a symbol
    bool isSymbol() const;
};
