/*
    *** Base Viewer - I/O
    *** src/gui/viewer_io.cpp
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

#include "base/lineparser.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include "gui/render/renderablefactory.h"
#include "gui/viewer.hui"
#include <QMessageBox>
#include <string.h>

/*
 * Input Block Keywords
 */

// Return enum options for InputBlock
EnumOptions<BaseViewer::InputBlock> BaseViewer::inputBlockKeywords()
{
    static EnumOptionsList BaseViewerInputBlockOptions = EnumOptionsList()
                                                         << EnumOption(BaseViewer::EndSessionBlock, "EndSession")
                                                         << EnumOption(BaseViewer::RenderableBlock, "Renderable", 3)
                                                         << EnumOption(BaseViewer::RenderableGroupBlock, "RenderableGroup", 1)
                                                         << EnumOption(BaseViewer::ViewBlock, "View");

    static EnumOptions<BaseViewer::InputBlock> options("InputBlock", BaseViewerInputBlockOptions);

    return options;
}

// Parse main input blocks through specified parser
bool BaseViewer::parseInputBlocks(LineParser &parser)
{
    // Read line from file and decide what to do with it
    Renderable *renderable;
    RenderableGroup *group;
    auto success = true;
    while (!parser.eofOrBlank())
    {
        if (parser.getArgsDelim(LineParser::SemiColonLineBreaks) != LineParser::Success)
            return false;

        if (!inputBlockKeywords().isValid(parser.argsv(0)))
            return inputBlockKeywords().errorAndPrintValid(parser.argsv(0));
        BaseViewer::InputBlock block = inputBlockKeywords().enumeration(parser.argsv(0));
        if (!inputBlockKeywords().validNArgs(block, parser.nArgs() - 1))
            return false;

        switch (block)
        {
            // End of BaseViewer Input
            case (BaseViewer::EndSessionBlock):
                return true;
                break;
            // Renderable Block
            case (BaseViewer::RenderableBlock):
                // Determine Renderable type
                if (!Renderable::renderableTypes().isValid(parser.argsv(1)))
                    return Renderable::renderableTypes().errorAndPrintValid(parser.argsv(1));

                renderable = createRenderable(Renderable::renderableTypes().enumeration(parser.argsv(1)), parser.argsv(2),
                                              parser.argsv(3));
                if (!renderable)
                    return false;

                success = readRenderableBlock(parser, renderable);
                break;
            // RenderableGroup Block
            case (BaseViewer::RenderableGroupBlock):
                group = groupManager_.createGroup(parser.argsv(1));
                if (!group)
                    return false;

                success = readRenderableGroupBlock(parser, group);
                break;
            // View
            case (BaseViewer::ViewBlock):
                success = readViewBlock(parser);
                break;
            default:
                // Not one of the base blocks - is it a custom block?
                if (!readCustomInputBlock(parser))
                {
                    Messenger::warn("Unrecognised input block: {}\n", parser.argsv(0));
                    return false;
                }
                break;
        }

        // If we have failed for any reason, exit now
        if (!success)
            break;
    }

    return success;
}

/*
 * Axis Keywords
 */

// Return enum options for AxisKeyword
EnumOptions<BaseViewer::AxisKeyword> BaseViewer::axisKeywords()
{
    static EnumOptionsList BaseViewerAxisBlockOptions =
        EnumOptionsList() << EnumOption(BaseViewer::AutoScaleKeyword, "AutoScale", 1)
                          << EnumOption(BaseViewer::AutoTicksKeyword, "AutoTicks", 1)
                          << EnumOption(BaseViewer::EndAxisKeyword, "EndAxis")
                          << EnumOption(BaseViewer::FirstTickKeyword, "FirstTick", 1)
                          << EnumOption(BaseViewer::FractionalPositioningKeyword, "FractionalPositioning", 1)
                          << EnumOption(BaseViewer::GridLinesKeyword, "GridLines", 3)
                          << EnumOption(BaseViewer::GridLineMajorStyleKeyword, "GridLineMajorStyle", 6)
                          << EnumOption(BaseViewer::GridLineMinorStyleKeyword, "GridLineMinorStyle", 6)
                          << EnumOption(BaseViewer::InvertKeyword, "Invert", 1)
                          << EnumOption(BaseViewer::LabelAnchorKeyword, "LabelAnchor", 1)
                          << EnumOption(BaseViewer::LabelOrientationKeyword, "LabelOrientation", 3)
                          << EnumOption(BaseViewer::LimitsKeyword, "Limits", 2)
                          << EnumOption(BaseViewer::LogarithmicKeyword, "Logarithmic", 1)
                          << EnumOption(BaseViewer::MinorTicksKeyword, "MinorTicks", 1)
                          << EnumOption(BaseViewer::NumberFormatKeyword, "NumberFormat", 4)
                          << EnumOption(BaseViewer::PositionFractionalKeyword, "PositionFractional", 3)
                          << EnumOption(BaseViewer::PositionRealKeyword, "PositionReal", 3)
                          << EnumOption(BaseViewer::StretchKeyword, "Stretch", 1)
                          << EnumOption(BaseViewer::TickDeltaKeyword, "TickDelta", 1)
                          << EnumOption(BaseViewer::TickDirectionKeyword, "TickDirection", 3)
                          << EnumOption(BaseViewer::TitleKeyword, "Title", 1)
                          << EnumOption(BaseViewer::TitleAnchorKeyword, "TitleAnchor", 1)
                          << EnumOption(BaseViewer::TitleOrientationKeyword, "TitleOrientation", 5)
                          << EnumOption(BaseViewer::VisibleAxisKeyword, "Visible", 1);

    static EnumOptions<BaseViewer::AxisKeyword> options("AxisKeyword", BaseViewerAxisBlockOptions);

    return options;
}

// Parse AxisBlock keywords
bool BaseViewer::readAxisBlock(LineParser &parser, Axes &axes, int axis, bool strictBlockEnd)
{
    LineStipple::StippleType stipple;
    NumberFormat numberFormat;
    LineStyle lineStyle;

    while (!parser.eofOrBlank())
    {
        // Get line from file
        if (parser.getArgsDelim(LineParser::SemiColonLineBreaks) != LineParser::Success)
            return false;

        // Do we recognise this keyword and, if so, do we have an appropriate number of arguments?
        if (!axisKeywords().isValid(parser.argsv(0)))
            return axisKeywords().errorAndPrintValid(parser.argsv(0));
        auto kwd = axisKeywords().enumeration(parser.argsv(0));
        if (!axisKeywords().validNArgs(kwd, parser.nArgs() - 1))
            return false;

        // All OK, so process the keyword
        switch (kwd)
        {
            // Autoscale method
            case (BaseViewer::AutoScaleKeyword):
                if (Axes::autoScaleMethods().isValid(parser.argsv(1)))
                    axes.setAutoScale(axis, Axes::autoScaleMethods().enumeration(parser.argsv(1)));
                else
                {
                    Messenger::warn("Unrecognised autoscale method '{}'. Defaulting to '{}'.\n", parser.argsv(1),
                                    Axes::autoScaleMethods().keyword(Axes::NoAutoScale));
                    axes.setAutoScale(axis, Axes::NoAutoScale);
                }
                break;
            // Auto ticks
            case (BaseViewer::AutoTicksKeyword):
                axes.setAutoTicks(axis, parser.argb(1));
                break;
            // End input block
            case (BaseViewer::EndAxisKeyword):
                return true;
                break;
            // First ticks
            case (BaseViewer::FirstTickKeyword):
                axes.setFirstTick(axis, parser.argd(1));
                break;
            // Fractional positioning flag
            case (BaseViewer::FractionalPositioningKeyword):
                axes.setPositionIsFractional(axis, parser.argb(1));
                break;
            // GridLines
            case (BaseViewer::GridLinesKeyword):
                axes.setGridLinesMajor(axis, parser.argb(1));
                axes.setGridLinesMinor(axis, parser.argb(2));
                axes.setGridLinesFull(axis, parser.argb(3));
                break;
            // GridLine major style
            case (BaseViewer::GridLineMajorStyleKeyword):
                lineStyle.setWidth(parser.argd(1));
                stipple = LineStipple::stippleType(parser.argsv(2));
                if (stipple == LineStipple::nStippleTypes)
                {
                    Messenger::warn("Unrecognised line stipple type '{}'. Defaulting to 'NoStipple'.\n", parser.argsv(2));
                    stipple = LineStipple::NoStipple;
                    return false;
                }
                lineStyle.setStipple(stipple);
                lineStyle.setColour(parser.argd(3), parser.argd(4), parser.argd(5), parser.argd(6));
                axes.setGridLineMajorStyle(axis, lineStyle);
                break;
            // GridLine minor style
            case (BaseViewer::GridLineMinorStyleKeyword):
                lineStyle.setWidth(parser.argd(1));
                stipple = LineStipple::stippleType(parser.argsv(2));
                if (stipple == LineStipple::nStippleTypes)
                {
                    Messenger::warn("Unrecognised line stipple type '{}'. Defaulting to 'NoStipple'.\n", parser.argsv(2));
                    stipple = LineStipple::NoStipple;
                    return false;
                }
                lineStyle.setStipple(stipple);
                lineStyle.setColour(parser.argd(3), parser.argd(4), parser.argd(5), parser.argd(6));
                axes.setGridLineMinorStyle(axis, lineStyle);
                break;
            // Invert
            case (BaseViewer::InvertKeyword):
                axes.setInverted(axis, parser.argb(1));
                break;
            // Axis label anchor
            case (BaseViewer::LabelAnchorKeyword):
                if (TextPrimitive::textAnchors().isValid(parser.argsv(1)))
                    axes.setLabelAnchor(axis, TextPrimitive::textAnchors().enumeration(parser.argsv(1)));
                else
                {
                    Messenger::warn("Unrecognised text anchor '{}'. Defaulting to '{}'.\n", parser.argsv(1),
                                    TextPrimitive::textAnchors().keyword(TextPrimitive::TopMiddleAnchor));
                    axes.setLabelAnchor(axis, TextPrimitive::TopMiddleAnchor);
                }
                break;
            // Axis label orientation
            case (BaseViewer::LabelOrientationKeyword):
                axes.setLabelOrientation(axis, 0, parser.argd(1));
                axes.setLabelOrientation(axis, 1, parser.argd(2));
                axes.setLabelOrientation(axis, 2, parser.argd(3));
                break;
            // Limits
            case (BaseViewer::LimitsKeyword):
                axes.setMin(axis, parser.argd(1));
                axes.setMax(axis, parser.argd(2));
                break;
            // Axis logarithmic flag
            case (BaseViewer::LogarithmicKeyword):
                axes.setLogarithmic(axis, parser.argb(1));
                break;
            // Axis minor ticks
            case (BaseViewer::MinorTicksKeyword):
                axes.setMinorTicks(axis, parser.argi(1));
                break;
            // Number Format
            case (BaseViewer::NumberFormatKeyword):
                if (NumberFormat::formatTypes().isValid(parser.argsv(1)))
                    numberFormat.setType(NumberFormat::formatTypes().enumeration(parser.argsv(1)));
                else
                {
                    Messenger::warn("Unrecognised number format '{}'. Defaulting to '{}'.\n", parser.argsv(1),
                                    NumberFormat::formatTypes().keyword(NumberFormat::DecimalFormat));
                    numberFormat.setType(NumberFormat::DecimalFormat);
                }

                numberFormat.setNDecimals(parser.argi(2));
                numberFormat.setUseUpperCaseExponent(parser.argb(3));
                numberFormat.setForcePrecedingPlus(parser.argb(4));
                axes.setNumberFormat(axis, numberFormat);
                break;
            // Axis position (in fractional axis coordinates)
            case (BaseViewer::PositionFractionalKeyword):
                axes.setPositionFractional(axis, 0, parser.argd(1));
                axes.setPositionFractional(axis, 1, parser.argd(2));
                axes.setPositionFractional(axis, 2, parser.argd(3));
                break;
            // Axis position (in real surface-space coordinates)
            case (BaseViewer::PositionRealKeyword):
                axes.setPositionReal(axis, 0, parser.argd(1));
                axes.setPositionReal(axis, 1, parser.argd(2));
                axes.setPositionReal(axis, 2, parser.argd(3));
                break;
            // Axis stretch factors
            case (BaseViewer::StretchKeyword):
                axes.setStretch(axis, parser.argd(1));
                break;
            // Axis tick deltas
            case (BaseViewer::TickDeltaKeyword):
                axes.setTickDelta(axis, parser.argd(1));
                break;
            // Axis tick direction
            case (BaseViewer::TickDirectionKeyword):
                axes.setTickDirection(axis, 0, parser.argd(1));
                axes.setTickDirection(axis, 1, parser.argd(2));
                axes.setTickDirection(axis, 2, parser.argd(3));
                break;
            // Axis title
            case (BaseViewer::TitleKeyword):
                axes.setTitle(axis, parser.argsv(1));
                break;
            // Axis title anchor
            case (BaseViewer::TitleAnchorKeyword):
                if (TextPrimitive::textAnchors().isValid(parser.argsv(1)))
                    axes.setTitleAnchor(axis, TextPrimitive::textAnchors().enumeration(parser.argsv(1)));
                else
                {
                    Messenger::warn("Unrecognised text anchor '{}'. Defaulting to '{}'.\n",
                                    TextPrimitive::textAnchors().keyword(TextPrimitive::TopMiddleAnchor));
                    axes.setTitleAnchor(axis, TextPrimitive::TopMiddleAnchor);
                }
                break;
            // Axis title orientation
            case (BaseViewer::TitleOrientationKeyword):
                axes.setTitleOrientationNEW(axis, 0, parser.argd(1));
                axes.setTitleOrientationNEW(axis, 1, parser.argd(2));
                axes.setTitleOrientationNEW(axis, 2, parser.argd(3));
                axes.setTitleDistance(axis, parser.argd(4));
                axes.setTitleHorizontalOffset(axis, parser.argd(5));
                break;
            // Axis visibility
            case (BaseViewer::VisibleAxisKeyword):
                axes.setVisible(axis, parser.argb(1));
                break;
            // Unrecognised keyword
            default:
                Messenger::warn("Unrecognised axis keyword: {}\n", parser.argsv(0));
                return false;
                break;
        }
    }

    if (strictBlockEnd)
    {
        Messenger::print("Error : Unterminated 'Axis' block.\n");
        return false;
    }

    return true;
}

// Write AxisBlock keywords
bool BaseViewer::writeAxisBlock(LineParser &parser, const Axes &axes, int axis) const
{
    if (!parser.writeLineF("  {}  {}\n", BaseViewer::viewKeywords().keyword(BaseViewer::AxisBlockKeyword), axis))
        return false;
    if (!parser.writeLineF("    {}  {}\n", BaseViewer::axisKeywords().keyword(BaseViewer::AutoScaleKeyword),
                           Axes::autoScaleMethods().keyword(axes.autoScale(axis))))
        return false;
    if (!parser.writeLineF("    {}  {}\n", BaseViewer::axisKeywords().keyword(BaseViewer::AutoTicksKeyword),
                           DissolveSys::btoa(axes.autoTicks(axis))))
        return false;
    if (!parser.writeLineF("    {}  {}\n", BaseViewer::axisKeywords().keyword(BaseViewer::FirstTickKeyword),
                           axes.tickFirst(axis)))
        return false;
    if (!parser.writeLineF("    {}  {}\n", BaseViewer::axisKeywords().keyword(BaseViewer::FractionalPositioningKeyword),
                           DissolveSys::btoa(axes.positionIsFractional(axis))))
        return false;
    if (!parser.writeLineF("    {}  {} {} {}\n", BaseViewer::axisKeywords().keyword(BaseViewer::GridLinesKeyword),
                           DissolveSys::btoa(axes.gridLinesMajor(axis)), DissolveSys::btoa(axes.gridLinesMinor(axis)),
                           DissolveSys::btoa(axes.gridLinesFull(axis))))
        return false;
    LineStyle style = axes.gridLineMajorStyle(axis);
    if (!parser.writeLineF("    {}  {} '{}' {} {} {} {}\n",
                           BaseViewer::axisKeywords().keyword(BaseViewer::GridLineMajorStyleKeyword), style.width(),
                           LineStipple::stipple[style.stipple()].name, style.colour()[0], style.colour()[1], style.colour()[2],
                           style.colour()[3]))
        return false;
    style = axes.gridLineMinorStyle(axis);
    if (!parser.writeLineF("    {}  {} '{}' {} {} {} {}\n",
                           BaseViewer::axisKeywords().keyword(BaseViewer::GridLineMinorStyleKeyword), style.width(),
                           LineStipple::stipple[style.stipple()].name, style.colour()[0], style.colour()[1], style.colour()[2],
                           style.colour()[3]))
        return false;
    if (!parser.writeLineF("    {}  {}\n", BaseViewer::axisKeywords().keyword(BaseViewer::InvertKeyword),
                           DissolveSys::btoa(axes.inverted(axis))))
        return false;
    if (!parser.writeLineF("    {}  {}\n", BaseViewer::axisKeywords().keyword(BaseViewer::LabelAnchorKeyword),
                           TextPrimitive::textAnchors().keyword(axes.labelAnchor(axis))))
        return false;
    if (!parser.writeLineF("    {}  {} {} {}\n", BaseViewer::axisKeywords().keyword(BaseViewer::LabelOrientationKeyword),
                           axes.labelOrientation(axis).x, axes.labelOrientation(axis).y, axes.labelOrientation(axis).z))
        return false;
    if (!parser.writeLineF("    {}  {} {}\n", BaseViewer::axisKeywords().keyword(BaseViewer::LimitsKeyword), axes.min(axis),
                           axes.max(axis)))
        return false;
    if (!parser.writeLineF("    {}  {}\n", BaseViewer::axisKeywords().keyword(BaseViewer::LogarithmicKeyword),
                           DissolveSys::btoa(axes.logarithmic(axis))))
        return false;
    if (!parser.writeLineF("    {}  {}\n", BaseViewer::axisKeywords().keyword(BaseViewer::MinorTicksKeyword),
                           axes.minorTicks(axis)))
        return false;
    NumberFormat fmt = axes.numberFormat(axis);
    if (!parser.writeLineF("    {}  '{}' {} {} {}\n", BaseViewer::axisKeywords().keyword(BaseViewer::NumberFormatKeyword),
                           NumberFormat::formatTypes().keyword(fmt.type()), fmt.nDecimals(),
                           DissolveSys::btoa(fmt.useUpperCaseExponent()), DissolveSys::btoa(fmt.forcePrecedingPlus())))
        return false;
    if (!parser.writeLineF("    {}  {} {} {}\n", BaseViewer::axisKeywords().keyword(BaseViewer::PositionFractionalKeyword),
                           axes.positionFractional(axis).x, axes.positionFractional(axis).y, axes.positionFractional(axis).z))
        return false;
    if (!parser.writeLineF("    {}  {} {} {}\n", BaseViewer::axisKeywords().keyword(BaseViewer::PositionRealKeyword),
                           axes.positionReal(axis).x, axes.positionReal(axis).y, axes.positionReal(axis).z))
        return false;
    if (!parser.writeLineF("    {}  {}\n", BaseViewer::axisKeywords().keyword(BaseViewer::StretchKeyword), axes.stretch(axis)))
        return false;
    if (!parser.writeLineF("    {}  {}\n", BaseViewer::axisKeywords().keyword(BaseViewer::TickDeltaKeyword),
                           axes.tickDelta(axis)))
        return false;
    if (!parser.writeLineF("    {}  {} {} {}\n", BaseViewer::axisKeywords().keyword(BaseViewer::TickDirectionKeyword),
                           axes.tickDirection(axis).x, axes.tickDirection(axis).y, axes.tickDirection(axis).z))
        return false;
    if (!parser.writeLineF("    {}  {}\n", BaseViewer::axisKeywords().keyword(BaseViewer::TitleAnchorKeyword),
                           TextPrimitive::textAnchors().keyword(axes.titleAnchor(axis))))
        return false;
    if (!parser.writeLineF("    {}  '{}'\n", BaseViewer::axisKeywords().keyword(BaseViewer::TitleKeyword), axes.title(axis)))
        return false;
    if (!parser.writeLineF("    {}  {} {} {} {} {}\n", BaseViewer::axisKeywords().keyword(BaseViewer::TitleOrientationKeyword),
                           axes.titleOrientation(axis).x, axes.titleOrientation(axis).y, axes.titleOrientation(axis).z,
                           axes.titleDistance(axis), axes.titleHorizontalOffset(axis)))
        return false;
    if (!parser.writeLineF("    {}  {}\n", BaseViewer::axisKeywords().keyword(BaseViewer::VisibleAxisKeyword),
                           DissolveSys::btoa(axes.visible(axis))))
        return false;
    if (!parser.writeLineF("  {}\n", BaseViewer::axisKeywords().keyword(BaseViewer::EndAxisKeyword)))
        return false;

    return true;
}

/*
 * Renderable Keywords
 */

// Return enum options for RenderableKeyword
EnumOptions<BaseViewer::RenderableKeyword> BaseViewer::renderableKeywords()
{
    static EnumOptionsList BaseViewerRenderableBlockOptions =
        EnumOptionsList() << EnumOption(BaseViewer::ColourAlphaIsGlobalKeyword, "ColourAlphaIsGlobal", 1)
                          << EnumOption(BaseViewer::ColourCustomGradientKeyword, "ColourCustom", 5)
                          << EnumOption(BaseViewer::ColourGlobalAlphaKeyword, "ColourGlobalAlpha", 1)
                          << EnumOption(BaseViewer::ColourHSVGradientEndKeyword, "ColourHSVEnd", 5)
                          << EnumOption(BaseViewer::ColourHSVGradientStartKeyword, "ColourHSVStart", 5)
                          << EnumOption(BaseViewer::ColourRGBGradientEndKeyword, "ColourRGBEnd", 5)
                          << EnumOption(BaseViewer::ColourRGBGradientStartKeyword, "ColourRGBStart", 5)
                          << EnumOption(BaseViewer::ColourSingleKeyword, "ColourSingle", 4)
                          << EnumOption(BaseViewer::ColourStyleKeyword, "ColourStyle", 1)
                          << EnumOption(BaseViewer::EndRenderableKeyword, "EndRenderable")
                          << EnumOption(BaseViewer::EndStyleKeyword, "EndStyle")
                          << EnumOption(BaseViewer::GroupKeyword, "Group", 1)
                          << EnumOption(BaseViewer::LineStyleKeyword, "LineStyle", 2)
                          << EnumOption(BaseViewer::StyleKeyword, "Style")
                          << EnumOption(BaseViewer::TransformValuesKeyword, "TransformValues", 2)
                          << EnumOption(BaseViewer::VisibleKeyword, "Visible", 1);

    static EnumOptions<BaseViewer::RenderableKeyword> options("RenderableKeyword", BaseViewerRenderableBlockOptions);

    return options;
}

// Read RenderableBlock keywords
bool BaseViewer::readRenderableBlock(LineParser &parser, Renderable *renderable, bool strictBlockEnd)
{
    double alpha;
    ColourDefinition &colourDefinition = renderable->colour();
    QColor hsvColour;
    LineStipple::StippleType stipple;

    while (!parser.eofOrBlank())
    {
        // Get line from file
        if (parser.getArgsDelim(LineParser::SemiColonLineBreaks) != LineParser::Success)
            return false;

        // Do we recognise this keyword and, if so, do we have an appropriate number of arguments?
        if (!renderableKeywords().isValid(parser.argsv(0)))
            return renderableKeywords().errorAndPrintValid(parser.argsv(0));
        auto kwd = renderableKeywords().enumeration(parser.argsv(0));
        if (!renderableKeywords().validNArgs(kwd, parser.nArgs() - 1))
            return false;

        // All OK, so process the keyword
        switch (kwd)
        {
            // Colour alpha control
            case (BaseViewer::ColourAlphaIsGlobalKeyword):
                colourDefinition.setUseGlobalAlpha(parser.argb(1));
                break;
            // Colour Custom Gradient point definition
            case (BaseViewer::ColourCustomGradientKeyword):
                colourDefinition.addCustomGradientPoint(parser.argd(1),
                                                        QColor(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)));
                break;
            // Colour alpha fixed value
            case (BaseViewer::ColourGlobalAlphaKeyword):
                alpha = parser.argd(1);
                if ((alpha < 0.0) || (alpha > 1.0))
                {
                    Messenger::warn("Alpha value ({}) is out of range for {} keyword - it will be reset to 1.0.\n", alpha,
                                    renderableKeywords().keyword(kwd));
                    alpha = 1.0;
                }
                colourDefinition.setGlobalAlpha(alpha);
                break;
            // Colour HSV Gradient end point definition
            case (BaseViewer::ColourHSVGradientEndKeyword):
                hsvColour.setHsv(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5));
                colourDefinition.setHSVGradientEnd(parser.argd(1), hsvColour);
                break;
            // Colour HSV Gradient start point definition
            case (BaseViewer::ColourHSVGradientStartKeyword):
                hsvColour.setHsv(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5));
                colourDefinition.setHSVGradientStart(parser.argd(1), hsvColour);
                break;
            // Colour RGB Gradient end point definition
            case (BaseViewer::ColourRGBGradientEndKeyword):
                colourDefinition.setRGBGradientEnd(parser.argd(1),
                                                   QColor(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)));
                break;
            // Colour RGB Gradient start point definition
            case (BaseViewer::ColourRGBGradientStartKeyword):
                colourDefinition.setRGBGradientStart(parser.argd(1),
                                                     QColor(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)));
                break;
            // Colour single colour definition
            case (BaseViewer::ColourSingleKeyword):
                colourDefinition.setSingleColour(QColor(parser.argi(1), parser.argi(2), parser.argi(3), parser.argi(4)));
                break;
            // Colour style
            case (BaseViewer::ColourStyleKeyword):
                if (ColourDefinition::colourStyles().isValid(parser.argsv(1)))
                    colourDefinition.setStyle(ColourDefinition::colourStyles().enumeration(parser.argsv(1)));
                else
                {
                    Messenger::warn("Unrecognised colour style '{}'. Defaulting to '{}'.\n", parser.argsv(1),
                                    ColourDefinition::colourStyles().keyword(ColourDefinition::SingleColourStyle));
                    colourDefinition.setStyle(ColourDefinition::SingleColourStyle);
                }
                break;
            // End input block
            case (BaseViewer::EndRenderableKeyword):
                return true;
                break;
            // Group
            case (BaseViewer::GroupKeyword):
                groupManager_.addToGroup(renderable, parser.argsv(1));
                break;
            // Line style
            case (BaseViewer::LineStyleKeyword):
                renderable->lineStyle().setWidth(parser.argd(1));
                stipple = LineStipple::stippleType(parser.argsv(2));
                if (stipple == LineStipple::nStippleTypes)
                {
                    Messenger::warn("Unrecognised line stipple type '{}'. Defaulting to 'NoStipple'.\n", parser.argsv(2));
                    stipple = LineStipple::NoStipple;
                }
                renderable->lineStyle().setStipple(stipple);
                break;
            // Display style
            case (BaseViewer::StyleKeyword):
                if (!renderable->readStyleBlock(parser))
                    return false;
                break;
            // Transforms
            case (BaseViewer::TransformValuesKeyword):
                renderable->setValuesTransformEnabled(parser.argb(1));
                renderable->setValuesTransformEquation(parser.argsv(2));
                break;
            // Visible flag
            case (BaseViewer::VisibleKeyword):
                renderable->setVisible(parser.argb(1));
                break;
            // Unrecognised Keyword
            default:
                Messenger::warn("Unrecognised renderable keyword: {}\n", parser.argsv(0));
                return false;
                break;
        }
    }

    if (strictBlockEnd)
    {
        Messenger::print("Error : Unterminated 'Renderable' block.\n");
        return false;
    }

    return true;
}

// Write RenderableBlock keywords
bool BaseViewer::writeRenderableBlock(LineParser &parser, Renderable *renderable, int indentLevel) const
{
    // Construct indent string
    char *indent = new char[indentLevel * 2 + 1];
    for (int n = 0; n < indentLevel * 2; ++n)
        indent[n] = ' ';
    indent[indentLevel * 2] = '\0';

    if (!parser.writeLineF(
            "{}{}  {}  '{}'  '{}'\n", indent, BaseViewer::inputBlockKeywords().keyword(BaseViewer::RenderableBlock),
            Renderable::renderableTypes().keyword(renderable->type()), renderable->objectTag(), renderable->name()))
        return false;

    // -- Valus Transform
    if (!parser.writeLineF("{}  {}  {} {}\n", indent,
                           BaseViewer::renderableKeywords().keyword(BaseViewer::TransformValuesKeyword),
                           DissolveSys::btoa(renderable->valuesTransformEnabled()), renderable->valuesTransformEquation()))
        return false;

    // Colour Setup
    const ColourDefinition &colourDef = renderable->colour();
    if (!parser.writeLineF("{}  {}  '{}'\n", indent, BaseViewer::renderableKeywords().keyword(BaseViewer::ColourStyleKeyword),
                           ColourDefinition::colourStyles().keyword(colourDef.style())))
        return false;
    QColor colour;
    double value;
    // -- Single Colour
    colour = colourDef.singleColour();
    if (!parser.writeLineF("{}  {}  {} {} {} {}\n", indent,
                           BaseViewer::renderableKeywords().keyword(BaseViewer::ColourSingleKeyword), colour.red(),
                           colour.green(), colour.blue(), colour.alpha()))
        return false;
    // -- RGB Gradient
    colour = colourDef.rgbGradientStartColour();
    value = colourDef.rgbGradientStartValue();
    if (!parser.writeLineF("{}  {}  {} {} {} {} {}\n", indent,
                           BaseViewer::renderableKeywords().keyword(BaseViewer::ColourRGBGradientStartKeyword), value,
                           colour.red(), colour.green(), colour.blue(), colour.alpha()))
        return false;
    colour = colourDef.rgbGradientEndColour();
    value = colourDef.rgbGradientEndValue();
    if (!parser.writeLineF("{}  {}  {} {} {} {} {}\n", indent,
                           BaseViewer::renderableKeywords().keyword(BaseViewer::ColourRGBGradientEndKeyword), value,
                           colour.red(), colour.green(), colour.blue(), colour.alpha()))
        return false;
    // -- HSV Gradient
    colour = colourDef.hsvGradientStartColour();
    value = colourDef.hsvGradientStartValue();
    if (!parser.writeLineF("{}  {}  {} {} {} {} {}\n", indent,
                           BaseViewer::renderableKeywords().keyword(BaseViewer::ColourHSVGradientStartKeyword), value,
                           colour.hue(), colour.saturation(), colour.value(), colour.alpha()))
        return false;
    colour = colourDef.hsvGradientEndColour();
    value = colourDef.hsvGradientEndValue();
    if (!parser.writeLineF("{}  {}  {} {} {} {} {}\n", indent,
                           BaseViewer::renderableKeywords().keyword(BaseViewer::ColourHSVGradientEndKeyword), value,
                           colour.hue(), colour.saturation(), colour.value(), colour.alpha()))
        return false;
    // -- Custom Gradient
    const Array<ColourScalePoint> customGradient = colourDef.customGradientPoints();
    for (int n = 0; n < customGradient.nItems(); ++n)
    {
        const ColourScalePoint &point = customGradient.constAt(n);
        if (!parser.writeLineF("{}  {}  {} {} {} {} {}\n", indent,
                               BaseViewer::renderableKeywords().keyword(BaseViewer::ColourCustomGradientKeyword), point.value(),
                               point.colour().red(), point.colour().green(), point.colour().blue(), point.colour().alpha()))
            return false;
    }
    // -- Alpha control
    if (!parser.writeLineF("{}  {}  {}\n", indent,
                           BaseViewer::renderableKeywords().keyword(BaseViewer::ColourAlphaIsGlobalKeyword),
                           DissolveSys::btoa(colourDef.useGlobalAlpha())))
        return false;
    if (!parser.writeLineF("{}  {}  {}\n", indent,
                           BaseViewer::renderableKeywords().keyword(BaseViewer::ColourGlobalAlphaKeyword),
                           colourDef.globalAlpha()))
        return false;

    // Display
    if (!parser.writeLineF("{}  {}  {} '{}'\n", indent, BaseViewer::renderableKeywords().keyword(BaseViewer::LineStyleKeyword),
                           renderable->lineStyle().width(), LineStipple::stipple[renderable->lineStyle().stipple()].name))
        return false;
    if (!parser.writeLineF("{}  {}\n", indent, BaseViewer::renderableKeywords().keyword(BaseViewer::StyleKeyword)))
        return false;
    if (!renderable->writeStyleBlock(parser, indentLevel + 2))
        return false;
    if (!parser.writeLineF("{}  {}\n", indent, BaseViewer::renderableKeywords().keyword(BaseViewer::EndStyleKeyword)))
        return false;
    if (!parser.writeLineF("{}  {}  {}\n", indent, BaseViewer::renderableKeywords().keyword(BaseViewer::VisibleKeyword),
                           DissolveSys::btoa(renderable->isVisible())))
        return false;

    // Write Group if set
    if (renderable->group())
        parser.writeLineF("{}  {} '{}'\n", indent, BaseViewer::renderableKeywords().keyword(BaseViewer::GroupKeyword),
                          renderable->group()->name());

    if (!parser.writeLineF("{}{}\n", indent, BaseViewer::renderableKeywords().keyword(BaseViewer::EndRenderableKeyword)))
        return false;

    return true;
}

/*
 * RenderableGroup Keywords
 */

// Return enum options for RenderableGroupKeyword
EnumOptions<BaseViewer::RenderableGroupKeyword> BaseViewer::renderableGroupKeywords()
{
    static EnumOptionsList BaseViewerRenderableGroupBlockOptions =
        EnumOptionsList() << EnumOption(BaseViewer::ColouringStyleKeyword, "ColouringStyle", 1)
                          << EnumOption(BaseViewer::EndRenderableGroupKeyword, "EndRenderableGroup")
                          << EnumOption(BaseViewer::FixedStockColourKeyword, "FixedStockColour", 1)
                          << EnumOption(BaseViewer::GroupVisibleKeyword, "Visible", 1)
                          << EnumOption(BaseViewer::StippleKeyword, "Stipple", 1)
                          << EnumOption(BaseViewer::VerticalShiftingKeyword, "VerticalShifting", 1);

    static EnumOptions<BaseViewer::RenderableGroupKeyword> options("RenderableGroupKeyword",
                                                                   BaseViewerRenderableGroupBlockOptions);

    return options;
}

// Read RenderableGroupBlock keywords
bool BaseViewer::readRenderableGroupBlock(LineParser &parser, RenderableGroup *group, bool strictBlockEnd)
{
    LineStipple::StippleType stipple;
    while (!parser.eofOrBlank())
    {
        // Get line from file
        if (parser.getArgsDelim(LineParser::SemiColonLineBreaks) != LineParser::Success)
            return false;

        // Do we recognise this keyword and, if so, do we have an appropriate number of arguments?
        // Do we recognise this keyword and, if so, do we have the appropriate number of arguments?
        if (!renderableGroupKeywords().isValid(parser.argsv(0)))
            return renderableGroupKeywords().errorAndPrintValid(parser.argsv(0));
        auto kwd = renderableGroupKeywords().enumeration(parser.argsv(0));
        if (!renderableGroupKeywords().validNArgs(kwd, parser.nArgs() - 1))
            return false;

        // All OK, so process the keyword
        switch (kwd)
        {
            // Colouring Style
            case (BaseViewer::ColouringStyleKeyword):
                if (!RenderableGroup::groupColourings().isValid(parser.argsv(1)))
                    return RenderableGroup::groupColourings().errorAndPrintValid(parser.argsv(1));
                group->setColouringStyle(RenderableGroup::groupColourings().enumeration(parser.argsv(1)));
                break;
            // End input block
            case (BaseViewer::EndRenderableGroupKeyword):
                return true;
                break;
            // Fixed stock colour
            case (BaseViewer::FixedStockColourKeyword):
                if (!StockColours::stockColours().isValid(parser.argsv(1)))
                    return StockColours::stockColours().errorAndPrintValid(parser.argsv(1));
                group->setFixedStockColour(StockColours::stockColours().enumeration(parser.argsv(1)));
                break;
            // Group visibility flag
            case (BaseViewer::GroupVisibleKeyword):
                group->setVisible(parser.argb(1));
                break;
            // Stipple type
            case (BaseViewer::StippleKeyword):
                stipple = LineStipple::stippleType(parser.argsv(1));
                if (stipple == LineStipple::nStippleTypes)
                    return false;
                group->setLineStipple(stipple);
                break;
            // Vertical Shift Style
            case (BaseViewer::VerticalShiftingKeyword):
                if (!RenderableGroup::verticalShiftStyles().isValid(parser.argsv(1)))
                    return RenderableGroup::verticalShiftStyles().errorAndPrintValid(parser.argsv(1));
                group->setVerticalShiftStyle(RenderableGroup::verticalShiftStyles().enumeration(parser.argsv(1)));
                break;
            // Unrecognised Keyword
            default:
                Messenger::error("DEV_OOPS - {} block keyword '{}' not accounted for.\n",
                                 BaseViewer::inputBlockKeywords().keyword(BaseViewer::RenderableBlock),
                                 renderableGroupKeywords().keyword(kwd));
                return false;
                break;
        }
    }

    if (strictBlockEnd)
    {
        Messenger::print("Error : Unterminated 'RenderableGroup' block.\n");
        return false;
    }

    return true;
}

// Write RenderableGroupBlock keywords
bool BaseViewer::writeRenderableGroupBlock(LineParser &parser, RenderableGroup *group, int indentLevel) const
{
    // Construct indent string
    char *indent = new char[indentLevel * 2 + 1];
    for (int n = 0; n < indentLevel * 2; ++n)
        indent[n] = ' ';
    indent[indentLevel * 2] = '\0';

    if (!parser.writeLineF("{}{}  '{}'\n", indent, BaseViewer::inputBlockKeywords().keyword(BaseViewer::RenderableGroupBlock),
                           group->name()))
        return false;

    if (!parser.writeLineF("{}  {}  {}\n", indent, renderableGroupKeywords().keyword(BaseViewer::ColouringStyleKeyword),
                           RenderableGroup::groupColourings().keyword(group->colouringStyle())))
        return false;
    if (!parser.writeLineF("{}  {}  {}\n", indent, renderableGroupKeywords().keyword(BaseViewer::FixedStockColourKeyword),
                           StockColours::stockColours().keyword(group->fixedStockColour())))
        return false;
    if (!parser.writeLineF("{}  {}  '{}'\n", indent, renderableGroupKeywords().keyword(BaseViewer::StippleKeyword),
                           LineStipple::stippleType(group->lineStipple())))
        return false;
    if (!parser.writeLineF("{}  {}  {}\n", indent, renderableGroupKeywords().keyword(BaseViewer::VerticalShiftingKeyword),
                           RenderableGroup::verticalShiftStyles().keyword(group->verticalShiftStyle())))
        return false;
    if (!parser.writeLineF("{}  {}  {}\n", indent, renderableGroupKeywords().keyword(BaseViewer::GroupVisibleKeyword),
                           DissolveSys::btoa(group->isVisible())))
        return false;

    if (!parser.writeLineF("{}{}\n", indent, renderableGroupKeywords().keyword(BaseViewer::EndRenderableGroupKeyword)))
        return false;

    return true;
}

/*
 * View Keywords
 */

// Return enum options for ViewKeyword
EnumOptions<BaseViewer::ViewKeyword> BaseViewer::viewKeywords()
{
    static EnumOptionsList ViewKeywords = EnumOptionsList()
                                          << EnumOption(BaseViewer::AutoFollowTypeKeyword, "AutoFollowType", 1)
                                          << EnumOption(BaseViewer::AutoPositionTitlesKeyword, "AutoPositionTitles", 1)
                                          << EnumOption(BaseViewer::AxisBlockKeyword, "Axis", 1)
                                          << EnumOption(BaseViewer::BoundingBoxKeyword, "BoundingBox", 1)
                                          << EnumOption(BaseViewer::BoundingBoxPlaneYKeyword, "BoundingBoxPlaneY", 1)
                                          << EnumOption(BaseViewer::EndViewKeyword, "EndView")
                                          << EnumOption(BaseViewer::FlatLabelsKeyword, "FlatLabels", 1)
                                          << EnumOption(BaseViewer::LabelPointSizeKeyword, "LabelPointSize", 1)
                                          << EnumOption(BaseViewer::PerspectiveKeyword, "Perspective", 1)
                                          << EnumOption(BaseViewer::RotationXKeyword, "RotationX", 3)
                                          << EnumOption(BaseViewer::RotationYKeyword, "RotationY", 3)
                                          << EnumOption(BaseViewer::RotationZKeyword, "RotationZ", 3)
                                          << EnumOption(BaseViewer::TitlePointSizeKeyword, "TitlePointSize", 1)
                                          << EnumOption(BaseViewer::TranslationKeyword, "Translation", 3)
                                          << EnumOption(BaseViewer::UseBestFlatViewKeyword, "UseBestFlatView", 1)
                                          << EnumOption(BaseViewer::VerticalShiftKeyword, "VerticalShift", 1)
                                          << EnumOption(BaseViewer::ViewTypeKeyword, "ViewType", 1);

    static EnumOptions<BaseViewer::ViewKeyword> options("ViewKeyword", ViewKeywords);

    return options;
}

// Read ViewBlock keywords
bool BaseViewer::readViewBlock(LineParser &parser, bool strictBlockEnd)
{
    int xyz, axis;
    Matrix4 mat;

    while (!parser.eofOrBlank())
    {
        // Get line from file
        if (parser.getArgsDelim(LineParser::SemiColonLineBreaks) != LineParser::Success)
            return false;

        // Do we recognise this keyword and, if so, do we have an appropriate number of arguments?
        if (!viewKeywords().isValid(parser.argsv(0)))
            return viewKeywords().errorAndPrintValid(parser.argsv(0));
        auto kwd = viewKeywords().enumeration(parser.argsv(0));
        if (!viewKeywords().validNArgs(kwd, parser.nArgs() - 1))
            return false;

        // All OK, so process the keyword
        switch (kwd)
        {
            // Auto-follow type
            case (BaseViewer::AutoFollowTypeKeyword):
                if (View::autoFollowTypes().isValid(parser.argsv(1)))
                    view_.setAutoFollowType(View::autoFollowTypes().enumeration(parser.argsv(1)));
                else
                {
                    Messenger::warn("Unrecognised auto-follow type '{}'. Defaulting to '{}'.\n", parser.argsv(1),
                                    View::autoFollowTypes().keyword(View::NoAutoFollow));
                    view_.setAutoFollowType(View::NoAutoFollow);
                }
                break;
            // Auto Position Axis Titles
            case (BaseViewer::AutoPositionTitlesKeyword):
                view_.axes().setAutoPositionTitles(parser.argb(1));
                break;
            // Axis block
            case (BaseViewer::AxisBlockKeyword):
                // Get target axis...
                axis = parser.argi(1);
                if ((axis < 0) || (axis > 2))
                    Messenger::warn("Axis index is out of range in input file. Defaults will be used.\n");
                else if (!readAxisBlock(parser, view_.axes(), axis))
                    return false;
                break;
            // End input block
            case (BaseViewer::EndViewKeyword):
                return true;
                break;
            // Flat labels flag
            case (BaseViewer::FlatLabelsKeyword):
                view_.setFlatLabelsIn3D(parser.argb(1));
                break;
            // Label scale
            case (BaseViewer::LabelPointSizeKeyword):
                view_.setLabelPointSize(parser.argd(1));
                break;
            // Rotation
            case (BaseViewer::RotationXKeyword):
            case (BaseViewer::RotationYKeyword):
            case (BaseViewer::RotationZKeyword):
                xyz = kwd - BaseViewer::RotationXKeyword;
                view_.setViewRotationColumn(xyz, parser.argd(1), parser.argd(2), parser.argd(3));
                break;
            // Perspective
            case (BaseViewer::PerspectiveKeyword):
                view_.setHasPerspective(parser.argb(1));
                break;
            // Title scale
            case (BaseViewer::TitlePointSizeKeyword):
                view_.setTitlePointSize(parser.argd(1));
                break;
            // Translation
            case (BaseViewer::TranslationKeyword):
                view_.setViewTranslation(parser.argd(1), parser.argd(2), parser.argd(3));
                break;
            // Use best flat view
            case (BaseViewer::UseBestFlatViewKeyword):
                view_.axes().setUseBestFlatView(parser.argb(1));
                break;
            // Vertical shift (renderable group manager)
            case (BaseViewer::VerticalShiftKeyword):
                groupManager_.setVerticalShiftAmount((RenderableGroupManager::VerticalShiftAmount)parser.argi(1));
                break;
            // View Type
            case (BaseViewer::ViewTypeKeyword):
                if (View::viewTypes().isValid(parser.argsv(1)))
                    view_.setViewType(View::viewTypes().enumeration(parser.argsv(1)));
                else
                {
                    Messenger::warn("Unrecognised view type '{}'. Defaulting to '{}'.\n", parser.argsv(1),
                                    View::viewTypes().keyword(View::NormalView));
                    view_.setViewType(View::NormalView);
                }
                break;
            // Unrecognised Keyword
            default:
                Messenger::warn("Unrecognised view keyword: {}\n", parser.argsv(0));
                return false;
                break;
        }
    }

    if (strictBlockEnd)
    {
        Messenger::print("Error : Unterminated 'View' block.\n");
        return false;
    }

    return true;
}

// Write ViewBlock keywords
bool BaseViewer::writeViewBlock(LineParser &parser) const
{
    if (!parser.writeLineF("{}\n", BaseViewer::inputBlockKeywords().keyword(BaseViewer::ViewBlock)))
        return false;
    if (!parser.writeLineF("  {}  {}\n", BaseViewer::viewKeywords().keyword(BaseViewer::AutoFollowTypeKeyword),
                           View::autoFollowTypes().keyword(view_.autoFollowType())))
        return false;
    if (!parser.writeLineF("  {}  {}\n", BaseViewer::viewKeywords().keyword(BaseViewer::AutoPositionTitlesKeyword),
                           DissolveSys::btoa(view_.constAxes().autoPositionTitles())))
        return false;
    for (int axis = 0; axis < 3; ++axis)
        writeAxisBlock(parser, view_.constAxes(), axis);
    if (!parser.writeLineF("  {}  {}\n", BaseViewer::viewKeywords().keyword(BaseViewer::FlatLabelsKeyword),
                           DissolveSys::btoa(view_.flatLabelsIn3D())))
        return false;
    if (!parser.writeLineF("  {}  {}\n", BaseViewer::viewKeywords().keyword(BaseViewer::LabelPointSizeKeyword),
                           view_.labelPointSize()))
        return false;
    if (!parser.writeLineF("  {}  {}\n", BaseViewer::viewKeywords().keyword(BaseViewer::TitlePointSizeKeyword),
                           view_.titlePointSize()))
        return false;
    Matrix4 mat = view_.viewRotation();
    auto trans = view_.viewTranslation();
    if (!parser.writeLineF("  {}  {} {} {}\n", BaseViewer::viewKeywords().keyword(BaseViewer::RotationXKeyword), mat[0], mat[1],
                           mat[2]))
        return false;
    if (!parser.writeLineF("  {}  {} {} {}\n", BaseViewer::viewKeywords().keyword(BaseViewer::RotationYKeyword), mat[4], mat[5],
                           mat[6]))
        return false;
    if (!parser.writeLineF("  {}  {} {} {}\n", BaseViewer::viewKeywords().keyword(BaseViewer::RotationZKeyword), mat[8], mat[9],
                           mat[10]))
        return false;
    if (!parser.writeLineF("  {}  {} {} {}\n", BaseViewer::viewKeywords().keyword(BaseViewer::TranslationKeyword), trans.x,
                           trans.y, trans.z))
        return false;
    if (!parser.writeLineF("  {}  {}\n", BaseViewer::viewKeywords().keyword(BaseViewer::PerspectiveKeyword),
                           DissolveSys::btoa(view_.hasPerspective())))
        return false;
    if (!parser.writeLineF("  {}  {}\n", BaseViewer::viewKeywords().keyword(BaseViewer::UseBestFlatViewKeyword),
                           DissolveSys::btoa(view_.constAxes().useBestFlatView())))
        return false;
    if (!parser.writeLineF("  {}  {}\n", BaseViewer::viewKeywords().keyword(BaseViewer::VerticalShiftKeyword),
                           groupManager_.verticalShiftAmount()))
        return false;
    if (!parser.writeLineF("  {}  '{}'\n", BaseViewer::viewKeywords().keyword(BaseViewer::ViewTypeKeyword),
                           View::viewTypes().keyword(view_.viewType())))
        return false;
    if (!parser.writeLineF("{}\n", BaseViewer::viewKeywords().keyword(BaseViewer::EndViewKeyword)))
        return false;

    return true;
}

/*
 * Session I/O
 */

// Read custom input block through specified LineParser
bool BaseViewer::readCustomInputBlock(LineParser &parser) { return false; }

// Write custom input blocks through specified LineParser
bool BaseViewer::writeCustomInputBlocks(LineParser &parser) const { return true; }

// Read session through parser specified
bool BaseViewer::readSession(LineParser &parser)
{
    // Clear existing data
    clear();

    // Parse input blocks
    auto success = parseInputBlocks(parser);

    // Show a message if we encountered problems...
    if (!success)
        Messenger::error("Errors were encountered while reading the session data.\n");

    return success;
}

// Write session through parser specified
bool BaseViewer::writeSession(LineParser &parser) const
{
    // Write RenderableGroup data
    ListIterator<RenderableGroup> groupsIterator(groupManager_.groups());
    while (RenderableGroup *group = groupsIterator.iterate())
        if (!writeRenderableGroupBlock(parser, group))
            return false;

    // Write Renderable data
    ListIterator<Renderable> renderablesIterator(renderables_);
    while (Renderable *renderable = renderablesIterator.iterate())
        if (!writeRenderableBlock(parser, renderable))
            return false;

    // Write View Data
    if (!writeViewBlock(parser))
        return false;

    // Write end of session marker
    if (!parser.writeLineF("EndSession\n"))
        return false;

    return true;
}
