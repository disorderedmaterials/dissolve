// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/renderableData3D.h"
#include "base/lineParser.h"
#include "gui/render/renderableGroupManager.h"
#include "gui/render/view.h"
#include "items/list.h"
#include "math/data3D.h"
#include "templates/array2D.h"
#include "templates/array3D.h"

RenderableData3D::RenderableData3D(const Data3D &source)
    : Renderable(Renderable::Data3DRenderable, ""), source_(source), displayStyle_(SolidStyle), lowerCutoff_(0.0),
      upperCutoff_(1.0), surfaceShininess_(128.0)
{
    dataPrimitive_ = createPrimitive();
}

RenderableData3D::RenderableData3D(std::string_view tag)
    : Renderable(Renderable::Data3DRenderable, tag), displayStyle_(SolidStyle), lowerCutoff_(0.0), upperCutoff_(1.0),
      surfaceShininess_(128.0)
{
    dataPrimitive_ = createPrimitive();
}

/*
 * Data
 */

// Return source data
OptionalReferenceWrapper<const Data3D> RenderableData3D::source() const { return source_; }

// Attempt to set the data source, searching the supplied list for the object
void RenderableData3D::validateDataSource(const GenericList &sourceList)
{
    // Don't try to access source_ if we are not currently permitted to do so
    if (!sourceDataAccessEnabled_)
        return;

    if (source_)
        return;

    source_ = sourceList.search<const Data3D>(tag_);
}

// Invalidate the current data source
void RenderableData3D::invalidateDataSource() { source_ = std::nullopt; }

// Return version of data
int RenderableData3D::dataVersion() { return (source_ ? source_->get().version() : -99); }

/*
 * Transform / Limits
 */

// Transform data according to current settings
void RenderableData3D::transformValues()
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
        limitsMin_.set(transformedData_.xAxis().front(), transformedData_.yAxis().front(), transformedData_.zAxis().front());
        limitsMax_.set(transformedData_.xAxis().back(), transformedData_.yAxis().back(), transformedData_.zAxis().back());
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

    // Z
    for (auto n = 0; n < transformedData_.zAxis().size(); ++n)
    {
        if (transformedData_.zAxis(n) > 0.0)
        {
            if (positiveLimitsMin_.z < 0.0)
                positiveLimitsMin_.z = transformedData_.zAxis(n);
            else if (transformedData_.zAxis(n) < positiveLimitsMin_.z)
                positiveLimitsMin_.z = transformedData_.zAxis(n);

            if (transformedData_.zAxis(n) > positiveLimitsMax_.z)
                positiveLimitsMax_.z = transformedData_.zAxis(n);
        }
    }

    // Values
    for (auto n = 0; n < transformedData_.nValues(); ++n)
    {
        auto val = transformedData_.values().linearValue(n);
        if (val > 0.0)
        {
            if (positiveValuesMin_ < 0.0)
                positiveValuesMin_ = val;
            else if (val < positiveValuesMin_)
                positiveValuesMin_ = val;

            if (val > positiveValuesMax_)
                positiveValuesMax_ = val;
        }
    }

    // Update the transformed data 'version'
    valuesTransformDataVersion_ = dataVersion();
}

// Return reference to transformed data
const Data3D &RenderableData3D::transformedData()
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
void RenderableData3D::recreatePrimitives(const View &view, const ColourDefinition &colourDefinition)
{
    dataPrimitive_->initialise(GL_TRIANGLES, true, 65536);

    if (!source_)
        return;

    marchingCubesOriginal(transformedData_.xAxis(), transformedData_.yAxis(), transformedData_.zAxis(),
                          transformedData_.values(), lowerCutoff_, upperCutoff_, colourDefinition, view.axes(), dataPrimitive_);
}

// Send primitives for rendering
const void RenderableData3D::sendToGL(const double pixelScaling)
{
    // Apply the LineStyle of the Renderable
    lineStyle_.sendToGL(pixelScaling);

    // Enable lighting and shininess
    glEnable(GL_LIGHTING);
    glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 127);

    dataPrimitive_->sendToGL();

    // Reset LineStyle back to defaults
    LineStyle().sendToGL();
}

// Marching Cube Edge Vertex Lookup Table
int edgevertices[12][2] = {{0, 1}, {1, 2}, {2, 3}, {0, 3}, {4, 5}, {5, 6}, {6, 7}, {4, 7}, {0, 4}, {1, 5}, {3, 7}, {2, 6}};

Vec3<double> vertexPos[8] = {Vec3<double>(0, 0, 0), Vec3<double>(1, 0, 0), Vec3<double>(1, 1, 0), Vec3<double>(0, 1, 0),
                             Vec3<double>(0, 0, 1), Vec3<double>(1, 0, 1), Vec3<double>(1, 1, 1), Vec3<double>(0, 1, 1)};

// Marching Cube Face Triplet Lookup Table
int facetriples[256][15] = {
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 1, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 2, 11, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 8, 3, 2, 11, 8, 11, 9, 8, -1, -1, -1, -1, -1, -1},
    {3, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 10, 2, 8, 10, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 2, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 10, 2, 1, 9, 10, 9, 8, 10, -1, -1, -1, -1, -1, -1},
    {3, 11, 1, 10, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 11, 1, 0, 8, 11, 8, 10, 11, -1, -1, -1, -1, -1, -1},
    {3, 9, 0, 3, 10, 9, 10, 11, 9, -1, -1, -1, -1, -1, -1},
    {9, 8, 11, 11, 8, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1},
    {1, 2, 11, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 4, 7, 3, 0, 4, 1, 2, 11, -1, -1, -1, -1, -1, -1},
    {9, 2, 11, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1},
    {2, 11, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1},
    {8, 4, 7, 3, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 4, 7, 10, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1},
    {9, 0, 1, 8, 4, 7, 2, 3, 10, -1, -1, -1, -1, -1, -1},
    {4, 7, 10, 9, 4, 10, 9, 10, 2, 9, 2, 1, -1, -1, -1},
    {3, 11, 1, 3, 10, 11, 7, 8, 4, -1, -1, -1, -1, -1, -1},
    {1, 10, 11, 1, 4, 10, 1, 0, 4, 7, 10, 4, -1, -1, -1},
    {4, 7, 8, 9, 0, 10, 9, 10, 11, 10, 0, 3, -1, -1, -1},
    {4, 7, 10, 4, 10, 9, 9, 10, 11, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1},
    {1, 2, 11, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 1, 2, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1},
    {5, 2, 11, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1},
    {2, 11, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1},
    {9, 5, 4, 2, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 10, 2, 0, 8, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1},
    {0, 5, 4, 0, 1, 5, 2, 3, 10, -1, -1, -1, -1, -1, -1},
    {2, 1, 5, 2, 5, 8, 2, 8, 10, 4, 8, 5, -1, -1, -1},
    {11, 3, 10, 11, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1},
    {4, 9, 5, 0, 8, 1, 8, 11, 1, 8, 10, 11, -1, -1, -1},
    {5, 4, 0, 5, 0, 10, 5, 10, 11, 10, 0, 3, -1, -1, -1},
    {5, 4, 8, 5, 8, 11, 11, 8, 10, -1, -1, -1, -1, -1, -1},
    {9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1},
    {0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1},
    {1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 8, 9, 5, 7, 11, 1, 2, -1, -1, -1, -1, -1, -1},
    {11, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1},
    {8, 0, 2, 8, 2, 5, 8, 5, 7, 11, 5, 2, -1, -1, -1},
    {2, 11, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1},
    {7, 9, 5, 7, 8, 9, 3, 10, 2, -1, -1, -1, -1, -1, -1},
    {9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 10, -1, -1, -1},
    {2, 3, 10, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1},
    {10, 2, 1, 10, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1},
    {9, 5, 8, 8, 5, 7, 11, 1, 3, 11, 3, 10, -1, -1, -1},
    {5, 7, 10, 5, 10, 11, 1, 0, 9, -1, -1, -1, -1, -1, -1},
    {10, 11, 5, 10, 5, 7, 8, 0, 3, -1, -1, -1, -1, -1, -1},
    {10, 11, 5, 7, 10, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 5, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 1, 5, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 8, 3, 1, 9, 8, 5, 11, 6, -1, -1, -1, -1, -1, -1},
    {1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1},
    {9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1},
    {5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1},
    {2, 3, 10, 11, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 0, 8, 10, 2, 0, 11, 6, 5, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 2, 3, 10, 5, 11, 6, -1, -1, -1, -1, -1, -1},
    {5, 11, 6, 1, 9, 2, 9, 10, 2, 9, 8, 10, -1, -1, -1},
    {6, 3, 10, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1},
    {0, 8, 10, 0, 10, 5, 0, 5, 1, 5, 10, 6, -1, -1, -1},
    {3, 10, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1},
    {6, 5, 9, 6, 9, 10, 10, 9, 8, -1, -1, -1, -1, -1, -1},
    {5, 11, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 3, 0, 4, 7, 3, 6, 5, 11, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 5, 11, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1},
    {11, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1},
    {6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1},
    {1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1},
    {8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1},
    {7, 3, 2, 7, 2, 6, 5, 9, 4, -1, -1, -1, -1, -1, -1},
    {3, 10, 2, 7, 8, 4, 11, 6, 5, -1, -1, -1, -1, -1, -1},
    {5, 11, 6, 4, 7, 2, 4, 2, 0, 2, 7, 10, -1, -1, -1},
    {0, 1, 9, 4, 7, 8, 2, 3, 10, 5, 11, 6, -1, -1, -1},
    {9, 4, 5, 11, 2, 1, 7, 10, 6, -1, -1, -1, -1, -1, -1},
    {8, 4, 7, 3, 10, 5, 3, 5, 1, 5, 10, 6, -1, -1, -1},
    {5, 1, 0, 5, 0, 4, 7, 10, 6, -1, -1, -1, -1, -1, -1},
    {0, 3, 8, 4, 5, 9, 10, 6, 7, -1, -1, -1, -1, -1, -1},
    {4, 5, 9, 7, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 4, 9, 6, 4, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 11, 6, 4, 9, 11, 0, 8, 3, -1, -1, -1, -1, -1, -1},
    {11, 0, 1, 11, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1},
    {8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 11, -1, -1, -1},
    {1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1},
    {0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1},
    {11, 4, 9, 11, 6, 4, 10, 2, 3, -1, -1, -1, -1, -1, -1},
    {0, 8, 2, 2, 8, 10, 4, 9, 11, 4, 11, 6, -1, -1, -1},
    {3, 10, 2, 0, 1, 6, 0, 6, 4, 6, 1, 11, -1, -1, -1},
    {6, 4, 8, 6, 8, 10, 2, 1, 11, -1, -1, -1, -1, -1, -1},
    {9, 6, 4, 9, 3, 6, 9, 1, 3, 10, 6, 3, -1, -1, -1},
    {8, 10, 6, 8, 6, 4, 9, 1, 0, -1, -1, -1, -1, -1, -1},
    {3, 10, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1},
    {6, 4, 8, 10, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 11, 6, 7, 8, 11, 8, 9, 11, -1, -1, -1, -1, -1, -1},
    {0, 7, 3, 0, 11, 7, 0, 9, 11, 6, 7, 11, -1, -1, -1},
    {11, 6, 7, 1, 11, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1},
    {11, 6, 7, 11, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1},
    {1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1},
    {2, 6, 7, 2, 7, 3, 0, 9, 1, -1, -1, -1, -1, -1, -1},
    {7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1},
    {7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 10, 11, 6, 8, 11, 8, 9, 8, 6, 7, -1, -1, -1},
    {2, 0, 9, 2, 9, 11, 6, 7, 10, -1, -1, -1, -1, -1, -1},
    {1, 11, 2, 3, 8, 0, 6, 7, 10, -1, -1, -1, -1, -1, -1},
    {11, 2, 1, 6, 7, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 9, 1, 8, 1, 3, 10, 6, 7, -1, -1, -1, -1, -1, -1},
    {0, 9, 1, 10, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 8, 0, 10, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 10, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 10, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 1, 9, 8, 3, 1, 10, 7, 6, -1, -1, -1, -1, -1, -1},
    {11, 1, 2, 6, 10, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 11, 3, 0, 8, 6, 10, 7, -1, -1, -1, -1, -1, -1},
    {2, 9, 0, 2, 11, 9, 6, 10, 7, -1, -1, -1, -1, -1, -1},
    {2, 10, 3, 11, 8, 6, 11, 9, 8, 8, 7, 6, -1, -1, -1},
    {7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1},
    {2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1},
    {1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1},
    {11, 7, 6, 11, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1},
    {11, 7, 6, 1, 7, 11, 1, 8, 7, 1, 0, 8, -1, -1, -1},
    {0, 3, 7, 0, 7, 11, 0, 11, 9, 6, 11, 7, -1, -1, -1},
    {7, 6, 11, 7, 11, 8, 8, 11, 9, -1, -1, -1, -1, -1, -1},
    {6, 8, 4, 10, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 6, 10, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1},
    {8, 6, 10, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1},
    {9, 4, 6, 9, 6, 3, 9, 3, 1, 10, 3, 6, -1, -1, -1},
    {6, 8, 4, 6, 10, 8, 2, 11, 1, -1, -1, -1, -1, -1, -1},
    {3, 2, 10, 0, 6, 1, 0, 4, 6, 6, 11, 1, -1, -1, -1},
    {0, 2, 8, 2, 10, 8, 4, 11, 9, 4, 6, 11, -1, -1, -1},
    {11, 9, 4, 11, 4, 6, 10, 3, 2, -1, -1, -1, -1, -1, -1},
    {8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1},
    {0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 8, 0, 1, 9, 2, 2, 9, 4, 2, 4, 6, -1, -1, -1},
    {1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1},
    {8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 11, 1, -1, -1, -1},
    {11, 1, 0, 11, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1},
    {4, 6, 11, 4, 11, 9, 0, 3, 8, -1, -1, -1, -1, -1, -1},
    {11, 9, 4, 6, 11, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 5, 7, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 4, 9, 5, 10, 7, 6, -1, -1, -1, -1, -1, -1},
    {5, 0, 1, 5, 4, 0, 7, 6, 10, -1, -1, -1, -1, -1, -1},
    {8, 7, 4, 3, 5, 10, 3, 1, 5, 5, 6, 10, -1, -1, -1},
    {9, 5, 4, 11, 1, 2, 7, 6, 10, -1, -1, -1, -1, -1, -1},
    {0, 9, 1, 4, 8, 7, 2, 10, 3, 5, 6, 11, -1, -1, -1},
    {5, 6, 11, 4, 2, 7, 4, 0, 2, 2, 10, 7, -1, -1, -1},
    {3, 2, 10, 7, 4, 8, 11, 5, 6, -1, -1, -1, -1, -1, -1},
    {7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1},
    {8, 7, 4, 9, 5, 0, 0, 5, 6, 0, 6, 2, -1, -1, -1},
    {1, 5, 2, 5, 6, 2, 3, 4, 0, 3, 7, 4, -1, -1, -1},
    {6, 2, 1, 6, 1, 5, 4, 8, 7, -1, -1, -1, -1, -1, -1},
    {11, 5, 6, 1, 7, 9, 1, 3, 7, 7, 4, 9, -1, -1, -1},
    {1, 0, 9, 5, 6, 11, 8, 7, 4, -1, -1, -1, -1, -1, -1},
    {4, 0, 3, 4, 3, 7, 6, 11, 5, -1, -1, -1, -1, -1, -1},
    {5, 6, 11, 4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {6, 9, 5, 6, 10, 9, 10, 8, 9, -1, -1, -1, -1, -1, -1},
    {3, 6, 10, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1},
    {0, 10, 8, 0, 5, 10, 0, 1, 5, 5, 6, 10, -1, -1, -1},
    {6, 10, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1},
    {5, 6, 11, 1, 2, 9, 9, 2, 10, 9, 10, 8, -1, -1, -1},
    {0, 9, 1, 2, 10, 3, 5, 6, 11, -1, -1, -1, -1, -1, -1},
    {10, 8, 0, 10, 0, 2, 11, 5, 6, -1, -1, -1, -1, -1, -1},
    {2, 10, 3, 11, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1},
    {9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1},
    {1, 5, 6, 1, 6, 2, 3, 8, 0, -1, -1, -1, -1, -1, -1},
    {1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 3, 8, 1, 8, 9, 5, 6, 11, -1, -1, -1, -1, -1, -1},
    {9, 1, 0, 5, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 3, 8, 5, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 5, 11, 7, 5, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 5, 11, 10, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1},
    {5, 10, 7, 5, 11, 10, 1, 9, 0, -1, -1, -1, -1, -1, -1},
    {9, 8, 5, 8, 7, 5, 11, 3, 1, 11, 10, 3, -1, -1, -1},
    {10, 1, 2, 10, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1},
    {2, 10, 3, 0, 8, 1, 1, 8, 7, 1, 7, 5, -1, -1, -1},
    {9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 10, 7, -1, -1, -1},
    {7, 5, 9, 7, 9, 8, 3, 2, 10, -1, -1, -1, -1, -1, -1},
    {2, 5, 11, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1},
    {8, 2, 0, 8, 5, 2, 8, 7, 5, 11, 2, 5, -1, -1, -1},
    {11, 2, 1, 9, 0, 5, 5, 0, 3, 5, 3, 7, -1, -1, -1},
    {9, 8, 7, 9, 7, 5, 11, 2, 1, -1, -1, -1, -1, -1, -1},
    {1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1},
    {9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1},
    {9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {5, 8, 4, 5, 11, 8, 11, 10, 8, -1, -1, -1, -1, -1, -1},
    {5, 0, 4, 5, 10, 0, 5, 11, 10, 10, 3, 0, -1, -1, -1},
    {4, 5, 9, 0, 1, 8, 8, 1, 11, 8, 11, 10, -1, -1, -1},
    {11, 10, 3, 11, 3, 1, 9, 4, 5, -1, -1, -1, -1, -1, -1},
    {2, 5, 1, 2, 8, 5, 2, 10, 8, 4, 5, 8, -1, -1, -1},
    {0, 4, 5, 0, 5, 1, 2, 10, 3, -1, -1, -1, -1, -1, -1},
    {0, 2, 10, 0, 10, 8, 4, 5, 9, -1, -1, -1, -1, -1, -1},
    {9, 4, 5, 2, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 5, 11, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1},
    {5, 11, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1},
    {3, 8, 0, 1, 11, 2, 4, 5, 9, -1, -1, -1, -1, -1, -1},
    {1, 11, 2, 9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1},
    {0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 4, 5, 0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 10, 7, 4, 9, 10, 9, 11, 10, -1, -1, -1, -1, -1, -1},
    {4, 8, 7, 9, 10, 0, 9, 11, 10, 10, 3, 0, -1, -1, -1},
    {1, 11, 10, 1, 10, 4, 1, 4, 0, 7, 4, 10, -1, -1, -1},
    {3, 1, 11, 3, 11, 10, 7, 4, 8, -1, -1, -1, -1, -1, -1},
    {4, 10, 7, 9, 10, 4, 9, 2, 10, 9, 1, 2, -1, -1, -1},
    {9, 1, 0, 8, 7, 4, 2, 10, 3, -1, -1, -1, -1, -1, -1},
    {10, 7, 4, 10, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1},
    {8, 7, 4, 3, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 9, 11, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1},
    {9, 11, 2, 9, 2, 0, 8, 7, 4, -1, -1, -1, -1, -1, -1},
    {3, 7, 4, 3, 4, 0, 1, 11, 2, -1, -1, -1, -1, -1, -1},
    {1, 11, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1},
    {0, 9, 1, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 11, 8, 11, 10, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 9, 3, 9, 10, 10, 9, 11, -1, -1, -1, -1, -1, -1},
    {0, 1, 11, 0, 11, 8, 8, 11, 10, -1, -1, -1, -1, -1, -1},
    {3, 1, 11, 10, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 1, 10, 9, 9, 10, 8, -1, -1, -1, -1, -1, -1},
    {1, 0, 9, 2, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 2, 10, 8, 0, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 8, 2, 8, 11, 11, 8, 9, -1, -1, -1, -1, -1, -1},
    {9, 11, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 3, 8, 1, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 3, 8, 11, 1, 2, 6, 10, 7, 4, 9, 5, -1, -1, -1}
    // 	{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

// marching cubes
// Render volumetric isosurface with Marching Cubes ORIGINAL
void RenderableData3D::marchingCubesOriginal(const std::vector<double> &displayXAbscissa,
                                             const std::vector<double> &displayYAbscissa,
                                             const std::vector<double> &displayZAbscissa, const Array3D<double> &displayValues,
                                             double lowerCutoff, double upperCutoff, const ColourDefinition &colourDefinition,
                                             const Axes &axes, Primitive *primitive)
{
    int i, j, k, n, cubeType, *faces;
    Vec3<GLfloat> normal, gradient[8];
    Vec3<double> r, v1;
    double vertex[8], ipol, a, b;
    GLfloat colour[4];
    std::vector<double> x = displayXAbscissa, y = displayYAbscissa, z = displayZAbscissa;

    // Get distances between grid points
    double dx, dy, dz;
    dx = x[1] - x[0];
    dy = y[1] - y[0];
    dz = z[1] - z[0];

    // Transform abscissa values (still in data space) into axes coordinates
    axes.transformX(x);
    auto nX = x.size();
    if (nX < 2)
        return;

    // Transform abscissa values (still in data space) into axes coordinates
    axes.transformY(y);
    auto nY = y.size();
    if (nY < 2)
        return;

    // Transform abscissa values (still in data space) into axes coordinates
    axes.transformZ(z);
    auto nZ = z.size();
    if (nZ < 2)
        return;

    // Get single colour for surface (if colourScale style == SingleColourStyle)
    if (colourDefinition.style() == ColourDefinition::SingleColourStyle)
        colourDefinition.colour(0.0, colour);

    // Generate isosurface
    for (i = 1; i < x.size() - 2; ++i)
    {
        for (j = 1; j < y.size() - 2; ++j)
        {
            for (k = 1; k < z.size() - 2; ++k)
            {
                // Grab values that form vertices of cube.
                vertex[0] = displayValues[std::tuple{i, j, k}];
                vertex[1] = displayValues[std::tuple{i + 1, j, k}];
                vertex[2] = displayValues[std::tuple{i + 1, j + 1, k}];
                vertex[3] = displayValues[std::tuple{i, j + 1, k}];
                vertex[4] = displayValues[std::tuple{i, j, k + 1}];
                vertex[5] = displayValues[std::tuple{i + 1, j, k + 1}];
                vertex[6] = displayValues[std::tuple{i + 1, j + 1, k + 1}];
                vertex[7] = displayValues[std::tuple{i, j + 1, k + 1}];

                // Calculate gradients at the cube vertices
                gradient[0].x = (vertex[1] - displayValues[std::tuple{i - 1, j, k}]) / dx;
                gradient[0].y = (vertex[3] - displayValues[std::tuple{i, j - 1, k}]) / dy;
                gradient[0].z = (vertex[4] - displayValues[std::tuple{i, j, k - 1}]) / dz;
                gradient[1].x = (displayValues[std::tuple{i + 2, j, k}] - vertex[0]) / dx;
                gradient[1].y = (vertex[2] - displayValues[std::tuple{i + 1, j - 1, k}]) / dy;
                gradient[1].z = (vertex[5] - displayValues[std::tuple{i + 1, j, k - 1}]) / dz;
                gradient[2].x = (displayValues[std::tuple{i + 2, j + 1, k}] - vertex[3]) / dx;
                gradient[2].y = (displayValues[std::tuple{i + 1, j + 2, k}] - vertex[1]) / dy;
                gradient[2].z = (vertex[6] - displayValues[std::tuple{i + 1, j + 1, k - 1}]) / dz;
                gradient[3].x = (vertex[2] - displayValues[std::tuple{i - 1, j + 1, k}]) / dx;
                gradient[3].y = (displayValues[std::tuple{i, j + 2, k}] - vertex[0]) / dy;
                gradient[3].z = (vertex[7] - displayValues[std::tuple{i, j + 1, k - 1}]) / dz;
                gradient[4].x = (vertex[5] - displayValues[std::tuple{i - 1, j, k + 1}]) / dx;
                gradient[4].y = (vertex[7] - displayValues[std::tuple{i, j - 1, k + 1}]) / dy;
                gradient[4].z = (displayValues[std::tuple{i, j, k + 2}] - vertex[0]) / dz;
                gradient[5].x = (displayValues[std::tuple{i + 2, j, k + 1}] - vertex[4]) / dx;
                gradient[5].y = (vertex[6] - displayValues[std::tuple{i + 1, j - 1, k + 1}]) / dy;
                gradient[5].z = (displayValues[std::tuple{i + 1, j, k + 2}] - vertex[1]) / dz;
                gradient[6].x = (displayValues[std::tuple{i + 2, j + 1, k + 1}] - vertex[7]) / dx;
                gradient[6].y = (displayValues[std::tuple{i + 1, j + 2, k + 1}] - vertex[5]) / dy;
                gradient[6].z = (displayValues[std::tuple{i + 1, j + 1, k + 2}] - vertex[2]) / dz;
                gradient[7].x = (vertex[6] - displayValues[std::tuple{i - 1, j + 1, k + 1}]) / dx;
                gradient[7].y = (displayValues[std::tuple{i, j + 2, k + 1}] - vertex[4]) / dy;
                gradient[7].z = (displayValues[std::tuple{i, j + 1, k + 2}] - vertex[3]) / dz;

                // Determine cube type
                cubeType = 0;
                if ((vertex[0] >= lowerCutoff) && (vertex[0] <= upperCutoff))
                    cubeType += 1;
                if ((vertex[1] >= lowerCutoff) && (vertex[1] <= upperCutoff))
                    cubeType += 2;
                if ((vertex[2] >= lowerCutoff) && (vertex[2] <= upperCutoff))
                    cubeType += 4;
                if ((vertex[3] >= lowerCutoff) && (vertex[3] <= upperCutoff))
                    cubeType += 8;
                if ((vertex[4] >= lowerCutoff) && (vertex[4] <= upperCutoff))
                    cubeType += 16;
                if ((vertex[5] >= lowerCutoff) && (vertex[5] <= upperCutoff))
                    cubeType += 32;
                if ((vertex[6] >= lowerCutoff) && (vertex[6] <= upperCutoff))
                    cubeType += 64;
                if ((vertex[7] >= lowerCutoff) && (vertex[7] <= upperCutoff))
                    cubeType += 128;

                if ((cubeType != 0) && (cubeType != 255))
                {
                    // Get edges from list and draw triangles or points
                    faces = facetriples[cubeType];
                    for (n = 0; n < 15; n++)
                    {
                        if (faces[n] == -1)
                            break;

                        // Interpolate between data values (a,b) along this edge
                        a = vertex[edgevertices[faces[n]][0]];
                        b = vertex[edgevertices[faces[n]][1]];
                        ipol = (lowerCutoff - a) / (b - a);
                        if (ipol > 1.0)
                            ipol = 1.0;
                        else if (ipol < 0.0)
                            ipol = 0.0;

                        // Get vector for edge (in unit coordinates) and scale to data spacing (dx, dy,
                        // dz)
                        v1 = vertexPos[edgevertices[faces[n]][0]];
                        r = (vertexPos[edgevertices[faces[n]][1]] - v1);
                        r.multiply(dx, dy, dz);
                        r *= ipol;

                        normal = -(gradient[edgevertices[faces[n]][0]] +
                                   (gradient[edgevertices[faces[n]][1]] - gradient[edgevertices[faces[n]][0]]) * ipol);
                        normal.normalise();

                        // Add data lower-left-corner coordinate to r, and data-scaled edge origin v1
                        r.add(x[i] + dx * v1[0], y[j] + dy * v1[1], z[k] + dz * v1[2]);

                        // Plot vertex
                        if (colourDefinition.style() == ColourDefinition::SingleColourStyle)
                            primitive->defineVertex(r.x, r.y, r.z, normal.x, normal.y, normal.z, colour);
                        else
                        {
                            // Get the colour from the colourscale
                            // TODO CHECK - Are we using the correct data value for the colour
                            // lookup here?
                            colourDefinition.colour((a + b) * 0.5, colour);
                            primitive->defineVertex(r.x, r.y, r.z, normal.x, normal.y, normal.z, colour);
                        }
                    }
                }
            }
        }
    }
}

/*
 * Style
 */

// Return EnumOptions for Data3DDisplayStyle
EnumOptions<RenderableData3D::Data3DDisplayStyle> RenderableData3D::data3DDisplayStyles()
{
    return EnumOptions<RenderableData3D::Data3DDisplayStyle>("Data3DDisplayStyle", {{RenderableData3D::SolidStyle, "Solid"}});
}

// Set display style for renderable
void RenderableData3D::setDisplayStyle(Data3DDisplayStyle displayStyle)
{
    displayStyle_ = displayStyle;

    ++styleVersion_;
}

// Return display style for the renderable
RenderableData3D::Data3DDisplayStyle RenderableData3D::displayStyle() const { return displayStyle_; }

// Set lower cutoff for surface generation
void RenderableData3D::setLowerCutoff(double cutoff)
{
    lowerCutoff_ = cutoff;

    ++styleVersion_;
}

// Return lower cutoff for surface generation
double RenderableData3D::lowerCutoff() const { return lowerCutoff_; }

// Set upper cutoff for surface generation
void RenderableData3D::setUpperCutoff(double cutoff)
{
    upperCutoff_ = cutoff;

    ++styleVersion_;
}

// Return upper cutoff for surface generation
double RenderableData3D::upperCutoff() const { return upperCutoff_; }
