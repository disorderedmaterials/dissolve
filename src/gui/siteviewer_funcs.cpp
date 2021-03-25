// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/species.h"
#include "data/elements.h"
#include "gui/render/renderablespecies.h"
#include "gui/siteviewer.hui"

SiteViewer::SiteViewer(QWidget *parent) : BaseViewer(parent)
{
    // Source data
    species_ = nullptr;
    site_ = nullptr;
    speciesRenderable_ = nullptr;
    siteRenderable_ = nullptr;

    // Interaction
    setInteractionMode(SiteViewer::InteractionMode::Select);
    transientInteractionMode_ = TransientInteractionMode::None;
    clickedAtom_ = nullptr;

    // Set up the view
    view_.setViewType(View::NormalView);
    view_.axes().setTitle(0, "X");
    view_.axes().setTitle(1, "Y");
    view_.axes().setTitle(2, "Z");

    // Tweak the options of the underlying BaseViewer
    setClipToAxesVolume(false);
}

SiteViewer::~SiteViewer() {}

/*
 * Source Species and Site
 */

// Set target Species
void SiteViewer::setSpecies(Species *sp)
{
    species_ = sp;
    speciesRenderable_ = nullptr;

    // Clear Renderables
    clearRenderables();

    // Create a new Renderable for the supplied Species
    if (species_)
    {
        speciesRenderable_ = createRenderable<RenderableSpecies, Species>(species_, "Species");
        speciesRenderable_->setDisplayStyle(RenderableSpecies::LinesStyle);
        addRenderable(speciesRenderable_);

        view_.showAllData();
    }

    // Send relevant signals
    emit(atomsChanged());
}

// Return target Species
Species *SiteViewer::species() const { return species_; }

// Set target SpeciesSite
void SiteViewer::setSite(SpeciesSite *site)
{
    site_ = site;
    if (siteRenderable_ != nullptr)
        removeRenderable(siteRenderable_);
    siteRenderable_ = nullptr;

    // Create a new Renderable for the parent Species
    if (site_)
    {
        siteRenderable_ = std::make_shared<RenderableSpeciesSite>(species_, site_);
        siteRenderable_->setName("Site");
        addRenderable(siteRenderable_);
    }
}

// Return target site
SpeciesSite *SiteViewer::speciesSite() const { return site_; }

/*
 * Renderable
 */

// Set Species renderable drawing style
void SiteViewer::setSpeciesRenderableDrawStyle(RenderableSpecies::SpeciesDisplayStyle ds)
{
    if (speciesRenderable_)
        speciesRenderable_->setDisplayStyle(ds);
}

// Return current renderable draw style
RenderableSpecies::SpeciesDisplayStyle SiteViewer::speciesRenderableDrawStyle() const
{
    if (speciesRenderable_)
        return speciesRenderable_->displayStyle();

    return RenderableSpecies::LinesStyle;
}

// Recreate selection primitive
void SiteViewer::recreateSelectionPrimitive()
{
    if (speciesRenderable_)
        speciesRenderable_->recreateSelectionPrimitive();
}
