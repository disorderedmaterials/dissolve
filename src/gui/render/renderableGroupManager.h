// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/render/renderableGroup.h"
#include "templates/optionalRef.h"

// Renderable Group Manager
class RenderableGroupManager
{
    public:
    RenderableGroupManager();
    // Clear all groups
    void clear();

    /*
     * Group Management
     */
    private:
    // Vector of current RenderableGroups in use
    std::vector<RenderableGroup> groups_;
    // Usage counters for stock colours
    std::vector<int> stockColourUsageCount_;

    public:
    // Create named group, or return existing group by the same name
    RenderableGroup &createGroup(std::string_view name);
    // Add Renderable to its specified group, creating / associating as necessary
    RenderableGroup &addToGroup(const std::shared_ptr<Renderable> &renderable, std::string_view groupName);
    // Return named group, if it exists
    OptionalReferenceWrapper<RenderableGroup> group(std::string_view name);
    // Return group for specified Renderable, if one has been assigned
    OptionalReferenceWrapper<RenderableGroup> group(const std::shared_ptr<Renderable> &renderable);
    // Return current RenderableGroups in use
    std::vector<RenderableGroup> &groups();
    const std::vector<RenderableGroup> &groups() const;
    // Empty all groups
    void emptyGroups();

    /*
     * Colouring
     */
    public:
    // Set colouring style for named group
    void setGroupColouring(std::string_view groupName, RenderableGroup::GroupColouring colouringStyle);
    // Set fixed colour for named group
    void setGroupFixedColour(std::string_view groupName, StockColours::StockColour stockColour);

    /*
     * Line Styling
     */
    public:
    // Line stipple to use for group
    void setGroupStipple(std::string_view groupName, LineStipple::StippleType stipple);

    /*
     * Vertical Shifting
     */
    public:
    // Vertical Shift Amounts
    enum VerticalShiftAmount
    {
        NoVerticalShift,
        HalfVerticalShift,
        OneVerticalShift,
        TwoVerticalShift,
        nVerticalShifts
    };

    private:
    // Current vertical shift delta index being applied to groups
    VerticalShiftAmount verticalShiftAmount_;

    private:
    // Set vertical shifts for current RenderableGroups
    void setRenderableGroupShifts();

    public:
    // Set vertical shifting style for named group
    void setGroupVerticalShifting(std::string_view groupName, RenderableGroup::VerticalShiftStyle shiftStyle);
    // Cycle vertical shift amount applied to RenderableGroups
    void cycleVerticalShiftAmount();
    // Set vertical shift amount applied to RenderableGroups
    void setVerticalShiftAmount(VerticalShiftAmount shiftAmount);
    // Return current vertical shift amount
    VerticalShiftAmount verticalShiftAmount() const;
    // Remove all vertical shifts from RenderableGroups
    void removeVerticalShifts();
};
