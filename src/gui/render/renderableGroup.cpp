// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/renderableGroup.h"
#include "gui/render/renderable.h"

RenderableGroup::RenderableGroup(std::string_view name, StockColours::StockColour colour)
{
    name_ = name;

    visible_ = true;

    colouringStyle_ = RenderableGroup::NoGroupColouring;
    automaticStockColourUsageCount_.fill(0);
    fixedStockColour_ = colour;
    lineStipple_ = LineStipple::NoStipple;

    verticalShiftStyle_ = GroupVerticalShifting;
    verticalShift_ = 0.0;
    verticalShiftMultiplier_ = 1.0;
}

RenderableGroup::RenderableGroup(RenderableGroup &&other)
{
    empty();

    // Copy style settings
    name_ = other.name_;
    visible_ = other.visible_;
    colouringStyle_ = other.colouringStyle_;
    automaticStockColourUsageCount_ = other.automaticStockColourUsageCount_;
    fixedStockColour_ = other.fixedStockColour_;
    lineStipple_ = other.lineStipple_;
    verticalShiftStyle_ = other.verticalShiftStyle_;
    verticalShift_ = other.verticalShift_;
    verticalShiftMultiplier_ = other.verticalShiftMultiplier_;

    // Copy the renderables by hand
    for (auto &renderable : other.renderables_)
    {
        renderables_.emplace_back(renderable);
        renderable->setGroup(*this);
    }
    other.renderables_.clear();
    other.automaticStockColourUsageCount_.fill(0);
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
void RenderableGroup::addRenderable(const std::shared_ptr<Renderable> &renderable)
{
    // Return immediately if tne renderable is already in the group
    if (std::find(renderables_.begin(), renderables_.end(), renderable) != renderables_.end())
        return;

    renderables_.push_back(renderable);

    // Apply colouring information if necessary
    setRenderableColour(renderable);

    // Apply vertical shift to the renderable if necessary
    setRenderableVerticalShift(renderable.get(), renderables_.size() - 1);

    // Apply line style if necessary
    setRenderableLineStyle(renderable.get());
}

// Remove Renderable from group (if it exists)
void RenderableGroup::removeRenderable(Renderable *renderable)
{
    auto it = std::find_if(renderables_.begin(), renderables_.end(), [&renderable](auto &r) { return r.get() == renderable; });
    if (it == renderables_.end())
    {
        Messenger::warn("Renderable '{}' is not present in the group '{}', so can't remove it.\n", renderable->name(), name());
        return;
    }

    renderables_.erase(it);

    // Remove shift from the renderable first, if one is being applied
    renderable->setValuesTransformEnabled(false);
    renderable->setValuesTransformEquation("value");

    renderable->unSetGroup();
}

// Return whether the group is used by the specified renderable
bool RenderableGroup::usedByRenderable(const Renderable *renderable) const
{
    return std::find_if(renderables_.begin(), renderables_.end(), [&renderable](auto &r) { return r.get() == renderable; }) !=
           renderables_.end();
}

// Return list of Renderables using this group
std::vector<std::shared_ptr<Renderable>> &RenderableGroup::renderables() { return renderables_; }
const std::vector<std::shared_ptr<Renderable>> &RenderableGroup::renderables() const { return renderables_; }

// Return whether the group is empty
bool RenderableGroup::isEmpty() const { return renderables_.empty(); }

// Empty the group, removing all Renderable targets
void RenderableGroup::empty()
{
    for (auto &renderable : renderables_)
        renderable->unSetGroup();

    renderables_.clear();

    // Reset colour counters
    automaticStockColourUsageCount_.fill(0);
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
void RenderableGroup::setRenderableColour(std::shared_ptr<Renderable> renderable)
{
    if (colouringStyle_ == FixedGroupColouring)
        renderable->setColour(fixedStockColour_);
    else if (colouringStyle_ == AutomaticIndividualColouring)
    {
        // Find the StockColour with the lowest usage count
        auto lowestId = 0;
        for (auto colourId = 0; colourId < StockColours::nStockColours; ++colourId)
        {
            if (automaticStockColourUsageCount_[colourId] < automaticStockColourUsageCount_[lowestId])
                lowestId = colourId;
        }

        renderable->setColour((StockColours::StockColour)lowestId);

        ++automaticStockColourUsageCount_[lowestId];
    }
}

// Set all renderable colours
void RenderableGroup::setRenderableColours()
{
    for (auto &renderable : renderables_)
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
void RenderableGroup::setRenderableLineStyle(Renderable *renderable)
{
    if (lineStipple_ != LineStipple::NoStipple)
        renderable->lineStyle().setStipple(lineStipple_);
}

// Set all Renderable line styles
void RenderableGroup::setRenderableLineStyles()
{
    for (auto &renderable : renderables_)
        setRenderableLineStyle(renderable.get());
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
    for (auto &renderable : renderables_)
        setRenderableVerticalShift(renderable.get(), index++);
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
