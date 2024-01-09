// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "gui/render/renderable.h"

// Forward Declarations
class Axes;

// Renderable for Configuration
class RenderableConfiguration : public Renderable
{
    public:
    RenderableConfiguration(const Configuration *source);
    ~RenderableConfiguration();

    /*
     * Data
     */
    private:
    // Source data
    const Configuration *source_;

    public:
    // Return version of data
    int dataVersion() override;

    /*
     * Transform / Limits
     */
    protected:
    // Transform data according to current settings
    void transformValues() override;

    /*
     * Rendering Primitives
     */
    private:
    // Basic primitives
    Primitive *atomPrimitive_, *unitCellPrimitive_, *bondPrimitive_;
    // Main primitives
    Primitive *lineConfigurationPrimitive_, *lineInteractionPrimitive_;
    // Main assemblies
    PrimitiveAssembly configurationAssembly_, interactionAssembly_, unitCellAssembly_;

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
    enum ConfigurationDisplayStyle
    {
        LinesStyle,
        SpheresStyle,
        nConfigurationDisplayStyles
    };
    // Return EnumOptions for ConfigurationDisplayStyle
    static EnumOptions<ConfigurationDisplayStyle> configurationDisplayStyles();

    private:
    // Display style for the renderable
    ConfigurationDisplayStyle displayStyle_;
    // Radius of free (unbound) atoms when drawing with lines
    double linesAtomRadius_;
    // Radius of atoms when drawing with spheres
    double spheresAtomRadius_;
    // Radius of bonds when drawing with spheres
    double spheresBondRadius_;

    public:
    // Set display style for renderable
    void setDisplayStyle(ConfigurationDisplayStyle displayStyle);
    // Return display style for the renderable
    ConfigurationDisplayStyle displayStyle() const;
};
