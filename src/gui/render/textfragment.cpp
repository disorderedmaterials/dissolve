// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/render/textfragment.h"

TextFragment::TextFragment(QString text, double scale, Vec3<double> translation, bool isItalic, bool isBold)
    : text_(text), scale_(scale), translation_(translation), isItalic_(isItalic), isBold_(isBold)
{
}

/*
 * Data
 */

// Return text of fragment
const QString &TextFragment::text() const { return text_; }

// Return local scale for fragment
double TextFragment::scale() const { return scale_; }

// Return local translation for fragment
Vec3<double> TextFragment::translation() const { return translation_; }

// Return whether fragment is to be drawn italic
bool TextFragment::isItalic() const { return isItalic_; }

// Return whether fragment is to be drawn bold
bool TextFragment::isBold() const { return isBold_; }
