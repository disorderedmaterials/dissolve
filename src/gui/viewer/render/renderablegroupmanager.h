/*
	*** Renderable Group Manager
	*** src/gui/viewer/renderablegroupmanager.h
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

#ifndef DISSOLVE_RENDERABLEGROUPMANAGER_H
#define DISSOLVE_RENDERABLEGROUPMANAGER_H

#include "gui/viewer/render/renderablegroup.h"
// #include "base/charstring.h"
// #include "base/sysfunc.h"
// #include "templates/array.h"
// #include "templates/list.h"
// #include "templates/listitem.h"
// #include "templates/reflist.h"

// Forward declarations
/* none */

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
	List<RenderableGroup> groups_;
	// Usage counters for stock colours
	Array<int> stockColourUsageCount_;

	public:
	// Add Renderable to its specified group, creating / associating if necessary
	RenderableGroup* addToGroup(Renderable* renderable);
	// Return named group, if it exists
	RenderableGroup* group(const char* name);
	// Return group for specified Renderable, if one has been assigned
	RenderableGroup* group(Renderable* renderable);
	// Remove Renderable from its specified group
	void removeFromGroup(Renderable* renderable);


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
	const ColourDefinition& colourDefinition(Renderable* renderable);
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
