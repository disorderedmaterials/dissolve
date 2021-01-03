// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/species.h"
#include "classes/speciessite.h"
#include "gui/render/renderable.h"

// Forward Declarations
class Axes;

// Renderable for SpeciesSite
class RenderableSpeciesSite : public Renderable
{
    public:
    RenderableSpeciesSite(const Species *sp, const SpeciesSite *site);
    ~RenderableSpeciesSite();

    /*
     * Data
     */
    private:
    // Source Species
    const Species *speciesSource_;
    // Source site in Species
    const SpeciesSite *siteSource_;

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
    // Transform data values according to current settings
    void transformValues();

    /*
     * Rendering Primitives
     */
    private:
    // Basic primitives
    Primitive *originPrimitive_, *crossPrimitive_, *axesPrimitive_;
    // Main assemblies
    PrimitiveAssembly siteAssembly_;

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
    enum SpeciesSiteDisplayStyle
    {
        LinesStyle,
        nSpeciesSiteDisplayStyles
    };
    // Return EnumOptions for SpeciesSiteDisplayStyle
    static EnumOptions<SpeciesSiteDisplayStyle> speciesSiteDisplayStyles();

    private:
    // Display style for the renderable
    SpeciesSiteDisplayStyle displayStyle_;

    public:
    // Set display style for renderable
    void setDisplayStyle(SpeciesSiteDisplayStyle displayStyle);
    // Return display style for the renderable
    SpeciesSiteDisplayStyle displayStyle() const;

    /*
     * Style I/O
     */
    public:
    // SpeciesSiteStyle Keywords Enum
    enum SpeciesSiteStyleKeyword
    {
        DisplayKeyword,  /* 'Display' - General display style for renderable */
        EndStyleKeyword, /* 'EndStyle' - End of Style block */
        nSpeciesSiteStyleKeywords
    };
    // Return enum option info for RenderableKeyword
    static EnumOptions<RenderableSpeciesSite::SpeciesSiteStyleKeyword> speciesSiteStyleKeywords();
    // Write style information
    bool writeStyleBlock(LineParser &parser, int indentLevel = 0) const;
    // Read style information
    bool readStyleBlock(LineParser &parser);
};
