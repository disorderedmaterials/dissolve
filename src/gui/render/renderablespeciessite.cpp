// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/render/renderablespeciessite.h"
#include "base/lineparser.h"
#include "classes/atom.h"
#include "classes/molecule.h"
#include "classes/site.h"
#include "data/elementcolours.h"
#include "gui/render/renderablegroupmanager.h"
#include "gui/render/view.h"

RenderableSpeciesSite::RenderableSpeciesSite(const Species *sp, const SpeciesSite *site)
    : Renderable(Renderable::SpeciesSiteRenderable, "SiteRenderable")
{
    // Set defaults
    displayStyle_ = LinesStyle;

    // Set sources
    speciesSource_ = sp;
    siteSource_ = site;

    // Create basic primitives
    originPrimitive_ = createPrimitive(GL_LINES, false);
    originPrimitive_->wireOrthorhomboid(0.7, 0.7, 0.7);
    crossPrimitive_ = createPrimitive(GL_LINES, false);
    crossPrimitive_->cross(0.9);
    axesPrimitive_ = createPrimitive(GL_LINES, true);
    axesPrimitive_->colouredAxes(1.2);
}

RenderableSpeciesSite::~RenderableSpeciesSite() {}

/*
 * Data
 */

// Return version of data
int RenderableSpeciesSite::dataVersion() { return (siteSource_ ? siteSource_->version() : -99); }

/*
 * Rendering Primitives
 */

// Recreate necessary primitives / primitive assemblies for the data
void RenderableSpeciesSite::recreatePrimitives(const View &view, const ColourDefinition &colourDefinition)
{
    Matrix4 A;

    // Clear existing data
    siteAssembly_.clear();

    // Check source pointers
    if (!siteSource_)
        return;

    // Generate a temporary Site from the parent Species
    Site *site = siteSource_->createFromParent();
    if (!site)
        return;

    // Render according to the current displayStyle
    if (displayStyle_ == LinesStyle)
    {
        // Plot origin
        A.setTranslation(site->origin());
        if (site->hasAxes())
            A.applyRotation(site->axes());
        siteAssembly_.add(originPrimitive_, A, 0.0, 0.0, 0.0, 1.0);
        siteAssembly_.add(crossPrimitive_, A, 0.0, 0.0, 0.0, 1.0);

        // Plot axes?
        if (site->hasAxes())
        {
            A.setRotation(site->axes());
            siteAssembly_.add(axesPrimitive_, A);
        }
    }
}

// Send primitives for rendering
const void RenderableSpeciesSite::sendToGL(const double pixelScaling)
{
    glDisable(GL_LIGHTING);
    LineStyle(1.0).sendToGL();
    siteAssembly_.sendToGL(pixelScaling);
}

/*
 * Style
 */

// Return EnumOptions for SpeciesSiteDisplayStyle
EnumOptions<RenderableSpeciesSite::SpeciesSiteDisplayStyle> RenderableSpeciesSite::speciesSiteDisplayStyles()
{
    return EnumOptions<RenderableSpeciesSite::SpeciesSiteDisplayStyle>("SpeciesSiteDisplayStyle",
                                                                       {{RenderableSpeciesSite::LinesStyle, "Lines"}});
}

// Set display style for renderable
void RenderableSpeciesSite::setDisplayStyle(SpeciesSiteDisplayStyle displayStyle)
{
    displayStyle_ = displayStyle;

    ++styleVersion_;
}

// Return display style for the renderable
RenderableSpeciesSite::SpeciesSiteDisplayStyle RenderableSpeciesSite::displayStyle() const { return displayStyle_; }

/*
 * Style I/O
 */

// Return enum option info for RenderableKeyword
EnumOptions<RenderableSpeciesSite::SpeciesSiteStyleKeyword> RenderableSpeciesSite::speciesSiteStyleKeywords()
{
    return EnumOptions<RenderableSpeciesSite::SpeciesSiteStyleKeyword>(
        "SpeciesSiteStyleKeyword",
        {{RenderableSpeciesSite::DisplayKeyword, "Display", 1}, {RenderableSpeciesSite::EndStyleKeyword, "EndStyle"}});
}

// Write style information
bool RenderableSpeciesSite::writeStyleBlock(LineParser &parser, int indentLevel) const
{
    // Construct indent string
    char *indent = new char[indentLevel * 2 + 1];
    for (auto n = 0; n < indentLevel * 2; ++n)
        indent[n] = ' ';
    indent[indentLevel * 2] = '\0';

    if (!parser.writeLineF("{}{}  {}\n", indent, speciesSiteStyleKeywords().keyword(RenderableSpeciesSite::DisplayKeyword),
                           speciesSiteDisplayStyles().keyword(displayStyle_)))
        return false;

    return true;
}

// Read style information
bool RenderableSpeciesSite::readStyleBlock(LineParser &parser)
{
    while (!parser.eofOrBlank())
    {
        // Get line from file
        if (parser.getArgsDelim(LineParser::SemiColonLineBreaks) != LineParser::Success)
            return false;

        // Do we recognise this keyword and, if so, do we have an appropriate number of arguments?
        if (!speciesSiteStyleKeywords().isValid(parser.argsv(0)))
            return speciesSiteStyleKeywords().errorAndPrintValid(parser.argsv(0));
        auto kwd = speciesSiteStyleKeywords().enumeration(parser.argsv(0));
        if (!speciesSiteStyleKeywords().validNArgs(kwd, parser.nArgs() - 1))
            return false;

        // All OK, so process the keyword
        switch (kwd)
        {
            // Display style
            case (RenderableSpeciesSite::DisplayKeyword):
                if (!speciesSiteDisplayStyles().isValid(parser.argsv(1)))
                    return speciesSiteDisplayStyles().errorAndPrintValid(parser.argsv(1));
                displayStyle_ = speciesSiteDisplayStyles().enumeration(parser.argsv(1));
                break;
            // End of block
            case (RenderableSpeciesSite::EndStyleKeyword):
                return true;
            // Unrecognised Keyword
            default:
                Messenger::warn("Unrecognised display style keyword for RenderableSpeciesSite: {}\n", parser.argsv(0));
                return false;
                break;
        }
    }

    return true;
}
