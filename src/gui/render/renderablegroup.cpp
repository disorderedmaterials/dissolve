/*
	*** Renderable Group
	*** src/gui/render/renderablegroup.cpp
	Copyright T. Youngs 2013-2020

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

#include "gui/render/renderablegroup.h"
#include "gui/render/renderable.h"

// Constructor
RenderableGroup::RenderableGroup(const char* name, StockColours::StockColour colour) : ListItem<RenderableGroup>()
{
	name_ = name;

	visible_ = true;

	colouringStyle_ = RenderableGroup::NoGroupColouring;
	automaticStockColourUsageCount_.initialise(StockColours::nStockColours);
	automaticStockColourUsageCount_= 0;
	setFixedStockColour(StockColours::BlackStockColour);
	lineStipple_ = LineStipple::NoStipple;

	verticalShiftStyle_ = GroupVerticalShifting;
	verticalShift_ = 0.0;
	verticalShiftMultiplier_ = 1.0;
}

/*
 * Name
 */

// Return name of group
const char* RenderableGroup::name() const
{
	return name_.get();
}

/*
 * Renderable Targets
 */

// Associate Renderable to group (if it isn't already)
void RenderableGroup::associateRenderable(Renderable* renderable)
{
	if (renderables_.contains(renderable))
	{
		Messenger::warn("Group '%s' already contains the Renderable '%s', so not adding it again.\n", name(), renderable->name());
		return;
	}

	renderables_.append(renderable);

	// Apply colouring information if necessary
	setRenderableColour(renderable);

	// Apply vertical shift to the renderable if necessary
	setRenderableVerticalShift(renderable, renderables_.nItems()-1);

	// Apply line style if necessary
	setRenderableLineStyle(renderable);
}

// Remove Renderable from group (if it exists)
void RenderableGroup::removeRenderable(Renderable* renderable)
{
	if (!renderables_.contains(renderable))
	{
		Messenger::warn("Renderable '%s' is not present in the group '%s', so can't remove it.\n", renderable->name(), name());
		return;
	}

	// Remove shift from the renderable first, if one is being applied
	renderable->setValuesTransformEnabled(false);
	renderable->setValuesTransformEquation("value");

	renderables_.remove(renderable);
	renderable->setGroup(NULL);
}

// Return whether the group is used by the specified renderable
bool RenderableGroup::usedByRenderable(Renderable* renderable) const
{
	return renderables_.contains(renderable);
}

// Return list of Renderables using this group
const RefList<Renderable>& RenderableGroup::renderables() const
{
	return renderables_;
}

// Return whether the group is empty
bool RenderableGroup::isEmpty() const
{
	return renderables_.nItems() == 0;
}

// Empty the group, removing all Renderable targets
void RenderableGroup::empty()
{
	while (Renderable* renderable = renderables_.firstItem())
	{
		renderables_.remove(renderable);
		renderable->setGroup(NULL);
	}

	// Reset colour counters
	automaticStockColourUsageCount_ = 0;
}

/*
 * Visibility
 */

// Set whether group contents are visible
void RenderableGroup::setVisible(bool visible)
{
	visible_ = visible;
}

// Return whether group contents are visible
bool RenderableGroup::isVisible() const
{
	return visible_;
}

/*
 * Colouring
 */

// Return enum options for GroupColouring
EnumOptions<RenderableGroup::GroupColouring> RenderableGroup::groupColourings()
{
	static EnumOptionsList GroupColouringOptions = EnumOptionsList() <<
		EnumOption(RenderableGroup::NoGroupColouring,			"None") <<
		EnumOption(RenderableGroup::FixedGroupColouring,		"Fixed") <<
		EnumOption(RenderableGroup::AutomaticIndividualColouring,	"Automatic");

	static EnumOptions<RenderableGroup::GroupColouring> options("GroupColouring", GroupColouringOptions);

	return options;
}

// Set colour information for the supplied Renderable, according to our settings
void RenderableGroup::setRenderableColour(Renderable* rend)
{
	if (colouringStyle_ == FixedGroupColouring) rend->setColour(fixedStockColour_);
	else if (colouringStyle_ == AutomaticIndividualColouring) 
	{
		// Find the StockColour with the lowest usage count
		int lowestId = 0;
		for (int colourId = 0; colourId < StockColours::nStockColours; ++colourId)
		{
			if (automaticStockColourUsageCount_[colourId] < automaticStockColourUsageCount_[lowestId]) lowestId = colourId;
		}

		rend->setColour((StockColours::StockColour) lowestId);

		++automaticStockColourUsageCount_[lowestId];
	}
}

// Set all renderable colours
void RenderableGroup::setRenderableColours()
{
	for (Renderable* renderable : renderables_) setRenderableColour(renderable);
}

// Set colouring style for the group
void RenderableGroup::setColouringStyle(RenderableGroup::GroupColouring colouringStyle)
{
	colouringStyle_ = colouringStyle;

	setRenderableColours();
}

// Return colouring style for the group
RenderableGroup::GroupColouring RenderableGroup::colouringStyle() const
{
	return colouringStyle_;
}

// Set fixed stock colour for the group
void RenderableGroup::setFixedStockColour(StockColours::StockColour stockColour)
{
	fixedStockColour_ = stockColour;

	setRenderableColours();
}

// Return fixed stock colour associated to the group
StockColours::StockColour RenderableGroup::fixedStockColour() const
{
	return fixedStockColour_;
}

/*
 * Line Style
 */

// Set line style for the supplied Renderable, according to our settings
void RenderableGroup::setRenderableLineStyle(Renderable* rend)
{
	if (lineStipple_ != LineStipple::NoStipple) rend->lineStyle().setStipple(lineStipple_);
}

// Set all Renderable line styles
void RenderableGroup::setRenderableLineStyles()
{
	for (Renderable* renderable : renderables_) setRenderableLineStyle(renderable);
}

// Set line stipple for the group
void RenderableGroup::setLineStipple(LineStipple::StippleType stipple)
{
	lineStipple_ = stipple;

	setRenderableLineStyles();
}

// Return line stipple for the group
LineStipple::StippleType RenderableGroup::lineStipple() const
{
	return lineStipple_;
}

/*
 * Vertical Shifting
 */

// Return enum options for VerticalShiftStyle
EnumOptions<RenderableGroup::VerticalShiftStyle> RenderableGroup::verticalShiftStyles()
{
	static EnumOptionsList VerticalShiftStyleOptions = EnumOptionsList() <<
		EnumOption(RenderableGroup::PreventVerticalShifting,		"Prevent") <<
		EnumOption(RenderableGroup::GroupVerticalShifting,		"Group") <<
		EnumOption(RenderableGroup::IndividualVerticalShifting,		"Individual");	

	static EnumOptions<RenderableGroup::VerticalShiftStyle> options("VerticalShiftStyle", VerticalShiftStyleOptions);

	return options;
}

// Set vertical shift in specified Renderable
void RenderableGroup::setRenderableVerticalShift(Renderable* renderable, int rendIndex)
{
	renderable->setValuesTransformEnabled(verticalShiftStyle_ != PreventVerticalShifting);

	if (verticalShiftStyle_ == GroupVerticalShifting) renderable->setValuesTransformEquation(CharString("value+%f", verticalShift_ * verticalShiftMultiplier_));
	else if (verticalShiftStyle_ == IndividualVerticalShifting) renderable->setValuesTransformEquation(CharString("value+%f", verticalShift_ * rendIndex));
	else renderable->setValuesTransformEquation("value");
}

// Set vertical shift in all Renderables in the group via their transform equations
void RenderableGroup::setRenderableVerticalShifts()
{
	int index = 0;
	for (Renderable* renderable : renderables_) setRenderableVerticalShift(renderable, index++);
}

// Set whether vertical shifting is enabled in this group
void RenderableGroup::setVerticalShiftStyle(RenderableGroup::VerticalShiftStyle shiftStyle)
{
	verticalShiftStyle_ = shiftStyle;

	setRenderableVerticalShifts();
}

// Return vertical shifting in force for this group
RenderableGroup::VerticalShiftStyle RenderableGroup::verticalShiftStyle() const
{
	return verticalShiftStyle_;
}

// Apply the specified vertical shift (if VerticalShiftStyle != PreventVerticalShifting)
void RenderableGroup::applyVerticalShift(double dy, int groupIndex)
{
	verticalShift_ = dy;
	verticalShiftMultiplier_ = groupIndex;

	setRenderableVerticalShifts();
}
