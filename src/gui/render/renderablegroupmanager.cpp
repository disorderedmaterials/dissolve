/*
    *** Renderable Group
    *** src/gui/render/renderablegroupmanager.cpp
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

#include "gui/render/renderablegroupmanager.h"
#include "base/sysfunc.h"
#include "gui/render/renderable.h"

RenderableGroupManager::RenderableGroupManager()
{
    stockColourUsageCount_.initialise(StockColours::nStockColours);

    clear();
}

// Clear all groups
void RenderableGroupManager::clear()
{
    stockColourUsageCount_ = 0;
    verticalShiftAmount_ = NoVerticalShift;

    emptyGroups();

    groups_.clear();
}

/*
 * Group Management
 */

// Create named group, or return existing group by the same name
RenderableGroup *RenderableGroupManager::createGroup(std::string_view name)
{
    // Does a group with this name already exist?
    RenderableGroup *renderableGroup = group(name);
    if (!renderableGroup)
    {
        // No existing group, so must add a new one
        // First, find the StockColour with the lowest usage count
        auto lowestId = 0;
        for (int colourId = 0; colourId < StockColours::nStockColours; ++colourId)
        {
            if (stockColourUsageCount_[colourId] < stockColourUsageCount_[lowestId])
                lowestId = colourId;
        }

        renderableGroup = new RenderableGroup(name, (StockColours::StockColour)lowestId);
        groups_.own(renderableGroup);
        ++stockColourUsageCount_[lowestId];

        // Need to update vertical shifts
        setRenderableGroupShifts();
    }

    return renderableGroup;
}

// Add Renderable to its specified group, creating / associating as necessary
RenderableGroup *RenderableGroupManager::addToGroup(Renderable *renderable, std::string_view groupName)
{
    // Check to see if the Renderable is already associated to a group...
    if (renderable->group())
    {
        if (renderable->group()->name() == groupName)
        {
            fmt::print("Renderable '{}' already associated to group '{}'...\n", renderable->name(),
                       renderable->group()->name());
            return renderable->group();
        }

        // Remove it from the current group
        renderable->group()->removeRenderable(renderable);
    }

    // Create / retrieve the group
    RenderableGroup *renderableGroup = createGroup(groupName);

    // Add unique Renderable reference to the group
    renderableGroup->associateRenderable(renderable);
    renderable->setGroup(renderableGroup);

    return renderableGroup;
}

// Return named group, if it exists
RenderableGroup *RenderableGroupManager::group(std::string_view name)
{
    for (auto *group = groups_.first(); group != nullptr; group = group->next())
        if (group->name() == name)
            return group;
    return nullptr;
}

// Return group for specified Renderable, if one has been assigned
RenderableGroup *RenderableGroupManager::group(Renderable *renderable)
{
    for (auto *group = groups_.first(); group != nullptr; group = group->next())
        if (group->usedByRenderable(renderable))
            return group;
    return nullptr;
}

// Return current RenderableGroups in use
const List<RenderableGroup> &RenderableGroupManager::groups() const { return groups_; }

// Remove Renderable from its specified group
void RenderableGroupManager::removeFromGroup(Renderable *renderable)
{
    // If no group is currently set in the Renderable, nothing more to do here
    RenderableGroup *renderableGroup = renderable->group();
    if (!renderableGroup)
        return;

    // Remove the Renderable from the group and nullify its pointer
    renderableGroup->removeRenderable(renderable);

    // If the group is now empty, we can delete it
    if (renderableGroup->isEmpty())
    {
        if (renderableGroup->colouringStyle() == RenderableGroup::FixedGroupColouring)
            --stockColourUsageCount_[renderableGroup->fixedStockColour()];
        groups_.remove(renderableGroup);
    }
}

// Empty all groups of Renderables
void RenderableGroupManager::emptyGroups()
{
    for (auto *group = groups_.first(); group != nullptr; group = group->next())
        group->empty();
}

/*
 * Colouring
 */

// Set colouring style for named group
void RenderableGroupManager::setGroupColouring(std::string_view groupName, RenderableGroup::GroupColouring colouringStyle)
{
    RenderableGroup *g = group(groupName);
    if (!g)
        g = createGroup(groupName);

    g->setColouringStyle(colouringStyle);
}

// Set fixed colour for named group
void RenderableGroupManager::setGroupFixedColour(std::string_view groupName, StockColours::StockColour stockColour)
{
    RenderableGroup *g = group(groupName);
    if (!g)
        g = createGroup(groupName);

    g->setFixedStockColour(stockColour);
}

/*
 * Line Styling
 */

// Line stipple to use for group
void RenderableGroupManager::setGroupStipple(std::string_view groupName, LineStipple::StippleType stipple)
{
    RenderableGroup *g = group(groupName);
    if (!g)
        g = createGroup(groupName);

    g->setLineStipple(stipple);
}

/*
 * Vertical Shifting
 */

// Vertical shifts
double VerticalShiftAmounts[] = {0.0, 0.5, 1.0, 2.0};

// Set vertical shifts for current RenderableGroups
void RenderableGroupManager::setRenderableGroupShifts()
{
    // Loop over RenderableGroups
    auto groupIndex = 0;
    for (auto *group = groups_.first(); group != nullptr; group = group->next())
    {
        group->applyVerticalShift(VerticalShiftAmounts[verticalShiftAmount_], groupIndex++);
    }
}

// Set vertical shifting style for named group
void RenderableGroupManager::setGroupVerticalShifting(std::string_view groupName,
                                                      RenderableGroup::VerticalShiftStyle shiftStyle)
{
    RenderableGroup *g = group(groupName);
    if (!g)
        g = createGroup(groupName);

    g->setVerticalShiftStyle(shiftStyle);
}

// Cycle vertical shift amount applied to RenderableGroups
void RenderableGroupManager::cycleVerticalShiftAmount()
{
    verticalShiftAmount_ = (RenderableGroupManager::VerticalShiftAmount)((verticalShiftAmount_ + 1) % nVerticalShifts);

    setRenderableGroupShifts();
}

// Set vertical shift applied to RenderableGroups
void RenderableGroupManager::setVerticalShiftAmount(VerticalShiftAmount shiftType)
{
    verticalShiftAmount_ = shiftType;

    setRenderableGroupShifts();
}

// Return current vertical shift amount
RenderableGroupManager::VerticalShiftAmount RenderableGroupManager::verticalShiftAmount() const { return verticalShiftAmount_; }

// Remove all vertical shifts from RenderableGroups
void RenderableGroupManager::removeVerticalShifts()
{
    verticalShiftAmount_ = NoVerticalShift;

    setRenderableGroupShifts();
}
