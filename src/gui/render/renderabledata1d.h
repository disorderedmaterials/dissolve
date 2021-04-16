// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/render/renderable.h"
#include "math/data1d.h"

// Forward Declarations
class Axes;

// Renderable for Data1D
class RenderableData1D : public Renderable
{
    public:
    RenderableData1D(const Data1DBase &source);
    RenderableData1D(std::string_view tag);
    ~RenderableData1D() = default;

    /*
     * Data
     */
    private:
    // Source data
    OptionalReferenceWrapper<const Data1DBase> source_;

    public:
    // Return source data
    OptionalReferenceWrapper<const Data1DBase> source() const;
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
    Data1D transformedData_;

    protected:
    // Transform data according to current settings
    void transformValues() override;
    // Return reference to transformed data
    const Data1D &transformedData();

    public:
    // Calculate min/max y value over specified x range (if possible in the underlying data)
    bool yRangeOverX(double xMin, double xMax, double &yMin, double &yMax);

    /*
     * Rendering Primitives
     */
    private:
    // Primitive to represent data
    Primitive *dataPrimitive_;

    private:
    // Create line strip primitive
    void constructLineXY(const std::vector<double> &displayAbscissa, const std::vector<double> &displayValues,
                         Primitive *primitive, const Axes &axes, const ColourDefinition &colourDefinition,
                         double zCoordinate = 0.0);

    protected:
    // Recreate necessary primitives / primitive assemblies for the data
    void recreatePrimitives(const View &view, const ColourDefinition &colourDefinition);
    // Send primitives for rendering
    const void sendToGL(const double pixelScaling);

    /*
     * Style
     */
    public:
    // Display Styles enum
    enum Data1DDisplayStyle
    {
        LinesStyle,
        nDisplayStyles
    };
    // Return EnumOptions for Data1DDisplayStyle
    static EnumOptions<Data1DDisplayStyle> data1DDisplayStyles();

    private:
    // Display style for the renderable
    Data1DDisplayStyle displayStyle_;

    public:
    // Set display style for renderable
    void setDisplayStyle(Data1DDisplayStyle displayStyle);
    // Return display style for the renderable
    Data1DDisplayStyle displayStyle() const;

    /*
     * Style I/O
     */
    public:
    // Data1DStyle Keywords Enum
    enum Data1DStyleKeyword
    {
        DisplayKeyword,  /* 'Display' - General display style for renderable */
        EndStyleKeyword, /* 'EndStyle' - End of Style block */
        nData1DStyleKeywords
    };
    // Return enum option info for RenderableKeyword
    static EnumOptions<RenderableData1D::Data1DStyleKeyword> data1DStyleKeywords();
    // Write style information
    bool writeStyleBlock(LineParser &parser, int indentLevel = 0) const;
    // Read style information
    bool readStyleBlock(LineParser &parser);
};
