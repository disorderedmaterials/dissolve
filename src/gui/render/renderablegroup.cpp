// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/render/renderablegroup.h"
#include "gui/render/renderable.h"

RenderableGroup::RenderableGroup(std::string_view name, StockColours::StockColour colour) : ListItem<RenderableGroup>()
{
    name_ = name;

    visible_ = true;

    colouringStyle_ = RenderableGroup::NoGroupColouring;
    automaticStockColourUsageCount_.initialise(StockColours::nStockColours);
    automaticStockColourUsageCount_ = 0;
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
std::string_view RenderableGroup::name() const { return name_; }

/*
 * Renderable Targets
 */

// Associate Renderable to group (if it isn't already)
void RenderableGroup::associateRenderable(Renderable *renderable)
{
    if (renderables_.contains(renderable))
    {
        fmt::print("Group '{}' already contains the Renderable '{}', so not adding it again.\n", name(), renderable->name());
        return;
    }

    renderables_.append(renderable);

    // Apply colouring information if necessary
    setRenderableColour(renderable);

    // Apply vertical shift to the renderable if necessary
    setRenderableVerticalShift(renderable, renderables_.nItems() - 1);

    // Apply line style if necessary
    setRenderableLineStyle(renderable);
}

// Remove Renderable from group (if it exists)
void RenderableGroup::removeRenderable(Renderable *renderable)
{
    if (!renderables_.contains(renderable))
    {
        fmt::print("Renderable '{}' is not present in the group '{}', so can't remove it.\n", renderable->name(), name());
        return;
    }

    // Remove shift from the renderable first, if one is being applied
    renderable->setValuesTransformEnabled(false);
    renderable->setValuesTransformEquation("value");

    renderables_.remove(renderable);
    renderable->setGroup(nullptr);
}

// Return whether the group is used by the specified renderable
bool RenderableGroup::usedByRenderable(Renderable *renderable) const { return renderables_.contains(renderable); }

// Return list of Renderables using this group
const RefList<Renderable> &RenderableGroup::renderables() const { return renderables_; }

// Return whether the group is empty
bool RenderableGroup::isEmpty() const { return renderables_.nItems() == 0; }

// Empty the group, removing all Renderable targets
void RenderableGroup::empty()
{
    while (Renderable *renderable = renderables_.firstItem())
    {
        renderables_.remove(renderable);
        renderable->setGroup(nullptr);
    }

    // Reset colour counters
    automaticStockColourUsageCount_ = 0;
}

/*
 * Visibility
 */

// Set whether group contents are visible
void RenderableGroup::setVisible(bool visible) { visible_ = visible; }

// Return whether group contents are visible
bool RenderableGroup::isVisible() const { return visible_; }

/*
 * Colouring
 */

// Return enum options for GroupColouring
EnumOptions<RenderableGroup::GroupColouring> RenderableGroup::groupColourings()
{
    return EnumOptions<RenderableGroup::GroupColouring>("GroupColouring",
                                                        {{RenderableGroup::NoGroupColouring, "None"},
                                                         {RenderableGroup::FixedGroupColouring, "Fixed"},
                                                         {RenderableGroup::AutomaticIndividualColouring, "Automatic"}});
}

// Set colour information for the supplied Renderable, according to our settings
void RenderableGroup::setRenderableColour(Renderable *rend)
{
    if (colouringStyle_ == FixedGroupColouring)
        rend->setColour(fixedStockColour_);
    else if (colouringStyle_ == AutomaticIndividualColouring)
    {
        // Find the StockColour with the lowest usage count
        auto lowestId = 0;
        for (auto colourId = 0; colourId < StockColours::nStockColours; ++colourId)
        {
            if (automaticStockColourUsageCount_[colourId] < automaticStockColourUsageCount_[lowestId])
                lowestId = colourId;
        }

        rend->setColour((StockColours::StockColour)lowestId);

        ++automaticStockColourUsageCount_[lowestId];
    }
}

// Set all renderable colours
void RenderableGroup::setRenderableColours()
{
    for (Renderable *renderable : renderables_)
        setRenderableColour(renderable);
}

// Set colouring style for the group
void RenderableGroup::setColouringStyle(RenderableGroup::GroupColouring colouringStyle)
{
    colouringStyle_ = colouringStyle;

    setRenderableColours();
}

// Return colouring style for the group
RenderableGroup::GroupColouring RenderableGroup::colouringStyle() const { return colouringStyle_; }

// Set fixed stock colour for the group
void RenderableGroup::setFixedStockColour(StockColours::StockColour stockColour)
{
    fixedStockColour_ = stockColour;

    setRenderableColours();
}

// Return fixed stock colour associated to the group
StockColours::StockColour RenderableGroup::fixedStockColour() const { return fixedStockColour_; }

/*
 * Line Style
 */

// Set line style for the supplied Renderable, according to our settings
void RenderableGroup::setRenderableLineStyle(Renderable *rend)
{
    if (lineStipple_ != LineStipple::NoStipple)
        rend->lineStyle().setStipple(lineStipple_);
}

// Set all Renderable line styles
void RenderableGroup::setRenderableLineStyles()
{
    for (Renderable *renderable : renderables_)
        setRenderableLineStyle(renderable);
}

// Set line stipple for the group
void RenderableGroup::setLineStipple(LineStipple::StippleType stipple)
{
    lineStipple_ = stipple;

    setRenderableLineStyles();
}

// Return line stipple for the group
LineStipple::StippleType RenderableGroup::lineStipple() const { return lineStipple_; }

/*
 * Vertical Shifting
 */

// Return enum options for VerticalShiftStyle
EnumOptions<RenderableGroup::VerticalShiftStyle> RenderableGroup::verticalShiftStyles()
{
    return EnumOptions<RenderableGroup::VerticalShiftStyle>("VerticalShiftStyle",
                                                            {{RenderableGroup::PreventVerticalShifting, "Prevent"},
                                                             {RenderableGroup::GroupVerticalShifting, "Group"},
                                                             {RenderableGroup::IndividualVerticalShifting, "Individual"}});
}

// Set vertical shift in specified Renderable
void RenderableGroup::setRenderableVerticalShift(Renderable *renderable, int rendIndex)
{
    renderable->setValuesTransformEnabled(verticalShiftStyle_ != PreventVerticalShifting);

    if (verticalShiftStyle_ == GroupVerticalShifting)
        renderable->setValuesTransformEquation(fmt::format("value+{}", verticalShift_ * verticalShiftMultiplier_));
    else if (verticalShiftStyle_ == IndividualVerticalShifting)
        renderable->setValuesTransformEquation(fmt::format("value+{}", verticalShift_ * rendIndex));
    else
        renderable->setValuesTransformEquation("value");
}

// Set vertical shift in all Renderables in the group via their transform equations
void RenderableGroup::setRenderableVerticalShifts()
{
    auto index = 0;
    for (Renderable *renderable : renderables_)
        setRenderableVerticalShift(renderable, index++);
}

// Set whether vertical shifting is enabled in this group
void RenderableGroup::setVerticalShiftStyle(RenderableGroup::VerticalShiftStyle shiftStyle)
{
    verticalShiftStyle_ = shiftStyle;

    setRenderableVerticalShifts();
}

// Return vertical shifting in force for this group
RenderableGroup::VerticalShiftStyle RenderableGroup::verticalShiftStyle() const { return verticalShiftStyle_; }

// Apply the specified vertical shift (if VerticalShiftStyle != PreventVerticalShifting)
void RenderableGroup::applyVerticalShift(double dy, int groupIndex)
{
    verticalShift_ = dy;
    verticalShiftMultiplier_ = groupIndex;

    setRenderableVerticalShifts();
}
