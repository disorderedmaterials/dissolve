/*
    *** Renderable - Data3D
    *** src/gui/render/renderabledata3d.h
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
#include "math/data3d.h"

// Forward Declarations
class Axes;

// Renderable for Data3D
class RenderableData3D : public Renderable
{
    public:
    RenderableData3D(const Data3D *source, std::string_view objectTag);
    ~RenderableData3D();

    /*
     * Data
     */
    private:
    // Source data
    const Data3D *source_;

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
    Data3D transformedData_;

    protected:
    // Transform data according to current settings
    void transformValues();
    // Return reference to transformed data
    const Data3D &transformedData();

    /*
     * Rendering Primitives
     */
    private:
    // Primitive to represent data
    Primitive *dataPrimitive_;

    private:
    // Create line strip primitive
    void constructLine(const Array<double> &displayXAbscissa, const Array<double> &displayYAbscissa,
                       const Array<double> &displayAbscissa, const Array3D<double> &displayValues, const Axes &axes,
                       const ColourDefinition &colourDefinition);
    void marchingCubesOriginal(const Array<double> &displayXAbscissa, const Array<double> &displayYAbscissa,
                               const Array<double> &displayZAbscissa, const Array3D<double> &displayValues, double lowerCutoff,
                               double upperCutoff, const ColourDefinition &colourDefinition, const Axes &axes,
                               Primitive *primitive);

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
    enum Data3DDisplayStyle
    {
        SolidStyle,
        nData3DDisplayStyles
    };
    // Return EnumOptions for Data3DDisplayStyle
    static EnumOptions<Data3DDisplayStyle> data3DDisplayStyles();

    private:
    // Display style for the renderable
    Data3DDisplayStyle displayStyle_;
    // Lower cutoff for surface generation
    double lowerCutoff_;
    // Upper cutoff for surface generation
    double upperCutoff_;
    // Surface shininess for SolidSurface style
    double surfaceShininess_;

    public:
    // Set display style for renderable
    void setDisplayStyle(Data3DDisplayStyle displayStyle);
    // Return display style for the renderable
    Data3DDisplayStyle displayStyle() const;
    // Set lower cutoff for surface generation
    void setLowerCutoff(double cutoff);
    // Return lower cutoff for surface generation
    double lowerCutoff() const;
    // Set upper cutoff for surface generation
    void setUpperCutoff(double cutoff);
    // Return upper cutoff for surface generation
    double upperCutoff() const;

    /*
     * Style I/O
     */
    public:
    // Data3DStyle Keywords Enum
    enum Data3DStyleKeyword
    {
        DisplayKeyword,     /* 'Display' - General display style for renderable */
        EndStyleKeyword,    /* 'EndStyle' - End of Style block */
        LowerCutoffKeyword, /* 'LowerCutoff' - Lower cutoff value for surface generation */
        UpperCutoffKeyword, /* 'UpperCutoff' - Upper cutoff value for surface generation */
        nData3DStyleKeywords
    };
    // Return enum option info for RenderableKeyword
    static EnumOptions<RenderableData3D::Data3DStyleKeyword> data3DStyleKeywords();
    // Write style information
    bool writeStyleBlock(LineParser &parser, int indentLevel = 0) const;
    // Read style information
    bool readStyleBlock(LineParser &parser);
};
