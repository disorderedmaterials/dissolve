/*
	*** Renderable Group
	*** src/gui/viewer/render/renderablegroup.cpp
	Copyright T. Youngs 2013-2019

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/viewer/render/renderablegroup.h"
#include "gui/viewer/render/renderable.h"

// Constructor
RenderableGroup::RenderableGroup(const char* name, ColourDefinition::StockColour colour) : ListItem<RenderableGroup>()
{
	name_ = name;
	stockColour_ = colour;
	hasVerticalShift_ = false;
	verticalShift_ = 0.0;

	// Set up the ColourDefinition to be single (stock) colour
	colour_.setColourSource(ColourDefinition::SingleColourSource);
	colour_.setColourScalePoint(ColourDefinition::SingleColourSource, ColourDefinition::stockColour(stockColour_));
}

// Set vertical shift in all Renderables in the group via their transform equations
void RenderableGroup::setVerticalShiftInRenderables()
{
	RefListIterator<Renderable,int> renderableIterator(renderables_);
	while (Renderable* renderable = renderableIterator.iterate())
	{
		renderable->setTransformEnabled(1, hasVerticalShift_);
		renderable->setTransformEquation(1, CharString("y+%f", verticalShift_));
	}
}

// Return name of group
const char* RenderableGroup::name() const
{
	return name_.get();
}

// Return colour associated to the group
ColourDefinition::StockColour RenderableGroup::stockColour() const
{
	return stockColour_;
}

// Return colour definition for the group
const ColourDefinition& RenderableGroup::colour() const
{
	return colour_;
}

// Associate Renderable to group (if it isn't already)
void RenderableGroup::associateRenderable(Renderable* renderable)
{
	renderables_.addUnique(renderable);

	// Apply vertical shift to the renderable if necessary
	if (hasVerticalShift_)
	{
		renderable->setTransformEnabled(1, true);
		renderable->setTransformEquation(1, CharString("y+%f", verticalShift_));
	}
}

// Remove Renderable from group (if it exists)
void RenderableGroup::removeRenderable(Renderable* renderable)
{
	// Remove shift from the renderable first, if one is being applied
	if (hasVerticalShift_ && renderables_.contains(renderable))
	{
		renderable->setTransformEnabled(1, false);
		renderable->setTransformEquation(1, "y");
	}

	renderables_.remove(renderable);
}

// Return whether the group is used by the specified renderable
bool RenderableGroup::usedByRenderable(Renderable* renderable) const
{
	return renderables_.contains(renderable);
}

// Return whether the group is empty
bool RenderableGroup::isEmpty() const
{
	return renderables_.nItems() == 0;
}

// Set whether vertical shifting is enabled in this group
void RenderableGroup::setVerticalShift(bool enabled, double verticalShift)
{
	hasVerticalShift_ = enabled;
	verticalShift_ = verticalShift;

	setVerticalShiftInRenderables();
}

// Whether vertical shifting is enabled in this group
bool RenderableGroup::hasVerticalShift() const
{
	return hasVerticalShift_;
}

// Return shift (in vertical axis) to apply to Renderables
double RenderableGroup::verticalShift() const
{
	return verticalShift_;
}
