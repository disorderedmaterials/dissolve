/*
	*** Renderable Group
	*** src/gui/viewer/renderablegroup.h
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

#ifndef DISSOLVE_RENDERABLEGROUP_H
#define DISSOLVE_RENDERABLEGROUP_H

#include "gui/viewer/render/colourdefinition.h"
#include "base/charstring.h"
#include "base/sysfunc.h"
#include "templates/array.h"
#include "templates/list.h"
#include "templates/listitem.h"
#include "templates/reflist.h"

// Forward declarations
class Renderable;

// Renderable Group
class RenderableGroup : public ListItem<RenderableGroup>
{
	public:
	// Constructor
	RenderableGroup(const char* name, ColourDefinition::StockColour colour);

	private:
	// Name of the group
	CharString name_;
	// Renderables using this group
	RefList<Renderable,int> collections_;
	// Colour associated to this group
	ColourDefinition::StockColour stockColour_;
	// Colour definition for this group
	ColourDefinition colour_;
	// Whether vertical shifting is enabled in this group
	bool hasVerticalShift_;
	// Shift (in vertical axis) to apply to Renderables
	double verticalShift_;

	private:
	// Set vertical shift in all Renderables in the group via their transform equations
	void setVerticalShiftInRenderables();

	public:
	// Return name of group
	const char* name() const;
	// Return colour associated to the group
	ColourDefinition::StockColour stockColour() const;
	// Return colour definition for the group
	const ColourDefinition& colour() const;
	// Associate Renderable to group (if it isn't already)
	void associateRenderable(Renderable* collection);
	// Remove Renderable from group (if it exists)
	void removeRenderable(Renderable* collection);
	// Return whether the group is used by the specified collection
	bool usedByRenderable(Renderable* collection);
	// Return whether the group is empty
	bool isEmpty();
	// Set whether vertical shifting is enabled in this group
	void setVerticalShift(bool enabled, double verticalShift);
	// Whether vertical shifting is enabled in this group
	bool hasVerticalShift();
	// Return shift (in vertical axis) to apply to Renderables
	double verticalShift();
};

// Renderable Group Manager
class RenderableGroupManager
{
	public:
	// Constructor
	RenderableGroupManager();

	/*
	 * Group Management
	 */
	private:
	// List of current RenderableGroups in use
	List<RenderableGroup> collectionGroups_;
	// Usage counters for stock colours
	Array<int> stockColourUsageCount_;

	public:
	// Add Renderable to its specified group, creating / associating if necessary
	RenderableGroup* addToGroup(Renderable* collection);
	// Return named group, if it exists
	RenderableGroup* group(const char* name);
	// Return group for specified Renderable, if one has been assigned
	RenderableGroup* group(Renderable* collection);
	// Remove Renderable from its specified group
	void removeFromGroup(Renderable* collection);


	/*
	 * Properties
	 */
	public:
	// Vertical shifts enum
	enum VerticalShift
	{
		NoVerticalShift,
		HalfVerticalShift,
		OneVerticalShift,
		TwoVerticalShift,
		nVerticalShifts
	};

	private:
	// Current vertical shift delta index being applied to groups
	VerticalShift verticalShift_;

	private:
	// Set vertical shifts for current RenderableGroups
	void setRenderableGroupShifts();

	public:
	// Return colour definition for specified Renderable
	const ColourDefinition& colourDefinition(Renderable* collection);
	// Cycle vertical shift applied to RenderableGroups
	void cycleVerticalShifts();
	// Set vertical shift applied to RenderableGroups
	void setVerticalShift(VerticalShift shiftType);
	// Return current vertical shift type
	VerticalShift verticalShift() const;
	// Remove all vertical shifts from RenderableGroups
	void removeVerticalShifts();
};

#endif
