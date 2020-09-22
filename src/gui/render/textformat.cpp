// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "gui/render/textformat.h"

TextFormat::TextFormat() : ListItem<TextFormat>()
{
    y_ = 0.0;
    scale_ = 1.0;
    italic_ = false;
    bold_ = false;
    symbol_ = false;
}

// Desctructor
TextFormat::~TextFormat() {}

TextFormat::TextFormat(const TextFormat &source) { (*this) = source; }

void TextFormat::operator=(const TextFormat &source)
{
    y_ = source.y_;
    scale_ = source.scale_;
    italic_ = source.italic_;
    bold_ = source.bold_;
    symbol_ = source.symbol_;
}

// Set vertical (bottom-edge) position
void TextFormat::setY(double y) { y_ = y; }

// Adjust vertical (bottom-edge) position
void TextFormat::adjustY(double delta) { y_ += delta; }

// Return vertical (bottom-edge) position
double TextFormat::y() { return y_; }

// Set scale
void TextFormat::setScale(double scale) { scale_ = scale; }

// Return scale
double TextFormat::scale() { return scale_; }
// Set whether text is italic
void TextFormat::setItalic(bool italic) { italic_ = italic; }

// Return whether text is italic
bool TextFormat::italic() { return italic_; }

// Set whether text is bold
void TextFormat::setBold(bool bold) { bold_ = bold; }

// Return whether text is bold
bool TextFormat::bold() { return bold_; }

// Set whether text is actually a symbol
void TextFormat::setSymbol(bool symbol) { symbol_ = symbol; }

// Return whether text is actually a symbol
bool TextFormat::symbol() { return symbol_; }
