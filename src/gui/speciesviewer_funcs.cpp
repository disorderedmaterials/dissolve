// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/species.h"
#include "data/elements.h"
#include "gui/render/renderablespecies.h"
#include "gui/speciesviewer.hui"

SpeciesViewer::SpeciesViewer(QWidget *parent) : BaseViewer(parent)
{
    // Source data
    species_ = nullptr;
    speciesRenderable_ = nullptr;

    // Interaction
    setInteractionMode(SpeciesViewer::InteractionMode::Select);
    transientInteractionMode_ = SpeciesViewer::TransientInteractionMode::None;
    clickedAtom_ = nullptr;
    drawElement_ = Elements::H;

    // Set up the view
    view_.setViewType(View::NormalView);
    view_.axes().setTitle(0, "X");
    view_.axes().setTitle(1, "Y");
    view_.axes().setTitle(2, "Z");

    // Tweak the options of the underlying BaseViewer
    setClipToAxesVolume(false);
}

SpeciesViewer::~SpeciesViewer() {}

/*
 * Target Species
 */

// Set target Species
void SpeciesViewer::setSpecies(Species *sp)
{
    species_ = sp;
    speciesRenderable_ = nullptr;

    // Clear Renderables
    clearRenderables();

    // Create a new Renderable for the supplied Species
    if (species_)
    {
        speciesRenderable_ = createRenderable<RenderableSpecies, Species>(species_, species_->name());

        view_.showAllData();
    }

    // Send relevant signals
    emit(atomsChanged());
}

// Return target Species
Species *SpeciesViewer::species() const { return species_; }

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
