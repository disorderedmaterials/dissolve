// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/renderableSpeciesSite.h"
#include "base/lineParser.h"
#include "classes/atom.h"
#include "classes/molecule.h"
#include "classes/site.h"
#include "data/elementColours.h"
#include "gui/render/renderableGroupManager.h"
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

    // Generate temporary site(s) from the parent Species
    auto sites = siteSource_->createFromParent();
    if (sites.empty())
        return;

    // Render according to the current displayStyle
    if (displayStyle_ == LinesStyle)
    {
        for (auto &site : sites)
        {
            // Plot origin
            A.setTranslation(site->origin());
            if (site->hasAxes())
                A.setRotation(site->axes());
            siteAssembly_.add(originPrimitive_, A, 0.0, 0.0, 0.0, 1.0);
            siteAssembly_.add(crossPrimitive_, A, 0.0, 0.0, 0.0, 1.0);

            // Plot axes?
            if (site->hasAxes())
            {
                siteAssembly_.add(axesPrimitive_, A);
            }
        }
    }

    // Set the value transform data version here, since we don't use it
    valuesTransformDataVersion_ = dataVersion();
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
