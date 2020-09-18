/*
    *** Renderable - Data1D
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

#include "gui/render/renderabledata1d.h"
#include "base/lineparser.h"
#include "gui/render/renderablegroupmanager.h"
#include "gui/render/view.h"

RenderableData1D::RenderableData1D(const Data1D *source, std::string_view objectTag)
    : Renderable(Renderable::Data1DRenderable, objectTag), source_(source)
{
    // Set style defaults
    displayStyle_ = LinesStyle;

    // Create primitive
    dataPrimitive_ = createPrimitive();
}

RenderableData1D::~RenderableData1D() {}

/*
 * Data
 */

// Return whether a valid data source is available (attempting to set it if not)
bool RenderableData1D::validateDataSource()
{
    // Don't try to access source_ if we are not currently permitted to do so
    if (!sourceDataAccessEnabled_)
        return false;

    // If there is no valid source set, attempt to set it now...
    if (!source_)
        source_ = Data1D::findObject(objectTag_);

    return source_;
}

// Invalidate the current data source
void RenderableData1D::invalidateDataSource() { source_ = nullptr; }

// Return version of data
int RenderableData1D::dataVersion() { return (validateDataSource() ? source_->version() : -99); }

/*
 * Transform / Limits
 */

// Transform data according to current settings
void RenderableData1D::transformValues()
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
    positiveLimitsMin_.set(-1.0, -1.0, 0.1);
    positiveLimitsMax_.set(-1.0, -1.0, 1.0);
    valuesMin_ = 0.0;
    valuesMax_ = 0.0;
    positiveValuesMin_ = -1.0;
    positiveValuesMax_ = -1.0;

    // Set initial limits if we can
    if (transformedData_.nValues() > 0)
    {
        valuesMin_ = transformedData_.minValue();
        valuesMax_ = transformedData_.maxValue();
        limitsMin_.set(transformedData_.constXAxis().firstValue(), valuesMin_, 0.0);
        limitsMax_.set(transformedData_.constXAxis().lastValue(), valuesMax_, 0.0);
    }

    // Now determine minimum positive limits - loop over points in data, searching for first positive, non-zero value
    for (int n = 0; n < transformedData_.nValues(); ++n)
    {
        // X
        if (transformedData_.constXAxis(n) > 0.0)
        {
            if (positiveLimitsMin_.x < 0.0)
                positiveLimitsMin_.x = transformedData_.constXAxis(n);
            else if (transformedData_.constXAxis(n) < positiveLimitsMin_.x)
                positiveLimitsMin_.x = transformedData_.constXAxis(n);

            if (transformedData_.constXAxis(n) > positiveLimitsMax_.x)
                positiveLimitsMax_.x = transformedData_.constXAxis(n);
        }

        // Value
        if (transformedData_.constValue(n) > 0.0)
        {
            if (positiveValuesMin_ < 0.0)
                positiveValuesMin_ = transformedData_.constValue(n);
            else if (transformedData_.constValue(n) < positiveValuesMin_)
                positiveValuesMin_ = transformedData_.constValue(n);

            if (transformedData_.constValue(n) > positiveValuesMax_)
                positiveValuesMax_ = transformedData_.constValue(n);
        }
    }

    // Check positive value limits (since all datapoints might have been negative)
    if (positiveLimitsMin_.x < 0.0)
    {
        positiveLimitsMin_.x = 0.1;
        positiveLimitsMax_.x = 1.0;
    }
    if (positiveValuesMin_ < 0.0)
    {
        positiveValuesMin_ = 0.1;
        positiveValuesMax_ = 1.0;
    }

    // Copy positive value limits over to y axis
    positiveLimitsMin_.y = positiveValuesMin_;
    positiveLimitsMax_.y = positiveValuesMax_;

    // Update the transformed data 'version'
    valuesTransformDataVersion_ = dataVersion();
}

// Return reference to transformed data
const Data1D &RenderableData1D::transformedData()
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

// Calculate min/max y value over specified x range (if possible in the underlying data)
bool RenderableData1D::yRangeOverX(double xMin, double xMax, double &yMin, double &yMax)
{
    // Grab reference to transformed data
    const auto &data = transformedData();

    auto first = true;
    for (int n = 0; n < data.nValues(); ++n)
    {
        if (data.constXAxis(n) < xMin)
            continue;
        else if (data.constXAxis(n) > xMax)
            break;

        if (first)
        {
            yMin = data.constValue(n);
            yMax = yMin;
            first = false;
        }
        else
        {
            if (data.constValue(n) < yMin)
                yMin = data.constValue(n);
            else if (data.constValue(n) > yMax)
                yMax = data.constValue(n);
        }
    }

    return true;
}

/*
 * Rendering Primitives
 */

// Recreate necessary primitives / primitive assemblies for the data
void RenderableData1D::recreatePrimitives(const View &view, const ColourDefinition &colourDefinition)
{
    dataPrimitive_->initialise(GL_LINE_STRIP, true, 4096);

    constructLineXY(transformedData().constXAxis(), transformedData().constValues(), dataPrimitive_, view.constAxes(),
                    colourDefinition);
}

// Send primitives for rendering
const void RenderableData1D::sendToGL(const double pixelScaling)
{
    // Apply the LineStyle of the Renderable
    lineStyle_.sendToGL(pixelScaling);

    // Disable lighting
    glDisable(GL_LIGHTING);

    dataPrimitive_->sendToGL();

    // Reset LineStyle back to defaults
    LineStyle().sendToGL();
}

// Create line strip primitive
void RenderableData1D::constructLineXY(const Array<double> &displayAbscissa, const Array<double> &displayValues,
                                       Primitive *primitive, const Axes &axes, const ColourDefinition &colourDefinition,
                                       double zCoordinate)
{
    // Copy and transform abscissa values (still in data space) into axes coordinates
    Array<double> x = displayAbscissa;
    axes.transformX(x);
    auto nX = x.nItems();
    if (nX < 2)
        return;

    // Get some values from axes so we can calculate colours properly
    auto yLogarithmic = axes.logarithmic(1);
    double yStretch = axes.stretch(1);

    // Temporary variables
    GLfloat colour[4];
    Vec3<double> nrm(0.0, 1.0, 0.0);

    // Create lines for slices
    int vertexA, vertexB;
    // Grab y and z values
    Array<double> y = displayValues;
    axes.transformY(y);
    double z = axes.transformZ(zCoordinate);

    // Set vertexA to -1 so we don't draw a line at n=0
    vertexA = -1;

    // Check for a single colour style in the colourDefinition - use optimised case in that eventuality
    if (colourDefinition.style() == ColourDefinition::SingleColourStyle)
    {
        // Get the single colour
        colourDefinition.colour(0.0, colour);

        // Loop over x values
        for (int n = 0; n < nX; ++n)
        {
            vertexB = primitive->defineVertex(x.constAt(n), y.constAt(n), z, nrm, colour);

            // If both vertices are valid, plot a line
            if (vertexA != -1)
                primitive->defineIndices(vertexA, vertexB);

            vertexA = vertexB;
        }
    }
    else
    {
        // Loop over x values
        for (int n = 0; n < nX; ++n)
        {
            colourDefinition.colour(yLogarithmic ? pow(10.0, y.constAt(n) / yStretch) : y.constAt(n) / yStretch, colour);
            vertexB = primitive->defineVertex(x.constAt(n), y.constAt(n), z, nrm, colour);

            // If both vertices are valid, plot a line
            if (vertexA != -1)
                primitive->defineIndices(vertexA, vertexB);

            vertexA = vertexB;
        }
    }
}

/*
 * Style
 */

// Return EnumOptions for Data1DDisplayStyle
EnumOptions<RenderableData1D::Data1DDisplayStyle> RenderableData1D::data1DDisplayStyles()
{
    static EnumOptionsList Style1DOptions = EnumOptionsList() << EnumOption(RenderableData1D::LinesStyle, "Lines");

    static EnumOptions<RenderableData1D::Data1DDisplayStyle> options("Data1DDisplayStyle", Style1DOptions);

    return options;
}

// Set display style for renderable
void RenderableData1D::setDisplayStyle(Data1DDisplayStyle displayStyle)
{
    displayStyle_ = displayStyle;

    ++styleVersion_;
}

// Return display style for the renderable
RenderableData1D::Data1DDisplayStyle RenderableData1D::displayStyle() const { return displayStyle_; }

/*
 * Style I/O
 */

// Return enum option info for RenderableKeyword
EnumOptions<RenderableData1D::Data1DStyleKeyword> RenderableData1D::data1DStyleKeywords()
{
    static EnumOptionsList StyleKeywords = EnumOptionsList() << EnumOption(RenderableData1D::DisplayKeyword, "Display", 1)
                                                             << EnumOption(RenderableData1D::EndStyleKeyword, "EndStyle");

    static EnumOptions<RenderableData1D::Data1DStyleKeyword> options("Data1DStyleKeyword", StyleKeywords);

    return options;
}

// Write style information
bool RenderableData1D::writeStyleBlock(LineParser &parser, int indentLevel) const
{
    // Construct indent string
    char *indent = new char[indentLevel * 2 + 1];
    for (int n = 0; n < indentLevel * 2; ++n)
        indent[n] = ' ';
    indent[indentLevel * 2] = '\0';

    if (!parser.writeLineF("{}{}  {}\n", indent, data1DStyleKeywords().keyword(RenderableData1D::DisplayKeyword),
                           data1DDisplayStyles().keyword(displayStyle_)))
        return false;

    return true;
}

// Read style information
bool RenderableData1D::readStyleBlock(LineParser &parser)
{
    while (!parser.eofOrBlank())
    {
        // Get line from file
        if (parser.getArgsDelim(LineParser::SemiColonLineBreaks) != LineParser::Success)
            return false;

        // Do we recognise this keyword and, if so, do we have an appropriate number of arguments?
        if (!data1DStyleKeywords().isValid(parser.argsv(0)))
            return data1DStyleKeywords().errorAndPrintValid(parser.argsv(0));
        auto kwd = data1DStyleKeywords().enumeration(parser.argsv(0));
        if (!data1DStyleKeywords().validNArgs(kwd, parser.nArgs() - 1))
            return false;

        // All OK, so process the keyword
        switch (kwd)
        {
            // Display style
            case (RenderableData1D::DisplayKeyword):
                if (!data1DDisplayStyles().isValid(parser.argsv(1)))
                    return data1DDisplayStyles().errorAndPrintValid(parser.argsv(1));
                displayStyle_ = data1DDisplayStyles().enumeration(parser.argsv(1));
                break;
            // End of block
            case (RenderableData1D::EndStyleKeyword):
                return true;
            // Unrecognised Keyword
            default:
                Messenger::warn("Unrecognised display style keyword for RenderableData1D: {}\n", parser.argsv(0));
                return false;
                break;
        }
    }

    return true;
}
