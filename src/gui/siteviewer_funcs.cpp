/*
    *** Species Viewer - Functions
    *** src/gui/siteviewer_funcs.cpp
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
    setInteractionMode(SiteViewer::DefaultInteraction);
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
        speciesRenderable_ = new RenderableSpecies(species_, species_->objectTag());
        speciesRenderable_->setName("Species");
        speciesRenderable_->setDisplayStyle(RenderableSpecies::LinesStyle);
        ownRenderable(speciesRenderable_);
        view_.showAllData();
    }

    // Send relevant signals
    emit(atomSelectionChanged());
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
        siteRenderable_ = new RenderableSpeciesSite(species_, site_);
        siteRenderable_->setName("Site");
        ownRenderable(siteRenderable_);
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
    // 	else Messenger::warn("No RenderableSpecies exists, so can't set its draw style.\n");
}

// Return current renderable draw style
RenderableSpecies::SpeciesDisplayStyle SiteViewer::speciesRenderableDrawStyle() const
{
    if (speciesRenderable_)
        return speciesRenderable_->displayStyle();
    // 	else Messenger::warn("No RenderableSpecies exists, so can't return its draw style.\n");

    return RenderableSpecies::LinesStyle;
}

// Recreate selection primitive
void SiteViewer::recreateSelectionPrimitive()
{
    if (speciesRenderable_)
        speciesRenderable_->recreateSelectionPrimitive();
}
