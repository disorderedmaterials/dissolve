// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/render/renderable.h"
#include "math/data3D.h"

// Forward Declarations
class Axes;

// Renderable for Data3D
class RenderableData3D : public Renderable
{
    public:
    RenderableData3D(const Data3D &source);
    RenderableData3D(std::string_view tag);
    ~RenderableData3D() = default;

    /*
     * Data
     */
    private:
    // Source data
    OptionalReferenceWrapper<const Data3D> source_;

    public:
    // Return source data
    OptionalReferenceWrapper<const Data3D> source() const;
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
    Data3D transformedData_;

    protected:
    // Transform data according to current settings
    void transformValues() override;
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
    void constructLine(const std::vector<double> &displayXAbscissa, const std::vector<double> &displayYAbscissa,
                       const std::vector<double> &displayAbscissa, const Array3D<double> &displayValues, const Axes &axes,
                       const ColourDefinition &colourDefinition);
    void marchingCubesOriginal(const std::vector<double> &displayXAbscissa, const std::vector<double> &displayYAbscissa,
                               const std::vector<double> &displayZAbscissa, const Array3D<double> &displayValues,
                               double lowerCutoff, double upperCutoff, const ColourDefinition &colourDefinition,
                               const Axes &axes, Primitive *primitive);

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
};
