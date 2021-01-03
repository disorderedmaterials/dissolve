// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

// Forward Declarations
/* none */

// ModuleList Chart Metrics - Style / Layout Information
class ModuleListChartMetrics
{
    /*
     * Module Chart Block
     */
    public:
    // Border line width
    static int blockBorderWidth() { return 2; }
    // Border external radius
    static int blockBorderRadius() { return 6; }
    // Border mid-point
    static double blockBorderMidPoint() { return blockBorderWidth() / 2; }
    // Widget margins (to contain border line)
    static QMargins blockMargins()
    {
        int innerGap = 4;
        return QMargins(blockBorderWidth() + innerGap + blockDentRadius(), blockBorderWidth() + innerGap,
                        blockBorderWidth() + innerGap, blockBorderWidth() + innerGap);
    }
    // Vertical offset at which dent begins on left-hand side
    static int blockDentOffset() { return 16; }
    // Radius of the dent
    static int blockDentRadius() { return 9; }

    /*
     * Module Chart
     */
    public:
    // Inner margin around widgets
    static int chartMargin() { return 16; }
    // Horizontal spacing between modules in list
    static int verticalModuleSpacing() { return 16; }
    // Additional horizontal spacing for insertion widget
    static int verticalInsertionSpacing() { return 64; }
};
