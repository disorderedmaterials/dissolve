// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include <QMargins>

// Forward Declarations
/* none */

// Procedure Chart Metrics - Style / Layout Information
class ProcedureChartMetrics
{
    /*
     * Module Chart Block
     */
    public:
    // Border line width
    static int blockBorderWidth() { return 2; }
    // Border mid-point
    static double blockBorderMidPoint() { return (blockBorderWidth() / 2.0); }
    // Widget margins (to contain border line)
    static QMargins blockMargins()
    {
        int innerGap = 4;
        return QMargins(blockBorderWidth() + innerGap + dragHandleWidth(), blockBorderWidth() + innerGap,
                        blockBorderWidth() + innerGap, blockBorderWidth() + innerGap);
    }
    // Vertical spacing between blocks
    static int blockVerticalSpacing() { return 2; }
    // Indent width
    static int indentWidth() { return 30; }
    // Drag handle width
    static int dragHandleWidth() { return 10; }

    /*
     * Procedure Chart
     */
    public:
    // Inner margin around widgets
    static int chartMargin() { return 0; }
    // Spacing between rows
    static int chartRowSpacing() { return 0; }
};
