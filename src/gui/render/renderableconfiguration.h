/*
    *** Renderable - Configuration
    *** src/gui/render/renderableconfiguration.h
    Copyright T. Youngs 2019-2020

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

#include "classes/configuration.h"
#include "gui/render/renderable.h"

// Forward Declarations
class Axes;

// Renderable for Configuration
class RenderableConfiguration : public Renderable
{
    public:
    RenderableConfiguration(const Configuration *source, std::string_view objectTag);
    ~RenderableConfiguration();

    /*
     * Data
     */
    private:
    // Source data
    const Configuration *source_;

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
    protected:
    // Transform data according to current settings
    void transformValues();

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

    private:
    // Create cylinder bond between supplied atoms in specified assembly
    void createCylinderBond(PrimitiveAssembly &assembly, const Atom *i, const Atom *j, const Vec3<double> vij,
                            bool drawFromAtoms, double radialScaling);

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

    /*
     * Style I/O
     */
    public:
    // ConfigurationStyle Keywords Enum
    enum ConfigurationStyleKeyword
    {
        DisplayKeyword,  /* 'Display' - General display style for renderable */
        EndStyleKeyword, /* 'EndStyle' - End of Style block */
        nConfigurationStyleKeywords
    };
    // Return enum option info for RenderableKeyword
    static EnumOptions<RenderableConfiguration::ConfigurationStyleKeyword> configurationStyleKeywords();
    // Write style information
    bool writeStyleBlock(LineParser &parser, int indentLevel = 0) const;
    // Read style information
    bool readStyleBlock(LineParser &parser);
};
