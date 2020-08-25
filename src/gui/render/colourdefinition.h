/*
    *** Colour Definition
    *** src/gui/render/colourdefinition.h
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

#include "base/enumoptions.h"
#include "base/version.h"
#include "gui/render/colourscale.h"

// Forward Declarations
/* none */

// Colour Definition
class ColourDefinition
{
    public:
    ColourDefinition();
    ~ColourDefinition();
    ColourDefinition(const ColourDefinition &source);
    void operator=(const ColourDefinition &source);

    /*
     * Enumerations
     */
    public:
    // Colour Styles
    enum ColourStyle
    {
        SingleColourStyle,
        RGBGradientStyle,
        HSVGradientStyle,
        CustomGradientStyle,
        nColourStyles
    };
    // Return enum options for ColourStyle
    static EnumOptions<ColourDefinition::ColourStyle> &colourStyles();

    /*
     * Style
     */
    private:
    // Current colour style in use
    ColourStyle style_;
    // Whether to use global alpha value
    bool useGlobalAlpha_;
    // Global alpha value
    double globalAlpha_;
    // Version
    VersionCounter version_;

    public:
    // Set colour style to use
    void setStyle(ColourStyle source);
    // Return colour style in use
    ColourStyle style() const;
    // Set whether to use global alpha value
    void setUseGlobalAlpha(bool useGlobalAlpha);
    // Return whether to use global alpha value
    bool useGlobalAlpha() const;
    // Set global alpha value
    void setGlobalAlpha(double alpha);
    // Return global alpha value
    double globalAlpha() const;
    // Return version
    int version() const;

    /*
     * Single Colour Definition
     */
    private:
    // Single colour
    QColor singleColour_;

    public:
    // Set single colour
    void setSingleColour(QColor colour);
    // Return single colour
    QColor singleColour() const;

    /*
     * RGB Gradient Definition
     */
    private:
    // RGB Gradient
    ColourScale rgbGradient_;

    public:
    // Set RGB gradient start value
    void setRGBGradientStartValue(double value);
    // Return RGB gradient start value
    double rgbGradientStartValue() const;
    // Set RGB gradient start colour
    void setRGBGradientStartColour(QColor colour);
    // Return RGB gradient start colour
    QColor rgbGradientStartColour() const;
    // Set RGB gradient start
    void setRGBGradientStart(double value, QColor colour);
    // Set RGB gradient end value
    void setRGBGradientEndValue(double value);
    // Return RGB gradient end value
    double rgbGradientEndValue() const;
    // Set RGB gradient end colour
    void setRGBGradientEndColour(QColor colour);
    // Return RGB gradient end colour
    QColor rgbGradientEndColour() const;
    // Set RGB gradient end
    void setRGBGradientEnd(double value, QColor colour);

    /*
     * HSV Gradient Definition
     */
    private:
    // HSV gradient
    ColourScale hsvGradient_;

    public:
    // Set HSV gradient start value
    void setHSVGradientStartValue(double value);
    // Return HSV gradient start value
    double hsvGradientStartValue() const;
    // Set HSV gradient start colour
    void setHSVGradientStartColour(QColor colour);
    // Return HSV gradient start colour
    QColor hsvGradientStartColour() const;
    // Set HSV gradient start
    void setHSVGradientStart(double value, QColor colour);
    // Set HSV gradient end value
    void setHSVGradientEndValue(double value);
    // Return HSV gradient end value
    double hsvGradientEndValue() const;
    // Set HSV gradient end colour
    void setHSVGradientEndColour(QColor colour);
    // Return HSV gradient end colour
    QColor hsvGradientEndColour() const;
    // Set HSV gradient end
    void setHSVGradientEnd(double value, QColor colour);

    /*
     * Custom Gradient Definition
     */
    private:
    // Custom gradient
    ColourScale customGradient_;

    public:
    // Set custom gradient point value and colour
    void setCustomGradientPoint(int index, double value, QColor colour);
    // Return custom gradient point specified
    const ColourScalePoint &customGradientPoint(int index) const;
    // Return colour of custom gradient point specified
    QColor customGradientColour(int index) const;
    // Return value of custom gradient point specified
    double customGradientValue(ColourStyle source, int index) const;
    // Append point to custom gradient
    void appendCustomGradientPoint();
    // Add point to custom gradient
    void addCustomGradientPoint(double value, QColor colour);
    // Return number of points in custom gradient
    int nCustomGradientPoints() const;
    // Return custom gradient points
    const Array<ColourScalePoint> &customGradientPoints() const;
    // Remove specified colourscale point with index specified
    void removeCustomGradientPoint(int id);

    /*
     * Colour Access
     */
    public:
    // Return (set) colour for specified value
    void colour(double value, GLfloat *rgba) const;
};
