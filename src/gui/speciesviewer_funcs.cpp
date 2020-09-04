/*
    *** Species Viewer - Functions
    *** src/gui/speciesviewer_funcs.cpp
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
#include "gui/speciesviewer.hui"

SpeciesViewer::SpeciesViewer(QWidget *parent) : BaseViewer(parent)
{
    // Source data
    species_ = nullptr;
    speciesRenderable_ = nullptr;

    // Interaction
    setInteractionMode(SpeciesViewer::DefaultInteraction);
    clickedAtom_ = nullptr;
    drawElement_ = &Elements::element(ELEMENT_C);

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
        speciesRenderable_ = new RenderableSpecies(species_, species_->objectTag());
        ownRenderable(speciesRenderable_);
        view_.showAllData();
    }

    // Send relevant signals
    emit(atomSelectionChanged());
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
    // 	else Messenger::warn("No RenderableSpecies exists, so can't set its draw style.\n");
}

// Return current renderable draw style
RenderableSpecies::SpeciesDisplayStyle SpeciesViewer::renderableDrawStyle() const
{
    if (speciesRenderable_)
        return speciesRenderable_->displayStyle();
    // 	else Messenger::warn("No RenderableSpecies exists, so can't return its draw style.\n");

    return RenderableSpecies::LinesStyle;
}

// Recreate selection primitive
void SpeciesViewer::recreateSelectionPrimitive()
{
    if (speciesRenderable_)
        speciesRenderable_->recreateSelectionPrimitive();
}
