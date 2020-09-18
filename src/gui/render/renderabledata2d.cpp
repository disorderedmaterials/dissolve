/*
    *** Renderable - Data2D
    *** src/gui/render/renderabledata1d.cpp
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

#include "gui/render/renderabledata2d.h"
#include "base/lineparser.h"
#include "gui/render/renderablegroupmanager.h"
#include "gui/render/view.h"
#include "math/data2d.h"
#include "math/extrema.h"
#include "templates/array2d.h"

RenderableData2D::RenderableData2D(const Data2D *source, std::string_view objectTag)
    : Renderable(Renderable::Data2DRenderable, objectTag), source_(source)
{
    // Set defaults
    displayStyle_ = LinesStyle;
    colour().setStyle(ColourDefinition::HSVGradientStyle);
}

RenderableData2D::~RenderableData2D() {}

/*
 * Data
 */

// Return whether a valid data source is available (attempting to set it if not)
bool RenderableData2D::validateDataSource()
{
    // Don't try to access source_ if we are not currently permitted to do so
    if (!sourceDataAccessEnabled_)
        return false;

    // If there is no valid source set, attempt to set it now...
    if (!source_)
        source_ = Data2D::findObject(objectTag_);

    return source_;
}

// Invalidate the current data source
void RenderableData2D::invalidateDataSource() { source_ = nullptr; }

// Return version of data
int RenderableData2D::dataVersion() { return (validateDataSource() ? source_->version() : -99); }

/*
 * Transform / Limits
 */

// Transform data according to current settings
void RenderableData2D::transformValues()
{
    // If the transformed data are already up-to-date, no need to do anything
    if (valuesTransformDataVersion_ == dataVersion())
        return;

    // Copy original data and transform now. We do this even if the transformers are disabled, since they may have
    // previously been active
    if (!source_)
        transformedData_.clear();
    else
        transformedData_ = *source_;
    valuesTransform_.transformValues(transformedData_);

    limitsMin_ = 0.0;
    limitsMax_ = 0.0;
    positiveLimitsMin_ = -1.0;
    positiveLimitsMax_ = -1.0;
    valuesMin_ = 0.0;
    valuesMax_ = 0.0;
    positiveValuesMin_ = -1.0;
    positiveValuesMax_ = -1.0;

    // Set initial limits if we can
    if (transformedData_.nValues() > 0)
    {
        valuesMin_ = transformedData_.minValue();
        valuesMax_ = transformedData_.maxValue();
        limitsMin_.set(transformedData_.constXAxis().firstValue(), transformedData_.constYAxis().firstValue(), valuesMin_);
        limitsMax_.set(transformedData_.constXAxis().lastValue(), transformedData_.constYAxis().lastValue(), valuesMax_);
    }

    // Now determine minimum positive limits - loop over points in data, searching for first positive, non-zero value
    // X
    for (int n = 0; n < transformedData_.constXAxis().nItems(); ++n)
    {
        if (transformedData_.constXAxis(n) > 0.0)
        {
            if (positiveLimitsMin_.x < 0.0)
                positiveLimitsMin_.x = transformedData_.constXAxis(n);
            else if (transformedData_.constXAxis(n) < positiveLimitsMin_.x)
                positiveLimitsMin_.x = transformedData_.constXAxis(n);

            if (transformedData_.constXAxis(n) > positiveLimitsMax_.x)
                positiveLimitsMax_.x = transformedData_.constXAxis(n);
        }
    }

    // Y
    for (int n = 0; n < transformedData_.constYAxis().nItems(); ++n)
    {
        if (transformedData_.constYAxis(n) > 0.0)
        {
            if (positiveLimitsMin_.y < 0.0)
                positiveLimitsMin_.y = transformedData_.constYAxis(n);
            else if (transformedData_.constYAxis(n) < positiveLimitsMin_.y)
                positiveLimitsMin_.y = transformedData_.constYAxis(n);

            if (transformedData_.constYAxis(n) > positiveLimitsMax_.y)
                positiveLimitsMax_.y = transformedData_.constYAxis(n);
        }
    }

    // Values
    for (int n = 0; n < transformedData_.nValues(); ++n)
    {
        if (transformedData_.value(n) > 0.0)
        {
            if (positiveValuesMin_ < 0.0)
                positiveValuesMin_ = transformedData_.value(n);
            else if (transformedData_.value(n) < positiveValuesMin_)
                positiveValuesMin_ = transformedData_.value(n);

            if (transformedData_.value(n) > positiveValuesMax_)
                positiveValuesMax_ = transformedData_.value(n);
        }
    }

    // Copy positive value limits over to z axis
    positiveLimitsMin_.z = positiveValuesMin_;
    positiveLimitsMax_.z = positiveValuesMax_;

    // Update the transformed data 'version'
    valuesTransformDataVersion_ = dataVersion();
}

// Return reference to transformed data
const Data2D &RenderableData2D::transformedData()
{
    // Check that we have a valid source
    if (!validateDataSource())
        return transformedData_;

    // If the value transform is not enabled, just return the original data
    if (!valuesTransform_.enabled())
        return *source_;

    // Make sure the transformed data is up-to-date
    transformValues();

    return transformedData_;
}

/*
 * Rendering Primitives
 */

// Recreate necessary primitives / primitive assemblies for the data
void RenderableData2D::recreatePrimitives(const View &view, const ColourDefinition &colourDefinition)
{
    if (!validateDataSource())
    {
        if (!source_)
            reinitialisePrimitives(0, GL_LINE_STRIP, true);
        return;
    }

    reinitialisePrimitives(source_->constYAxis().nItems(), GL_LINE_STRIP, true);
    constructLine(transformedData().constXAxis(), transformedData().constYAxis(), transformedData().constValues2D(),
                  view.constAxes(), colourDefinition);
}

// Send primitives for rendering
const void RenderableData2D::sendToGL(const double pixelScaling)
{
    // Apply the LineStyle of the Renderable
    lineStyle_.sendToGL(pixelScaling);

    // Disable lighting
    glDisable(GL_LIGHTING);

    for (int n = 0; n < nPrimitives(); ++n)
        primitive(n)->sendToGL();

    // Reset LineStyle back to defaults
    LineStyle().sendToGL();
}

// Create line strip primitive
void RenderableData2D::constructLine(const Array<double> &displayXAbscissa, const Array<double> &displayYAbscissa,
                                     const Array2D<double> &displayValues, const Axes &axes,
                                     const ColourDefinition &colourDefinition)
{
    // Copy and transform abscissa values (still in data space) into axes coordinates
    Array<double> x = displayXAbscissa;
    axes.transformX(x);
    auto nX = x.nItems();
    if (nX < 2)
        return;

    // Copy and transform abscissa values (still in data space) into axes coordinates
    Array<double> y = displayYAbscissa;
    axes.transformY(y);
    auto nY = y.nItems();
    if (nY < 2)
        return;

    // Get some values from axes so we can calculate colours properly
    auto vLogarithmic = axes.logarithmic(2);
    // double vStretch = axes.stretch(2);

    // Temporary variables
    GLfloat colour[4];
    Vec3<double> nrm(0.0, 1.0, 0.0);

    // Create lines for slices
    int vertexA, vertexB;
    Array2D<double> v = displayValues;
    axes.transformZ(v);

    Primitive *p;

    // Check for a single colour style in the colourDefinition - use optimised case in that eventuality
    if (colourDefinition.style() == ColourDefinition::SingleColourStyle)
    {
        // Get the single colour
        colourDefinition.colour(0.0, colour);

        // Loop over y
        for (int n = 0; n < nY; ++n)
        {
            // Set vertexA to -1 so we don't draw a line at n=0
            vertexA = -1;
            p = primitive(n);
            // Loop over x
            for (int m = 0; m < nX; ++m)
            {
                vertexB = p->defineVertex(x.constAt(m), y.constAt(n), v.constAt(m, n), nrm, colour);

                // If both vertices are valid, plot a line
                if (vertexA != -1)
                    p->defineIndices(vertexA, vertexB);
                vertexA = vertexB;
            }
        }
    }
    else
    {
        ColourDefinition colourDef = colourDefinition;
        // Setting gradient start and end value based on minimum and maximum data points
        colourDef.setHSVGradientStartValue(positiveValuesMin_);
        colourDef.setHSVGradientEndValue(positiveValuesMax_);

        // Loop over y
        for (int n = 0; n < nY; ++n)
        {
            // Set vertexA to -1 so we don't draw a line at n=0
            vertexA = -1;
            p = primitive(n);

            // Loop over x
            for (int m = 0; m < nX; ++m)
            {
                // Assigning colour based on value
                double c = (vLogarithmic ? pow(displayValues.constAt(m, n), 10.0) : displayValues.constAt(m, n));
                colourDef.colour(c, colour);
                vertexB = p->defineVertex(x.constAt(m), y.constAt(n), v.constAt(m, n), nrm, colour);

                // If both vertices are valid, plot a line
                if (vertexA != -1)
                    p->defineIndices(vertexA, vertexB);
                vertexA = vertexB;
            }
        }
    }
}

/*
 * Style
 */

// Return EnumOptions for Data2DDisplayStyle
EnumOptions<RenderableData2D::Data2DDisplayStyle> RenderableData2D::data2DDisplayStyles()
{
    static EnumOptionsList Style2DOptions = EnumOptionsList() << EnumOption(RenderableData2D::LinesStyle, "Lines");

    static EnumOptions<RenderableData2D::Data2DDisplayStyle> options("Data2DDisplayStyle", Style2DOptions);

    return options;
}

// Set display style for renderable
void RenderableData2D::setDisplayStyle(Data2DDisplayStyle displayStyle)
{
    displayStyle_ = displayStyle;

    ++styleVersion_;
}

// Return display style for the renderable
RenderableData2D::Data2DDisplayStyle RenderableData2D::displayStyle() const { return displayStyle_; }

/*
 * Style I/O
 */

// Return enum option info for RenderableKeyword
EnumOptions<RenderableData2D::Data2DStyleKeyword> RenderableData2D::data2DStyleKeywords()
{
    static EnumOptionsList StyleKeywords = EnumOptionsList() << EnumOption(RenderableData2D::DisplayKeyword, "Display", 1)
                                                             << EnumOption(RenderableData2D::EndStyleKeyword, "EndStyle");

    static EnumOptions<RenderableData2D::Data2DStyleKeyword> options("Data2DStyleKeyword", StyleKeywords);

    return options;
}

// Write style information
bool RenderableData2D::writeStyleBlock(LineParser &parser, int indentLevel) const
{
    // Construct indent string
    char *indent = new char[indentLevel * 2 + 1];
    for (int n = 0; n < indentLevel * 2; ++n)
        indent[n] = ' ';
    indent[indentLevel * 2] = '\0';

    if (!parser.writeLineF("{}{}  {}\n", indent, data2DStyleKeywords().keyword(RenderableData2D::DisplayKeyword),
                           data2DDisplayStyles().keyword(displayStyle_)))
        return false;

    return true;
}

// Read style information
bool RenderableData2D::readStyleBlock(LineParser &parser)
{
    while (!parser.eofOrBlank())
    {
        // Get line from file
        if (parser.getArgsDelim(LineParser::SemiColonLineBreaks) != LineParser::Success)
            return false;

        // Do we recognise this keyword and, if so, do we have an appropriate number of arguments?
        if (!data2DStyleKeywords().isValid(parser.argsv(0)))
            return data2DStyleKeywords().errorAndPrintValid(parser.argsv(0));
        auto kwd = data2DStyleKeywords().enumeration(parser.argsv(0));
        if (!data2DStyleKeywords().validNArgs(kwd, parser.nArgs() - 1))
            return false;

        // All OK, so process the keyword
        switch (kwd)
        {
            // Display style
            case (RenderableData2D::DisplayKeyword):
                if (!data2DDisplayStyles().isValid(parser.argsv(1)))
                    return data2DDisplayStyles().errorAndPrintValid(parser.argsv(1));
                displayStyle_ = data2DDisplayStyles().enumeration(parser.argsv(1));
                break;
            // End of block
            case (RenderableData2D::EndStyleKeyword):
                return true;
            // Unrecognised Keyword
            default:
                Messenger::warn("Unrecognised display style keyword for RenderableData2D: {}\n", parser.argsv(0));
                return false;
                break;
        }
    }

    return true;
}
