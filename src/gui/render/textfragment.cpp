// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "gui/render/textfragment.h"

TextFragment::TextFragment() : ListItem<TextFragment>()
{
    scale_ = 1.0;
    italic_ = false;
    bold_ = false;
}

TextFragment::~TextFragment() {}

// Set fragment data
void TextFragment::set(QString &text, double scale, Vec3<double> translation, bool italic, bool bold)
{
    text_ = text;
    scale_ = scale;
    translation_ = translation;
    italic_ = italic;
    bold_ = bold;
}

// Return text of fragment
QString TextFragment::text() { return text_; }

// Return local scale for fragment
double TextFragment::scale() { return scale_; }

// Return local translation for fragment
Vec3<double> TextFragment::translation() { return translation_; }

// Return whether fragment is to be drawn italic
bool TextFragment::italic() { return italic_; }

// Return whether fragment is to be drawn bold
bool TextFragment::bold() { return bold_; }
