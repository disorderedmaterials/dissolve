// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QVector>
#include <QtGui/qopengl.h>

// Forward Declarations
class QComboBox;

// Line Stipple
class LineStipple
{
    public:
    // Line Stipple Types
    enum StippleType
    {
        NoStipple,
        DotStipple,
        FineDashStipple,
        EighthDashStipple,
        QuarterDashStipple,
        HalfDashStipple,
        DotDash1Stipple,
        nStippleTypes
    };
    // Convert text string to StippleType
    static LineStipple::StippleType stippleType(std::string_view s);
    // Convert InputBlock to text string
    static std::string_view stippleType(LineStipple::StippleType stipple);

    /*
     * Stipple
     */
    public:
    // Line stipple factor
    GLint stippleFactor;
    // Line stipple pattern
    GLushort stipplePattern;
    // Name of stipple
    std::string name;

    public:
    // Add stipple pattern to specified QComboBox
    void addStippleItem(QComboBox *combo, int lineHeight);
    // Return stipple pattern as a Qt-compatible dash pattern
    QVector<qreal> &dashPattern();

    /*
     * GL
     */
    public:
    // Apply stipple pattern
    void apply();

    /*
     * Singleton
     */
    public:
    // Static list of line stipples
    static LineStipple stipple[];
};
