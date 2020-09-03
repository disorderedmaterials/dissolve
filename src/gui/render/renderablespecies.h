/*
    *** Renderable - Species
    *** src/gui/render/renderablespecies.h
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

#include "classes/species.h"
#include "gui/render/renderable.h"

// Forward Declarations
class Axes;

// Renderable for Species
class RenderableSpecies : public Renderable
{
    public:
    RenderableSpecies(const Species *source, std::string_view objectTag);
    ~RenderableSpecies();

    /*
     * Data
     */
    private:
    // Source data
    const Species *source_;

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
    Primitive *atomPrimitive_, *selectedAtomPrimitive_, *unitCellPrimitive_, *bondPrimitive_;
    // Main primitives
    Primitive *lineSpeciesPrimitive_, *lineSelectionPrimitive_, *lineInteractionPrimitive_;
    // Main assemblies
    PrimitiveAssembly speciesAssembly_, selectionAssembly_, interactionAssembly_;
    // Version at which selection primitive was created, relative to selection version
    int selectionPrimitiveVersion_;

    private:
    // Create cylinder bond between supplied atoms in specified assembly
    void createCylinderBond(PrimitiveAssembly &assembly, const SpeciesAtom *i, const SpeciesAtom *j, double radialScaling);

    protected:
    // Recreate necessary primitives / primitive assemblies for the data
    void recreatePrimitives(const View &view, const ColourDefinition &colourDefinition);
    // Send primitives for rendering
    const void sendToGL(const double pixelScaling);

    public:
    // Recreate selection Primitive
    void recreateSelectionPrimitive();
    // Clear interaction Primitive
    void clearInteractionPrimitive();
    // Recreate interaction Primitive to display drawing interaction (from existing atom to existing atom)
    void recreateDrawInteractionPrimitive(SpeciesAtom *fromAtom, SpeciesAtom *toAtom);
    // Recreate interaction Primitive to display drawing interaction (from existing atom to point)
    void recreateDrawInteractionPrimitive(SpeciesAtom *fromAtom, Vec3<double> toPoint, Element *toElement);
    // Recreate interaction Primitive to display drawing interaction (from point to point)
    void recreateDrawInteractionPrimitive(Vec3<double> fromPoint, Element *fromElement, Vec3<double> toPoint,
                                          Element *toElement);

    /*
     * Style
     */
    public:
    // Display Styles enum
    enum SpeciesDisplayStyle
    {
        LinesStyle,
        SpheresStyle,
        nSpeciesDisplayStyles
    };
    // Return EnumOptions for SpeciesDisplayStyle
    static EnumOptions<SpeciesDisplayStyle> speciesDisplayStyles();

    private:
    // Display style for the renderable
    SpeciesDisplayStyle displayStyle_;
    // Radius of free (unbound) atoms when drawing with lines
    double linesAtomRadius_;
    // Radius of atoms when drawing with spheres
    double spheresAtomRadius_;
    // Radius of bonds when drawing with spheres
    double spheresBondRadius_;

    public:
    // Set display style for renderable
    void setDisplayStyle(SpeciesDisplayStyle displayStyle);
    // Return display style for the renderable
    SpeciesDisplayStyle displayStyle() const;

    /*
     * Style I/O
     */
    public:
    // SpeciesStyle Keywords Enum
    enum SpeciesStyleKeyword
    {
        DisplayKeyword,  /* 'Display' - General display style for renderable */
        EndStyleKeyword, /* 'EndStyle' - End of Style block */
        nSpeciesStyleKeywords
    };
    // Return enum option info for RenderableKeyword
    static EnumOptions<RenderableSpecies::SpeciesStyleKeyword> speciesStyleKeywords();
    // Write style information
    bool writeStyleBlock(LineParser &parser, int indentLevel = 0) const;
    // Read style information
    bool readStyleBlock(LineParser &parser);
};
