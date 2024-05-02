// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/render/colourDefinition.h"
#include "gui/render/lineStipple.h"
#include "gui/stockColours.h"
#include <array>

// Forward declarations
class Renderable;

// Renderable Group
class RenderableGroup
{
    public:
    RenderableGroup(std::string_view name, StockColours::StockColour colour);
    RenderableGroup(const RenderableGroup &other) = delete;
    RenderableGroup(RenderableGroup &&other);

    /*
     * Name
     */
    private:
    // Name of the group
    std::string name_;

    public:
    // Return name of group
    std::string_view name() const;

    /*
     * Renderable Targets
     */
    private:
    // Renderables using this group
    std::vector<std::shared_ptr<Renderable>> renderables_;

    public:
    // Add Renderable to group (if it isn't already)
    void addRenderable(const std::shared_ptr<Renderable> &renderable);
    // Remove Renderable from group (if it exists)
    void removeRenderable(Renderable *renderable);
    // Return whether the group is used by the specified renderable
    bool usedByRenderable(const Renderable *renderable) const;
    // Return list of Renderables using this group
    std::vector<std::shared_ptr<Renderable>> &renderables();
    const std::vector<std::shared_ptr<Renderable>> &renderables() const;
    // Return whether the group is empty
    bool isEmpty() const;
    // Empty the group, removing all Renderable targets
    void empty();

    /*
     * Visibility
     */
    private:
    // Whether data within the group is visible
    bool visible_;

    public:
    // Set whether group contents are visible
    void setVisible(bool visible);
    // Return whether group contents are visible
    bool isVisible() const;

    /*
     * Colouring
     */
    public:
    // Colouring to apply to targeted renderables
    enum GroupColouring
    {
        NoGroupColouring,            /* Renderables will use their own colour definition */
        FixedGroupColouring,         /* Renderables will be coloured as per the group colour definition */
        AutomaticIndividualColouring /* Renderables will be coloured individually by the group */
    };
    // Return enum options for GroupColouring
    static EnumOptions<RenderableGroup::GroupColouring> groupColourings();

    private:
    // Colouring style for the group
    GroupColouring colouringStyle_;
    // Stock colour associated to this group
    StockColours::StockColour fixedStockColour_;
    // Usage counters for stock colours when colouringStyle_ == AutomaticIndividualColouring
    std::array<int, StockColours::nStockColours> automaticStockColourUsageCount_;

    private:
    // Set colour information for the supplied Renderable, according to our settings
    void setRenderableColour(std::shared_ptr<Renderable> renderable);
    // Set all Renderable colours
    void setRenderableColours();

    public:
    // Set colouring style for the group
    void setColouringStyle(GroupColouring colouringStyle);
    // Return colouring style for the group
    GroupColouring colouringStyle() const;
    // Set fixed stock colour for the group
    void setFixedStockColour(StockColours::StockColour stockColour);
    // Return fixed stock colour associated to the group
    StockColours::StockColour fixedStockColour() const;
    // Return fixed colour definition for the group
    const ColourDefinition &fixedColour() const;

    /*
     * Line Style
     */
    private:
    // Line stipple for the group
    LineStipple::StippleType lineStipple_;

    private:
    // Set line style for the supplied Renderable, according to our settings
    void setRenderableLineStyle(Renderable *renderable);
    // Set all Renderable line styles
    void setRenderableLineStyles();

    public:
    // Set line stipple for the group
    void setLineStipple(LineStipple::StippleType stipple);
    // Return line stipple for the group
    LineStipple::StippleType lineStipple() const;

    /*
     * Vertical Shifting
     */
    public:
    // Vertical Shift Style
    enum VerticalShiftStyle
    {
        PreventVerticalShifting,   /* Data within the group will not be subject to vertical shifting */
        GroupVerticalShifting,     /* Data within the group will be shifted by the same amount */
        IndividualVerticalShifting /* Data within the group will be shifted individually and incrementally by the
                          specified amount */
    };
    // Return enum options for VerticalShiftStyle
    static EnumOptions<RenderableGroup::VerticalShiftStyle> verticalShiftStyles();

    private:
    // Vertical shifting style for this group
    VerticalShiftStyle verticalShiftStyle_;
    // Shift (in vertical axis) to apply to Renderables
    double verticalShift_;
    // Shift multiplier
    int verticalShiftMultiplier_;

    private:
    // Set vertical shift in specified Renderable
    void setRenderableVerticalShift(Renderable *renderable, int rendIndex);
    // Set vertical shift in all Renderables in the group via their transform equations
    void setRenderableVerticalShifts();

    public:
    // Set whether vertical shifting is enabled in this group
    void setVerticalShiftStyle(VerticalShiftStyle shiftStyle);
    // Return vertical shifting in force for this group
    VerticalShiftStyle verticalShiftStyle() const;
    // Apply the specified vertical shift (if VerticalShiftStyle != PreventVerticalShifting)
    void applyVerticalShift(double dy, int groupIndex);
};
