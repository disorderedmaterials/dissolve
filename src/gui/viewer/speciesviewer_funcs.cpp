/*
	*** Species Viewer - Functions
	*** src/gui/viewer/speciesviewer_funcs.cpp
	Copyright T. Youngs 2019

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

#include "gui/viewer/speciesviewer.hui"
#include "classes/species.h"

// Constructor
SpeciesViewer::SpeciesViewer(QWidget* parent) : BaseViewer(parent)
{
	// Source data
	species_ = NULL;
	drawStyle_ = SpeciesViewer::LineStyle;

	// Interaction
	setInteractionMode(SpeciesViewer::DefaultInteraction);

	// Set up the view
	view_.setViewType(View::NormalView);
}

// Destructor
SpeciesViewer::~SpeciesViewer()
{
}

/*
 * Target Species
 */

// Set target Species
void SpeciesViewer::setSpecies(Species *sp)
{
	species_ = sp;

	// Clear Renderables
	clearRenderables();

	// Create a new Renderable for the supplied Species
	if (species_)
	{
		createRenderable(Renderable::SpeciesRenderable, species_->objectTag(), species_->name());
		view_.showAllData();
	}
}

/*
 * Style
 */

// Set draw style
void SpeciesViewer::setDrawStyle(SpeciesViewer::DrawStyle style)
{
	drawStyle_ = style;
	update();
}
