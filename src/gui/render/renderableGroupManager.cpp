// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/renderableGroupManager.h"
#include "base/sysFunc.h"
#include "gui/render/renderable.h"

RenderableGroupManager::RenderableGroupManager()
{
    stockColourUsageCount_.resize(StockColours::nStockColours, 0);

    clear();
}

// Clear all groups
void RenderableGroupManager::clear()
{
    std::fill(stockColourUsageCount_.begin(), stockColourUsageCount_.end(), 0);
    verticalShiftAmount_ = NoVerticalShift;

    emptyGroups();

    groups_.clear();
}

/*
 * Group Management
 */

// Create named group, or return existing group by the same name
RenderableGroup &RenderableGroupManager::createGroup(std::string_view name)
{
    // Does a group with this name already exist?
    auto optGroup = group(name);
    if (optGroup)
        return *optGroup;

    // No existing group, so must add a new one
    // First, find the StockColour with the lowest usage count
    auto lowestId = 0;
    for (auto colourId = 0; colourId < StockColours::nStockColours; ++colourId)
    {
        if (stockColourUsageCount_[colourId] < stockColourUsageCount_[lowestId])
            lowestId = colourId;
    }

    auto &newGroup = groups_.emplace_back(name, static_cast<StockColours::StockColour>(lowestId));
    ++stockColourUsageCount_[lowestId];

    // Need to update vertical shifts
    setRenderableGroupShifts();

    return newGroup;
}

// Add Renderable to its specified group, creating / associating as necessary
RenderableGroup &RenderableGroupManager::addToGroup(const std::shared_ptr<Renderable> &renderable, std::string_view groupName)
{
    // Check to see if the Renderable is already associated to a group...
    if (renderable->group())
    {
        auto &group = renderable->group()->get();
        if (group.name() == groupName)
        {
            fmt::print("Renderable '{}' already associated to group '{}'...\n", renderable->name(), group.name());
            return group;
        }

        // Remove it from the current group
        group.removeRenderable(renderable.get());
    }

    // Create / retrieve the group
    auto &renderableGroup = createGroup(groupName);

    // Add unique Renderable reference to the group
    renderableGroup.addRenderable(renderable);
    renderable->setGroup(renderableGroup);

    return renderableGroup;
}

// Return named group, if it exists
OptionalReferenceWrapper<RenderableGroup> RenderableGroupManager::group(std::string_view name)
{
    auto it = std::find_if(groups_.begin(), groups_.end(), [name](auto &group) { return group.name() == name; });
    if (it != groups_.end())
        return *it;

    return {};
}

// Return group for specified Renderable, if one has been assigned
OptionalReferenceWrapper<RenderableGroup> RenderableGroupManager::group(const std::shared_ptr<Renderable> &renderable)
{
    return renderable->group();
}

// Return current RenderableGroups in use
std::vector<RenderableGroup> &RenderableGroupManager::groups() { return groups_; }
const std::vector<RenderableGroup> &RenderableGroupManager::groups() const { return groups_; }

// Empty all groups of Renderables
void RenderableGroupManager::emptyGroups()
{
    for (auto &group : groups_)
        group.empty();
}

/*
 * Colouring
 */

// Set colouring style for named group
void RenderableGroupManager::setGroupColouring(std::string_view groupName, RenderableGroup::GroupColouring colouringStyle)
{
    auto g = group(groupName);
    if (!g)
        g = createGroup(groupName);

    g->get().setColouringStyle(colouringStyle);
}

// Set fixed colour for named group
void RenderableGroupManager::setGroupFixedColour(std::string_view groupName, StockColours::StockColour stockColour)
{
    auto g = group(groupName);
    if (!g)
        g = createGroup(groupName);

    g->get().setFixedStockColour(stockColour);
}

/*
 * Line Styling
 */

// Line stipple to use for group
void RenderableGroupManager::setGroupStipple(std::string_view groupName, LineStipple::StippleType stipple)
{
    auto g = group(groupName);
    if (!g)
        g = createGroup(groupName);

    g->get().setLineStipple(stipple);
}

/*
 * Vertical Shifting
 */

// Vertical shifts
double VerticalShiftAmounts[] = {0.0, 0.5, 1.0, 2.0};

// Set vertical shifts for current RenderableGroups
void RenderableGroupManager::setRenderableGroupShifts()
{
    auto groupIndex = 0;
    for (auto &group : groups_)
        group.applyVerticalShift(VerticalShiftAmounts[verticalShiftAmount_], groupIndex++);
}

// Set vertical shifting style for named group
void RenderableGroupManager::setGroupVerticalShifting(std::string_view groupName,
                                                      RenderableGroup::VerticalShiftStyle shiftStyle)
{
    auto g = group(groupName);
    if (!g)
        g = createGroup(groupName);

    g->get().setVerticalShiftStyle(shiftStyle);
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
