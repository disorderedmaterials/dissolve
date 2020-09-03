/*
    *** Renderable - Data1D
    *** src/gui/render/renderabledata1d.h
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

#include "gui/render/renderable.h"
#include "math/data1d.h"

// Forward Declarations
class Axes;

// Renderable for Data1D
class RenderableData1D : public Renderable
{
    public:
    RenderableData1D(const Data1D *source, std::string_view objectTag);
    ~RenderableData1D();

    /*
     * Data
     */
    private:
    // Source data
    const Data1D *source_;

    private:
    // Return whether a valid data source is available (attempting to set it if not)
    bool validateDataSource();
    // Invalidate the current data source
    void invalidateDataSource();

    public:
    // Return version of data
    int dataVersion();

    /*
     * Transform / Limits
     */
    private:
    // Transformed data
    Data1D transformedData_;

    protected:
    // Transform data according to current settings
    void transformValues();
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
    void constructLineXY(const Array<double> &displayAbscissa, const Array<double> &displayValues, Primitive *primitive,
                         const Axes &axes, const ColourDefinition &colourDefinition, double zCoordinate = 0.0);

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
