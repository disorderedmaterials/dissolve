// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/species.h"
#include "data/elements.h"
#include "gui/render/renderableSpecies.h"
#include "gui/render/renderableSpeciesSite.h"
#include "gui/speciesViewer.hui"

SpeciesViewer::SpeciesViewer(QWidget *parent) : BaseViewer(parent)
{
    // Source data
    species_ = nullptr;
    site_ = nullptr;
    speciesRenderable_ = nullptr;
    siteRenderable_ = nullptr;

    // Interaction
    setInteractionMode(SpeciesViewer::InteractionMode::Select);
    transientInteractionMode_ = SpeciesViewer::TransientInteractionMode::None;
    drawElement_ = Elements::C;

    // Set up the view
    view_.setViewType(View::NormalView);
    view_.axes().setTitle(0, "X");
    view_.axes().setTitle(1, "Y");
    view_.axes().setTitle(2, "Z");

    // Tweak the options of the underlying BaseViewer
    setClipToAxesVolume(false);
}

/*
 * Source Species and Site
 */

// Set target Species
void SpeciesViewer::setSpecies(Species *sp)
{
    species_ = sp;
    if (speciesRenderable_ != nullptr)
        removeRenderable(speciesRenderable_);
    speciesRenderable_ = nullptr;

    // Clear Renderables
    clearRenderables();

    // Create a new Renderable for the supplied Species
    if (species_)
    {
        speciesRenderable_ = createRenderable<RenderableSpecies, Species>(species_, species_->name());
        addRenderable(speciesRenderable_);

        view_.showAllData();
    }

    // Send relevant signals
    Q_EMIT(atomsChanged());
}

// Return target Species
Species *SpeciesViewer::species() const { return species_; }

// Set target SpeciesSite
void SpeciesViewer::setSite(SpeciesSite *site)
{
    site_ = site;
    if (siteRenderable_ != nullptr)
        removeRenderable(siteRenderable_);
    siteRenderable_ = nullptr;

    // Create a new Renderable for the SpeciesSite
    if (site_ != nullptr && sitesVisible_)
    {
        siteRenderable_ = std::make_shared<RenderableSpeciesSite>(species_, site_);
        siteRenderable_->setName("Site");
        addRenderable(siteRenderable_);
    }
}

// Return target site
SpeciesSite *SpeciesViewer::speciesSite() const { return site_; }

// Toggle visibility of target site, if there is one being rendered
void SpeciesViewer::setSiteVisible(bool visible)
{
    sitesVisible_ = visible;
    if (visible && site_ && !siteRenderable_)
    {
        siteRenderable_ = std::make_shared<RenderableSpeciesSite>(species_, site_);
        siteRenderable_->setName("Site");
        addRenderable(siteRenderable_);
        postRedisplay();
    }
    else if (!visible && siteRenderable_)
    {
        removeRenderable(siteRenderable_);
        siteRenderable_ = nullptr;
    }
}

/*
 * Renderable
 */

// Set renderable drawing style
void SpeciesViewer::setRenderableDrawStyle(RenderableSpecies::SpeciesDisplayStyle ds)
{
    if (speciesRenderable_)
        speciesRenderable_->setDisplayStyle(ds);
}

// Return current renderable draw style
RenderableSpecies::SpeciesDisplayStyle SpeciesViewer::renderableDrawStyle() const
{
    if (speciesRenderable_)
        return speciesRenderable_->displayStyle();

    return RenderableSpecies::LinesStyle;
}

// Recreate selection primitive
void SpeciesViewer::recreateSelectionPrimitive()
{
    if (speciesRenderable_)
        speciesRenderable_->recreateSelectionPrimitive();
}
