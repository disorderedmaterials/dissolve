// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/colourDefinition.h"
#include "base/sysFunc.h"

ColourDefinition::ColourDefinition()
{
    // Colour Definitions
    singleColour_.setRgb(0, 0, 0, 255);
    rgbGradient_.addPoint(0.0, QColor(255, 255, 255));
    rgbGradient_.addPoint(1.0, QColor(0, 0, 255));
    hsvGradient_.setUseHSV(true);
    hsvGradient_.addPoint(0.0, QColor(255, 0, 0));
    hsvGradient_.addPoint(1.0, QColor(100, 40, 255));
    customGradient_.clear();

    // Style
    style_ = ColourDefinition::SingleColourStyle;
    useGlobalAlpha_ = false;
    globalAlpha_ = 0.5;
}

ColourDefinition::~ColourDefinition() {}

ColourDefinition::ColourDefinition(const ColourDefinition &source) { (*this) = source; }

void ColourDefinition::operator=(const ColourDefinition &source)
{
    // Colour Definitions
    style_ = source.style_;
    singleColour_ = source.singleColour_;
    rgbGradient_ = source.rgbGradient_;
    hsvGradient_ = source.hsvGradient_;
    customGradient_ = source.customGradient_;
    useGlobalAlpha_ = source.useGlobalAlpha_;
    globalAlpha_ = source.globalAlpha_;
    version_ = source.version_;
}

/*
 * Enumerations
 */

// Return enum options for ColourStyle
EnumOptions<ColourDefinition::ColourStyle> ColourDefinition::colourStyles()
{
    return EnumOptions<ColourDefinition::ColourStyle>("AutoScaleMethod",
                                                      {{ColourDefinition::SingleColourStyle, "SingleColour"},
                                                       {ColourDefinition::RGBGradientStyle, "RGBGradient"},
                                                       {ColourDefinition::HSVGradientStyle, "HSVGradient"},
                                                       {ColourDefinition::CustomGradientStyle, "CustomGradient"}});
}

/*
 * Style
 */

// Set colour style to use
void ColourDefinition::setStyle(ColourStyle style)
{
    style_ = style;

    ++version_;
}

// Return colour style in use
ColourDefinition::ColourStyle ColourDefinition::style() const { return style_; }

// Set whether to use global alpha value
void ColourDefinition::setUseGlobalAlpha(bool useGlobalAlpha)
{
    useGlobalAlpha_ = useGlobalAlpha;

    ++version_;
}

// Return whether to use global alpha value
bool ColourDefinition::useGlobalAlpha() const { return useGlobalAlpha_; }

// Set global alpha value
void ColourDefinition::setGlobalAlpha(double alpha)
{
    globalAlpha_ = alpha;

    ++version_;
}

// Return global alpha value
double ColourDefinition::globalAlpha() const { return globalAlpha_; }

// Return version
int ColourDefinition::version() const { return version_; }

/*
 * Colour Definitions
 */

// Set single colour
void ColourDefinition::setSingleColour(QColor colour)
{
    singleColour_ = colour;

    ++version_;
}

// Return single colour
QColor ColourDefinition::singleColour() const { return singleColour_; }

/*
 * RGB Gradient Definition
 */

// Set RGB gradient start value
void ColourDefinition::setRGBGradientStartValue(double value)
{
    rgbGradient_.setValue(0, value);

    ++version_;
}

// Return RGB gradient start value
double ColourDefinition::rgbGradientStartValue() const { return rgbGradient_.point(0).value(); }

// Set RGB gradient start colour
void ColourDefinition::setRGBGradientStartColour(QColor colour)
{
    rgbGradient_.setColour(0, colour);

    ++version_;
}

// Return RGB gradient start colour
QColor ColourDefinition::rgbGradientStartColour() const { return rgbGradient_.point(0).colour(); }

// Set RGB gradient start
void ColourDefinition::setRGBGradientStart(double value, QColor colour)
{
    rgbGradient_.setPoint(0, value, colour);

    ++version_;
}

// Set RGB gradient end value
void ColourDefinition::setRGBGradientEndValue(double value)
{
    rgbGradient_.setValue(1, value);

    ++version_;
}

// Return RGB gradient end value
double ColourDefinition::rgbGradientEndValue() const { return rgbGradient_.point(1).value(); }

// Set RGB gradient end colour
void ColourDefinition::setRGBGradientEndColour(QColor colour)
{
    rgbGradient_.setColour(1, colour);

    ++version_;
}

// Return RGB gradient end colour
QColor ColourDefinition::rgbGradientEndColour() const { return rgbGradient_.point(1).colour(); }

// Set RGB gradient end
void ColourDefinition::setRGBGradientEnd(double value, QColor colour)
{
    rgbGradient_.setPoint(1, value, colour);

    ++version_;
}

/*
 * HSV Gradient Definition
 */

// Set HSV gradient start value
void ColourDefinition::setHSVGradientStartValue(double value)
{
    hsvGradient_.setValue(0, value);

    ++version_;
}

// Return HSV gradient start value
double ColourDefinition::hsvGradientStartValue() const { return hsvGradient_.point(0).value(); }

// Set HSV gradient start colour
void ColourDefinition::setHSVGradientStartColour(QColor colour)
{
    hsvGradient_.setColour(0, colour);

    ++version_;
}

// Return HSV gradient start colour
QColor ColourDefinition::hsvGradientStartColour() const { return hsvGradient_.point(0).colour(); }

// Set HSV gradient start
void ColourDefinition::setHSVGradientStart(double value, QColor colour)
{
    hsvGradient_.setPoint(0, value, colour);

    ++version_;
}

// Set HSV gradient end value
void ColourDefinition::setHSVGradientEndValue(double value)
{
    hsvGradient_.setValue(1, value);

    ++version_;
}

// Return HSV gradient end value
double ColourDefinition::hsvGradientEndValue() const { return hsvGradient_.point(1).value(); }

// Set HSV gradient end colour
void ColourDefinition::setHSVGradientEndColour(QColor colour)
{
    hsvGradient_.setColour(1, colour);

    ++version_;
}

// Return HSV gradient end colour
QColor ColourDefinition::hsvGradientEndColour() const { return hsvGradient_.point(1).colour(); }

// Set HSV gradient end
void ColourDefinition::setHSVGradientEnd(double value, QColor colour)
{
    hsvGradient_.setPoint(1, value, colour);

    ++version_;
}

/*
 * Custom Gradient Definition
 */

// Set custom gradient point value and colour
void ColourDefinition::setCustomGradientPoint(int index, double value, QColor colour)
{
    customGradient_.setPoint(index, value, colour);

    ++version_;
}

// Return custom gradient point specified
const ColourScalePoint &ColourDefinition::customGradientPoint(int index) const { return customGradient_.point(index); }

// Return colour of custom gradient point specified
QColor ColourDefinition::customGradientColour(int index) const { return customGradient_.point(index).colour(); }

// Return value of custom gradient point specified
double ColourDefinition::customGradientValue(ColourStyle source, int index) const
{
    return customGradient_.point(index).value();
}

// Append point to custom gradient
void ColourDefinition::appendCustomGradientPoint()
{
    customGradient_.addPoint(customGradient_.nPoints() > 0 ? customGradient_.lastPoint().value() + 1.0 : 0.0, Qt::white);

    ++version_;
}

// Add point to custom gradient
void ColourDefinition::addCustomGradientPoint(double value, QColor colour)
{
    customGradient_.addPoint(value, colour);

    ++version_;
}

// Return number of points in custom gradient
int ColourDefinition::nCustomGradientPoints() const { return customGradient_.nPoints(); }

// Return custom gradient points
const std::vector<ColourScalePoint> &ColourDefinition::customGradientPoints() const { return customGradient_.points(); }

// Remove specified colourscale point with index specified
void ColourDefinition::removeCustomGradientPoint(int id)
{
    customGradient_.removePoint(id);

    ++version_;
}

/*
 * Colour Access
 */

// Return (set) colour for specified value
void ColourDefinition::colour(double value, GLfloat *rgba) const
{
    if (style_ == ColourDefinition::SingleColourStyle)
    {
        rgba[0] = singleColour_.redF();
        rgba[1] = singleColour_.greenF();
        rgba[2] = singleColour_.blueF();
        rgba[3] = useGlobalAlpha_ ? globalAlpha_ : singleColour_.alphaF();
    }
    else if (style_ == ColourDefinition::RGBGradientStyle)
    {
        rgbGradient_.colour(value, rgba);
        if (useGlobalAlpha_)
            rgba[3] = globalAlpha_;
    }
    else if (style_ == ColourDefinition::HSVGradientStyle)
    {
        hsvGradient_.colour(value, rgba);
        if (useGlobalAlpha_)
            rgba[3] = globalAlpha_;
    }
    else
    {
        customGradient_.colour(value, rgba);
        if (useGlobalAlpha_)
            rgba[3] = globalAlpha_;
    }
}
