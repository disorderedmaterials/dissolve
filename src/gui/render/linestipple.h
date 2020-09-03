/*
    *** Line Stipple
    *** src/gui/render/linestipple.h
    Copyright T. Youngs 2013-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
