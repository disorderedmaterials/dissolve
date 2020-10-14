// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "templates/list.h"
#include "templates/vector3.h"
#include <QString>

// Forward Declarations
/* none */

// Text Fragment
class TextFragment : public ListItem<TextFragment>
{
    public:
    TextFragment();
    ~TextFragment();

    /*
     * Fragment Definition
     */
    private:
    // Fragment text
    QString text_;
    // Local scale for fragment
    double scale_;
    // Local translation for fragment
    Vec3<double> translation_;
    // Whether fragment is to be drawn italic
    bool italic_;
    // Whether fragment is to be drawn bold
    bool bold_;

    public:
    // Set fragment data
    void set(QString &text, double scale = 1.0, Vec3<double> translation = Vec3<double>(), bool italic = false,
             bool bold = false);
    // Return fragment text
    QString text();
    // Return local scale for fragment
    double scale();
    // Return local translation for fragment
    Vec3<double> translation();
    // Return whether fragment is to be drawn italic
    bool italic();
    // Return whether fragment is to be drawn bold
    bool bold();
};
