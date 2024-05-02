// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "templates/vector3.h"
#include <string>

// Text Fragment
class TextFragment
{
    public:
    TextFragment(std::string_view text, double scale = 1.0, Vec3<double> translation = Vec3<double>(), bool isItalic = false,
                 bool isBold = false);
    ~TextFragment() = default;

    /*
     * Data
     */
    private:
    // Fragment text
    std::string text_;
    // Local scale for fragment
    double scale_;
    // Local translation for fragment
    Vec3<double> translation_;
    // Whether fragment is to be drawn italic
    bool isItalic_;
    // Whether fragment is to be drawn bold
    bool isBold_;

    public:
    // Return fragment text
    const std::string &text() const;
    // Return local scale for fragment
    double scale() const;
    // Return local translation for fragment
    Vec3<double> translation() const;
    // Return whether fragment is to be drawn italic
    bool isItalic() const;
    // Return whether fragment is to be drawn bold
    bool isBold() const;
};
