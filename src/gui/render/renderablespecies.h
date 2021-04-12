// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/species.h"
#include "gui/render/renderable.h"

// Forward Declarations
class Axes;

// Renderable for Species
class RenderableSpecies : public Renderable
{
    public:
    RenderableSpecies(const Species *source, std::string_view name);
    ~RenderableSpecies();

    /*
     * Data
     */
    private:
    // Source data
    const Species *source_;

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
    Primitive *atomPrimitive_, *selectedAtomPrimitive_, *crossBoxPrimitive_, *bondPrimitive_;
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
    void recreateDrawInteractionPrimitive(SpeciesAtom *fromAtom, Vec3<double> toPoint, Elements::Element toElement);
    // Recreate interaction Primitive to display drawing interaction (from point to point)
    void recreateDrawInteractionPrimitive(Vec3<double> fromPoint, Elements::Element fromElement, Vec3<double> toPoint,
                                          Elements::Element toElement);
    // Recreate interaction Primitive to display delete interaction (from existing atom to existing atom)
    void recreateDeleteInteractionPrimitive(SpeciesAtom *fromAtom, SpeciesAtom *toAtom = nullptr);

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
