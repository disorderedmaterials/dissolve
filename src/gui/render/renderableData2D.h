// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/render/renderable.h"
#include "math/data2D.h"

// Forward Declarations
class Axes;

// Renderable for Data2D
class RenderableData2D : public Renderable
{
    public:
    RenderableData2D(const Data2D &source);
    RenderableData2D(std::string_view tag);
    ~RenderableData2D() = default;

    /*
     * Data
     */
    private:
    // Source data
    OptionalReferenceWrapper<const Data2D> source_;

    public:
    // Return source data
    OptionalReferenceWrapper<const Data2D> source() const;
    // Attempt to set the data source, searching the supplied list for the object
    void validateDataSource(const GenericList &sourceList) override;
    // Invalidate the current data source
    void invalidateDataSource() override;
    // Return version of data
    int dataVersion() override;

    /*
     * Transform / Limits
     */
    private:
    // Transformed data
    Data2D transformedData_;

    protected:
    // Transform data according to current settings
    void transformValues() override;
    // Return reference to transformed data
    const Data2D &transformedData();

    /*
     * Rendering Primitives
     */

    private:
    // Create line strip primitive
    void constructLine(const std::vector<double> &displayXAbscissa, const std::vector<double> &displayYAbscissa,
                       const Array2D<double> &displayValues, const Axes &axes, const ColourDefinition &colourDefinition);

    protected:
    // Recreate necessary primitives / primitive assemblies for the data
    void recreatePrimitives(const View &view, const ColourDefinition &colourDefinition) override;
    // Send primitives for rendering
    const void sendToGL(double pixelScaling) override;

    /*
     * Style
     */
    public:
    // Display Styles enum
    enum Data2DDisplayStyle
    {
        LinesStyle,
        nData2DDisplayStyles
    };
    // Return EnumOptions for Data2DDisplayStyle
    static EnumOptions<Data2DDisplayStyle> data2DDisplayStyles();

    private:
    // Display style for the renderable
    Data2DDisplayStyle displayStyle_;

    public:
    // Set display style for renderable
    void setDisplayStyle(Data2DDisplayStyle displayStyle);
    // Return display style for the renderable
    Data2DDisplayStyle displayStyle() const;
};
