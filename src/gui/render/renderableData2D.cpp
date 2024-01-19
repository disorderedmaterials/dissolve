// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/renderableData2D.h"
#include "base/lineParser.h"
#include "gui/render/renderableGroupManager.h"
#include "gui/render/view.h"
#include "items/list.h"
#include "math/data2D.h"
#include "templates/array2D.h"

RenderableData2D::RenderableData2D(const Data2D &source)
    : Renderable(Renderable::Data2DRenderable, ""), source_(source), displayStyle_(LinesStyle)
{
    colour().setStyle(ColourDefinition::HSVGradientStyle);
}

RenderableData2D::RenderableData2D(std::string_view tag)
    : Renderable(Renderable::Data2DRenderable, tag), displayStyle_(LinesStyle)
{
    colour().setStyle(ColourDefinition::HSVGradientStyle);
}

/*
 * Data
 */

// Return source data
OptionalReferenceWrapper<const Data2D> RenderableData2D::source() const { return source_; }

// Attempt to set the data source, searching the supplied list for the object
void RenderableData2D::validateDataSource(const GenericList &sourceList)
{
    // Don't try to access source_ if we are not currently permitted to do so
    if (!sourceDataAccessEnabled_)
        return;

    if (source_)
        return;

    source_ = sourceList.search<const Data2D>(tag_);
}

// Invalidate the current data source
void RenderableData2D::invalidateDataSource() { source_ = std::nullopt; }

// Return version of data
int RenderableData2D::dataVersion() { return (source_ ? source_->get().version() : -99); }

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
        limitsMin_.set(transformedData_.xAxis().front(), transformedData_.yAxis().front(), valuesMin_);
        limitsMax_.set(transformedData_.xAxis().back(), transformedData_.yAxis().back(), valuesMax_);
    }

    // Now determine minimum positive limits - loop over points in data, searching for first positive, non-zero value
    // X
    for (auto n = 0; n < transformedData_.xAxis().size(); ++n)
    {
        if (transformedData_.xAxis(n) > 0.0)
        {
            if (positiveLimitsMin_.x < 0.0)
                positiveLimitsMin_.x = transformedData_.xAxis(n);
            else if (transformedData_.xAxis(n) < positiveLimitsMin_.x)
                positiveLimitsMin_.x = transformedData_.xAxis(n);

            if (transformedData_.xAxis(n) > positiveLimitsMax_.x)
                positiveLimitsMax_.x = transformedData_.xAxis(n);
        }
    }

    // Y
    for (auto n = 0; n < transformedData_.yAxis().size(); ++n)
    {
        if (transformedData_.yAxis(n) > 0.0)
        {
            if (positiveLimitsMin_.y < 0.0)
                positiveLimitsMin_.y = transformedData_.yAxis(n);
            else if (transformedData_.yAxis(n) < positiveLimitsMin_.y)
                positiveLimitsMin_.y = transformedData_.yAxis(n);

            if (transformedData_.yAxis(n) > positiveLimitsMax_.y)
                positiveLimitsMax_.y = transformedData_.yAxis(n);
        }
    }

    // Values
    for (auto n = 0; n < transformedData_.nValues(); ++n)
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
    if (!source_)
        return transformedData_;

    // If the value transform is not enabled, just return the original data
    if (!valuesTransform_.enabled())
        transformedData_ = *source_;
    else
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
    if (!source_)
    {
        reinitialisePrimitives(0, GL_LINE_STRIP, true);
        return;
    }

    reinitialisePrimitives(source_->get().yAxis().size(), GL_LINE_STRIP, true);
    constructLine(transformedData().xAxis(), transformedData().yAxis(), transformedData().values(), view.axes(),
                  colourDefinition);
}

// Send primitives for rendering
const void RenderableData2D::sendToGL(const double pixelScaling)
{
    // Apply the LineStyle of the Renderable
    lineStyle_.sendToGL(pixelScaling);

    // Disable lighting
    glDisable(GL_LIGHTING);

    for (auto n = 0; n < nPrimitives(); ++n)
        primitive(n)->sendToGL();

    // Reset LineStyle back to defaults
    LineStyle().sendToGL();
}

// Create line strip primitive
void RenderableData2D::constructLine(const std::vector<double> &displayXAbscissa, const std::vector<double> &displayYAbscissa,
                                     const Array2D<double> &displayValues, const Axes &axes,
                                     const ColourDefinition &colourDefinition)
{
    // Copy and transform abscissa values (still in data space) into axes coordinates
    auto x = displayXAbscissa;
    axes.transformX(x);
    auto nX = x.size();
    if (nX < 2)
        return;

    // Copy and transform abscissa values (still in data space) into axes coordinates
    auto y = displayYAbscissa;
    axes.transformY(y);
    auto nY = y.size();
    if (nY == 0)
        return;

    // Get some values from axes so we can calculate colours properly
    auto vLogarithmic = axes.logarithmic(2);

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
        for (auto n = 0; n < nY; ++n)
        {
            // Set vertexA to -1 so we don't draw a line at n=0
            vertexA = -1;
            p = primitive(n);
            // Loop over x
            for (auto m = 0; m < nX; ++m)
            {
                vertexB = p->defineVertex(x[m], y[n], v[{m, n}], nrm, colour);

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
        for (auto n = 0; n < nY; ++n)
        {
            // Set vertexA to -1 so we don't draw a line at n=0
            vertexA = -1;
            p = primitive(n);

            // Loop over x
            for (auto m = 0; m < nX; ++m)
            {
                // Assigning colour based on value
                double c = (vLogarithmic ? pow(displayValues[{m, n}], 10.0) : displayValues[{m, n}]);
                colourDef.colour(c, colour);
                vertexB = p->defineVertex(x[m], y[n], v[{m, n}], nrm, colour);

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
    return EnumOptions<RenderableData2D::Data2DDisplayStyle>("Data2DDisplayStyle", {{RenderableData2D::LinesStyle, "Lines"}});
}

// Set display style for renderable
void RenderableData2D::setDisplayStyle(Data2DDisplayStyle displayStyle)
{
    displayStyle_ = displayStyle;

    ++styleVersion_;
}

// Return display style for the renderable
RenderableData2D::Data2DDisplayStyle RenderableData2D::displayStyle() const { return displayStyle_; }
