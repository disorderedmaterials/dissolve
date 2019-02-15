/*
	*** Renderable Group
	*** src/gui/viewer/render/renderablegroupmanager.cpp
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

#include "gui/viewer/render/renderablegroupmanager.h"
#include "gui/viewer/render/renderable.h"
#include "base/sysfunc.h"

// Constructor
RenderableGroupManager::RenderableGroupManager()
{
	stockColourUsageCount_.initialise(ColourDefinition::nStockColours);
	verticalShift_ = NoVerticalShift;
}

// Add Renderable to its specified group, creating / associating if necessary
RenderableGroup* RenderableGroupManager::addToGroup(Renderable* renderable)
{
	// If group name specified in Renderable is empty, nothing more to do here
	if (!renderable->hasGroupName()) return NULL;

	// Does a group with this name already exist?
	RenderableGroup* renderableGroup = group(renderable->groupName());
	if (!renderableGroup)
	{
		// No existing group, so must add a new one
		// First, find the StockColour with the lowest usage count
		int lowestId = 0;
		for (int colourId = 0; colourId < ColourDefinition::nStockColours; ++colourId)
		{
			if (stockColourUsageCount_[colourId] < stockColourUsageCount_[lowestId]) lowestId = colourId;
		}

		renderableGroup = new RenderableGroup(renderable->groupName(), (ColourDefinition::StockColour) lowestId);
		groups_.own(renderableGroup);
		++stockColourUsageCount_[lowestId];

		// Need to update vertical shifts
		setRenderableGroupShifts();
	}

	// Add unique Renderable reference to the group
	renderableGroup->associateRenderable(renderable);
	return renderableGroup;
}

// Return named group, if it exists
RenderableGroup* RenderableGroupManager::group(const char* name)
{
	for (RenderableGroup* group = groups_.first(); group != NULL; group = group->next) if (DissolveSys::sameString(group->name(), name)) return group;
	return NULL;
}

// Return group for specified Renderable, if one has been assigned
RenderableGroup* RenderableGroupManager::group(Renderable* renderable)
{
	for (RenderableGroup* group = groups_.first(); group != NULL; group = group->next) if (group->usedByRenderable(renderable)) return group;
	return NULL;
}

// Remove Renderable from its specified group
void RenderableGroupManager::removeFromGroup(Renderable* renderable)
{
	// If group name specified in Renderable is empty, nothing more to do here
	if (!renderable->hasGroupName()) return;

	// Does a group with this name already exist?
	RenderableGroup* renderableGroup = group(renderable->groupName());
	if (!renderableGroup) return;

	renderableGroup->removeRenderable(renderable);

	// If the group is now empty, we can delete it
	if (renderableGroup->isEmpty())
	{
		--stockColourUsageCount_[renderableGroup->stockColour()];
		groups_.remove(renderableGroup);
	}
}

// Vertical shifts
double VerticalShiftAmounts[] = { 0.0, 0.5, 1.0, 2.0 };

// Set vertical shifts for current RenderableGroups
void RenderableGroupManager::setRenderableGroupShifts()
{
	// Loop over RenderableGroups
	double verticalShift = 0.0;
	for (RenderableGroup* group = groups_.first(); group != NULL; group = group->next)
	{
		group->setVerticalShift(verticalShift_ > 0, verticalShift);

		// Increase shift amount for the next group
		verticalShift += VerticalShiftAmounts[verticalShift_];
	}
}

// Return colour definition for specified Renderable
const ColourDefinition& RenderableGroupManager::colourDefinition(Renderable* renderable)
{
	RenderableGroup* renderableGroup = group(renderable);
	return (renderableGroup ? renderableGroup->colour() : renderable->colour());
}

// Cycle vertical shift applied to RenderableGroups
void RenderableGroupManager::cycleVerticalShifts()
{
	verticalShift_ = (RenderableGroupManager::VerticalShift) ((verticalShift_+1)%nVerticalShifts);

	setRenderableGroupShifts();
}

// Set vertical shift applied to RenderableGroups
void RenderableGroupManager::setVerticalShift(VerticalShift shiftType)
{
	verticalShift_ = shiftType;

	setRenderableGroupShifts();
}

// Return current vertical shift type
RenderableGroupManager::VerticalShift RenderableGroupManager::verticalShift() const
{
	return verticalShift_;
}

// Remove all vertical shifts from RenderableGroups
void RenderableGroupManager::removeVerticalShifts()
{
	verticalShift_ = NoVerticalShift;

	setRenderableGroupShifts();
}
