// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/textFormat.h"

TextFormat::TextFormat() : y_(0.0), scale_(1.0), isItalic_(false), isBold_(false), isSymbol_(false) {}

TextFormat::TextFormat(const TextFormat &source) { (*this) = source; }

void TextFormat::operator=(const TextFormat &source)
{
    y_ = source.y_;
    scale_ = source.scale_;
    isItalic_ = source.isItalic_;
    isBold_ = source.isBold_;
    isSymbol_ = source.isSymbol_;
}

// Set vertical (bottom-edge) position
void TextFormat::setY(double y) { y_ = y; }

// Adjust vertical (bottom-edge) position
void TextFormat::adjustY(double delta) { y_ += delta; }

// Return vertical (bottom-edge) position
double TextFormat::y() const { return y_; }

// Set scale
void TextFormat::setScale(double scale) { scale_ = scale; }

// Return scale
double TextFormat::scale() const { return scale_; }

// Set whether text is italic
void TextFormat::setItalic(bool italic) { isItalic_ = italic; }

// Return whether text is italic
bool TextFormat::isItalic() const { return isItalic_; }

// Set whether text is bold
void TextFormat::setBold(bool bold) { isBold_ = bold; }

// Return whether text is bold
bool TextFormat::isBold() const { return isBold_; }

// Set whether text is actually a symbol
void TextFormat::setSymbol(bool symbol) { isSymbol_ = symbol; }

// Return whether text is actually a symbol
bool TextFormat::isSymbol() const { return isSymbol_; }
