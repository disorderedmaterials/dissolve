// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/renderableData1D.h"
#include "base/lineParser.h"
#include "gui/render/renderableGroupManager.h"
#include "gui/render/view.h"
#include "items/list.h"
#include "math/sampledData1D.h"

RenderableData1D::RenderableData1D(const Data1DBase &source)
    : Renderable(Renderable::Data1DRenderable, ""), source_(source), displayStyle_(LinesStyle)
{
    dataPrimitive_ = createPrimitive();
}

RenderableData1D::RenderableData1D(std::string_view tag)
    : Renderable(Renderable::Data1DRenderable, tag), displayStyle_(LinesStyle)
{
    dataPrimitive_ = createPrimitive();
}

/*
 * Data
 */

// Return source data
OptionalReferenceWrapper<const Data1DBase> RenderableData1D::source() const { return source_; }

// Attempt to set the data source, searching the supplied list for the object
void RenderableData1D::validateDataSource(const GenericList &sourceList)
{
    // Don't try to access source_ if we are not currently permitted to do so
    if (!sourceDataAccessEnabled_)
        return;

    if (source_)
        return;

    source_ = sourceList.searchBase<Data1DBase, Data1D, SampledData1D>(tag_);
}

// Invalidate the current data source
void RenderableData1D::invalidateDataSource() { source_ = std::nullopt; }

// Return version of data
int RenderableData1D::dataVersion() { return (source_ ? source_->get().version() : -99); }

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
        transformedData_ = source_->get();
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
        limitsMin_.set(transformedData_.xAxis().front(), valuesMin_, 0.0);
        limitsMax_.set(transformedData_.xAxis().back(), valuesMax_, 0.0);
    }

    // Now determine minimum positive limits - loop over points in data, searching for first positive, non-zero value
    for (auto n = 0; n < transformedData_.nValues(); ++n)
    {
        // X
        if (transformedData_.xAxis(n) > 0.0)
        {
            if (positiveLimitsMin_.x < 0.0)
                positiveLimitsMin_.x = transformedData_.xAxis(n);
            else if (transformedData_.xAxis(n) < positiveLimitsMin_.x)
                positiveLimitsMin_.x = transformedData_.xAxis(n);

            if (transformedData_.xAxis(n) > positiveLimitsMax_.x)
                positiveLimitsMax_.x = transformedData_.xAxis(n);
        }

        // Value
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
    if (!source_)
        return transformedData_;

    // If the value transform is not enabled, just return the original data
    if (!valuesTransform_.enabled())
        transformedData_ = source_->get();
    else
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
    for (auto n = 0; n < data.nValues(); ++n)
    {
        if (data.xAxis(n) < xMin)
            continue;
        else if (data.xAxis(n) > xMax)
            break;

        if (first)
        {
            yMin = data.value(n);
            yMax = yMin;
            first = false;
        }
        else
        {
            if (data.value(n) < yMin)
                yMin = data.value(n);
            else if (data.value(n) > yMax)
                yMax = data.value(n);
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

    constructLineXY(transformedData().xAxis(), transformedData().values(), dataPrimitive_, view.axes(), colourDefinition);
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
void RenderableData1D::constructLineXY(const std::vector<double> &displayAbscissa, const std::vector<double> &displayValues,
                                       Primitive *primitive, const Axes &axes, const ColourDefinition &colourDefinition,
                                       double zCoordinate)
{
    // Copy and transform abscissa values (still in data space) into axes coordinates
    std::vector<double> x = displayAbscissa;
    axes.transformX(x);
    auto nX = x.size();
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
    std::vector<double> y = displayValues;
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
        for (auto n = 0; n < nX; ++n)
        {
            vertexB = primitive->defineVertex(x[n], y[n], z, nrm, colour);

            // If both vertices are valid, plot a line
            if (vertexA != -1)
                primitive->defineIndices(vertexA, vertexB);

            vertexA = vertexB;
        }
    }
    else
    {
        // Loop over x values
        for (auto n = 0; n < nX; ++n)
        {
            colourDefinition.colour(yLogarithmic ? pow(10.0, y[n] / yStretch) : y[n] / yStretch, colour);
            vertexB = primitive->defineVertex(x[n], y[n], z, nrm, colour);

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
    return EnumOptions<RenderableData1D::Data1DDisplayStyle>("Data1DDisplayStyle", {{RenderableData1D::LinesStyle, "Lines"}});
}

// Set display style for renderable
void RenderableData1D::setDisplayStyle(Data1DDisplayStyle displayStyle)
{
    displayStyle_ = displayStyle;

    ++styleVersion_;
}

// Return display style for the renderable
RenderableData1D::Data1DDisplayStyle RenderableData1D::displayStyle() const { return displayStyle_; }
