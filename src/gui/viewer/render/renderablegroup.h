/*
	*** Renderable Group
	*** src/gui/viewer/renderablegroup.h
	Copyright T. Youngs 2013-2019

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

#ifndef DISSOLVE_RENDERABLEGROUP_H
#define DISSOLVE_RENDERABLEGROUP_H

#include "gui/viewer/render/colourdefinition.h"
#include "base/charstring.h"

// Forward declarations
class Renderable;

// Renderable Group
class RenderableGroup : public ListItem<RenderableGroup>
{
	public:
	// Constructor
	RenderableGroup(const char* name, ColourDefinition::StockColour colour);


	/*
	 * Name
	 */
	private:
	// Name of the group
	CharString name_;

	public:
	// Return name of group
	const char* name() const;


	/*
	 * Renderable Targets
	 */
	private:
	// Renderables using this group
	RefList<Renderable,int> renderables_;

	public:
	// Associate Renderable to group (if it isn't already)
	void associateRenderable(Renderable* renderable);
	// Remove Renderable from group (if it exists)
	void removeRenderable(Renderable* renderable);
	// Return whether the group is used by the specified renderable
	bool usedByRenderable(Renderable* renderable) const;
	// Return whether the group is empty
	bool isEmpty() const;
	// Empty the group, removing all Renderable targets
	void empty();


	/*
	 * Style
	 */
	private:
	// Whether data within the group is visible
	bool visible_;
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
	// Set whether data is visible
	void setVisible(bool visible);
	// Return whether data is visible
	bool isVisible() const;
	// Return colour associated to the group
	ColourDefinition::StockColour stockColour() const;
	// Return colour definition for the group
	const ColourDefinition& colour() const;
	// Set whether vertical shifting is enabled in this group
	void setVerticalShift(bool enabled, double verticalShift);
	// Whether vertical shifting is enabled in this group
	bool hasVerticalShift() const;
	// Return shift (in vertical axis) to apply to Renderables
	double verticalShift() const;
};

#endif
