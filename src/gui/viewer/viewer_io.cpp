/*
	*** Base Viewer - I/O
	*** src/gui/viewer/viewer_io.cpp
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

#include "gui/viewer/viewer.hui"
#include "gui/viewer/render/renderablefactory.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "base/messenger.h"
#include <string.h>
#include <QMessageBox>

/*
 * Input Block Keywords
 */

// Input File Block Keywords
const char* BaseViewerInputBlockKeywords[] = { "EndSession", "Renderable", "View" };

// Convert text string to InputBlock
BaseViewer::InputBlock BaseViewer::inputBlock(const char* s)
{
	for (int n=0; n<BaseViewer::nInputBlocks; ++n) if (DissolveSys::sameString(s, BaseViewerInputBlockKeywords[n])) return (BaseViewer::InputBlock) n;
	return BaseViewer::nInputBlocks;
}

// Convert InputBlock to text string
const char* BaseViewer::inputBlock(BaseViewer::InputBlock id)
{
	return BaseViewerInputBlockKeywords[id];
}

// Parse main input blocks through specified parser
bool BaseViewer::parseInputBlocks(LineParser& parser)
{
	// Read line from file and decide what to do with it
	BaseViewer::InputBlock block;
	Renderable* renderable;
	Renderable::RenderableType rt;
	bool success = true;
	while (!parser.eofOrBlank())
	{
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks + LineParser::SemiColonLineBreaks);

		// We expect a block keyword in this loop...
		block = BaseViewer::inputBlock(parser.argc(0));
		switch (block)
		{
			// End of BaseViewer Input
			case (BaseViewer::EndSessionBlock):
				return true;
				break;
			// Renderable Block
			case (BaseViewer::RenderableBlock):
				// Check that two arguments have been given (type and object tag)
				if (!parser.hasArg(2)) return Messenger::error("Type and object tag for Renderable expected, but none found.\n");

				// Determine Renderable type
				rt = Renderable::renderableType(parser.argc(1));
				if (rt == Renderable::nRenderableTypes) return Messenger::error("Unknown Renderable type '%s' found.\n", parser.argc(1));

				renderable = createRenderable(rt, parser.argc(2), parser.argc(3), parser.argc(4));
				if (!renderable) return false;

				success = readRenderableBlock(parser, renderable);
				break;
			// View
			case (BaseViewer::ViewBlock):
				success = readViewBlock(parser);
				break;
			default:
				// Not one of the base blocks - is it a custom block?
				if (!readCustomInputBlock(parser))
				{
					Messenger::warn("Unrecognised input block: %s\n", parser.argc(0));
					return false;
				}
				break;
		}

		// If we have failed for any reason, exit now
		if (!success) break;
	}

	return success;
}


/*
 * Axis Keywords
 */

// Axis Block Keywords
const char* BaseViewerAxisBlockKeywords[] = {
	"AutoScale", "AutoTicks",
	"EndAxis",
	"FirstTick", "FractionalPositioning",
	"GridLines", "GridLineMajorStyle", "GridLineMinorStyle",
	"Invert",
	"LabelAnchor", "LabelOrientation", "Limits", "Logarithmic",
	"MinorTicks",
	"NumberFormat",
	"PositionFractional", "PositionReal",
	"Stretch",
	"TickDelta", "TickDirection", "Title", "TitleAnchor", "TitleOrientation",
	"Visible"
};

// Axis Block NArguments
int BaseViewerAxisKeywordNArguments[] = {
	1, 1,
	0,
	1, 1,
	3, 6, 6,
	1,
	1, 3, 2, 1,
	1,
	4,
	3, 3,
	1,
	1, 3, 1, 1, 4,
	1
};

// Convert text string to AxisKeyword
BaseViewer::AxisKeyword BaseViewer::axisKeyword(const char* s)
{
	for (int n=0; n<nAxisKeywords; ++n) if (DissolveSys::sameString(s, BaseViewerAxisBlockKeywords[n])) return (BaseViewer::AxisKeyword) n;
	return nAxisKeywords;
}

// Convert BaseViewerAxisBlockKeywords to text string
const char* BaseViewer::axisKeyword(BaseViewer::AxisKeyword kwd)
{
	return BaseViewerAxisBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int BaseViewer::axisKeywordNArguments(BaseViewer::AxisKeyword kwd)
{
	return BaseViewerAxisKeywordNArguments[kwd];
}

// Parse AxisBlock keywords
bool BaseViewer::readAxisBlock(LineParser& parser, Axes& axes, int axis, bool strictBlockEnd)
{
	TextPrimitive::TextAnchor anchor;
	LineStipple::StippleType stipple;
	NumberFormat::FormatType ft;
	Axes::AutoScaleMethod as;

	while (!parser.eofOrBlank())
	{
		// Get line from file
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks + LineParser::SemiColonLineBreaks);

		// Get keyword and check number of arguments provided
		BaseViewer::AxisKeyword axisKwd = BaseViewer::axisKeyword(parser.argc(0));
		if ((axisKwd != BaseViewer::nAxisKeywords) && (BaseViewer::axisKeywordNArguments(axisKwd) > (parser.nArgs()-1)))
		{
			Messenger::print("Error : Axis keyword '%s' requires %i arguments, but only %i have been provided.\n", BaseViewer::axisKeyword(axisKwd), BaseViewer::axisKeywordNArguments(axisKwd), parser.nArgs()-1);
			return false;
		}
		switch (axisKwd)
		{
			// Autoscale method
			case (BaseViewer::AutoScaleKeyword):
				as = Axes::autoScaleMethod(parser.argc(1));
				if (as == Axes::nAutoScaleMethods)
				{
					Messenger::warn("Unrecognised autoscale method '%s'. Defaulting to '%s'.\n", parser.argc(1), Axes::autoScaleMethod(Axes::NoAutoScale));
					as = Axes::NoAutoScale;
					return false;
				}
				axes.setAutoScale(axis, as);
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
				axes.gridLineMajorStyle(axis).setWidth(parser.argd(1));
				stipple = LineStipple::stippleType(parser.argc(2));
				if (stipple == LineStipple::nStippleTypes)
				{
					Messenger::warn("Unrecognised line stipple type '%s'. Defaulting to 'NoStipple'.\n", parser.argc(2));
					stipple = LineStipple::NoStipple;
					return false;
				}
				axes.gridLineMajorStyle(axis).setStipple(stipple);
				axes.gridLineMajorStyle(axis).setColour(parser.argd(3), parser.argd(4), parser.argd(5), parser.argd(6));
				break;
			// GridLine minor style
			case (BaseViewer::GridLineMinorStyleKeyword):
				axes.gridLineMinorStyle(axis).setWidth(parser.argd(1));
				stipple = LineStipple::stippleType(parser.argc(2));
				if (stipple == LineStipple::nStippleTypes)
				{
					Messenger::warn("Unrecognised line stipple type '%s'. Defaulting to 'NoStipple'.\n", parser.argc(2));
					stipple = LineStipple::NoStipple;
					return false;
				}
				axes.gridLineMinorStyle(axis).setStipple(stipple);
				axes.gridLineMinorStyle(axis).setColour(parser.argd(3), parser.argd(4), parser.argd(5), parser.argd(6));
				break;
			// Invert
			case (BaseViewer::InvertKeyword):
				axes.setInverted(axis, parser.argb(1));
				break;
			// Axis label anchor
			case (BaseViewer::LabelAnchorKeyword):
				anchor = TextPrimitive::textAnchor(parser.argc(1));
				if (anchor == TextPrimitive::nTextAnchors)
				{
					Messenger::warn("Unrecognised text anchor '%s'. Defaulting to '%s'.\n", parser.argc(1), TextPrimitive::textAnchor(TextPrimitive::TopMiddleAnchor));
					anchor = TextPrimitive::TopMiddleAnchor;
					return false;
				}
				axes.setLabelAnchor(axis, anchor);
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
				ft = NumberFormat::formatType(parser.argc(1));
				if (ft == NumberFormat::nNumberFormats)
				{
					Messenger::warn("Unrecognised number format '%s'. Defaulting to '%s'.\n", parser.argc(1), NumberFormat::formatType(NumberFormat::DecimalFormat));
					ft = NumberFormat::DecimalFormat;
					return false;
				}
				axes.numberFormat(axis).setType(ft);
				axes.numberFormat(axis).setNDecimals(parser.argi(2));
				axes.numberFormat(axis).setUseUpperCaseExponent(parser.argb(3));
				axes.numberFormat(axis).setForcePrecedingPlus(parser.argb(4));
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
				axes.setTitle(axis, parser.argc(1));
				break;
			// Axis title anchor
			case (BaseViewer::TitleAnchorKeyword):
				anchor = TextPrimitive::textAnchor(parser.argc(1));
				if (anchor == TextPrimitive::nTextAnchors)
				{
					Messenger::warn("Unrecognised text anchor '%s'. Defaulting to 'TopMiddle'.\n");
					anchor = TextPrimitive::TopMiddleAnchor;
					return false;
				}
				axes.setTitleAnchor(axis, anchor);
				break;
			// Axis title orientation
			case (BaseViewer::TitleOrientationKeyword):
				axes.setTitleOrientation(axis, 0, parser.argd(1));
				axes.setTitleOrientation(axis, 1, parser.argd(2));
				axes.setTitleOrientation(axis, 2, parser.argd(3));
				axes.setTitleOrientation(axis, 3, parser.argd(4));
				break;
			// Axis visibility
			case (BaseViewer::VisibleAxisKeyword):
				axes.setVisible(axis, parser.argb(1));
				break;
			// Unrecognised keyword
			default:
				Messenger::warn("Unrecognised axis keyword: %s\n", parser.argc(0));
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
bool BaseViewer::writeAxisBlock(LineParser& parser, Axes& axes, int axis)
{
	parser.writeLineF("    %s %i\n", BaseViewer::viewKeyword(BaseViewer::AxisBlockKeyword), axis);
	parser.writeLineF("      %s %s\n", BaseViewer::axisKeyword(BaseViewer::AutoScaleKeyword), Axes::autoScaleMethod(axes.autoScale(axis)));
	parser.writeLineF("      %s %s\n", BaseViewer::axisKeyword(BaseViewer::AutoTicksKeyword), DissolveSys::btoa(axes.autoTicks(axis)));
	parser.writeLineF("      %s %f\n", BaseViewer::axisKeyword(BaseViewer::FirstTickKeyword), axes.tickFirst(axis));
	parser.writeLineF("      %s %s\n", BaseViewer::axisKeyword(BaseViewer::FractionalPositioningKeyword), DissolveSys::btoa(axes.positionIsFractional(axis)));
	parser.writeLineF("      %s %s %s %s\n", BaseViewer::axisKeyword(BaseViewer::GridLinesKeyword), DissolveSys::btoa(axes.gridLinesMajor(axis)), DissolveSys::btoa(axes.gridLinesMinor(axis)), DissolveSys::btoa(axes.gridLinesFull(axis)));
	LineStyle style = axes.gridLineMajorStyle(axis);
	parser.writeLineF("      %s %f '%s' %f %f %f %f\n", BaseViewer::axisKeyword(BaseViewer::GridLineMajorStyleKeyword), style.width(), LineStipple::stipple[style.stipple()].name, style.colour()[0], style.colour()[1], style.colour()[2], style.colour()[3]);
	style = axes.gridLineMinorStyle(axis);
	parser.writeLineF("      %s %f '%s' %f %f %f %f\n", BaseViewer::axisKeyword(BaseViewer::GridLineMinorStyleKeyword), style.width(), LineStipple::stipple[style.stipple()].name, style.colour()[0], style.colour()[1], style.colour()[2], style.colour()[3]);
	parser.writeLineF("      %s %s\n", BaseViewer::axisKeyword(BaseViewer::InvertKeyword), DissolveSys::btoa(axes.inverted(axis)));
	parser.writeLineF("      %s %s\n", BaseViewer::axisKeyword(BaseViewer::LabelAnchorKeyword), TextPrimitive::textAnchor(axes.labelAnchor(axis)));
	parser.writeLineF("      %s %f %f %f\n", BaseViewer::axisKeyword(BaseViewer::LabelOrientationKeyword), axes.labelOrientation(axis).x, axes.labelOrientation(axis).y, axes.labelOrientation(axis).z);
	parser.writeLineF("      %s %f %f\n", BaseViewer::axisKeyword(BaseViewer::LimitsKeyword), axes.min(axis), axes.max(axis));
	parser.writeLineF("      %s %s\n", BaseViewer::axisKeyword(BaseViewer::LogarithmicKeyword), DissolveSys::btoa(axes.logarithmic(axis)));
	parser.writeLineF("      %s %i\n", BaseViewer::axisKeyword(BaseViewer::MinorTicksKeyword), axes.minorTicks(axis));
	NumberFormat fmt = axes.numberFormat(axis);
	parser.writeLineF("      %s '%s' %i %s %s\n", BaseViewer::axisKeyword(BaseViewer::NumberFormatKeyword), NumberFormat::formatType(fmt.type()), fmt.nDecimals(), DissolveSys::btoa(fmt.useUpperCaseExponent()), DissolveSys::btoa(fmt.forcePrecedingPlus()));
	parser.writeLineF("      %s %f %f %f\n", BaseViewer::axisKeyword(BaseViewer::PositionFractionalKeyword), axes.positionFractional(axis).x, axes.positionFractional(axis).y, axes.positionFractional(axis).z);
	parser.writeLineF("      %s %f %f %f\n", BaseViewer::axisKeyword(BaseViewer::PositionRealKeyword), axes.positionReal(axis).x, axes.positionReal(axis).y, axes.positionReal(axis).z);
	parser.writeLineF("      %s %f\n", BaseViewer::axisKeyword(BaseViewer::StretchKeyword), axes.stretch(axis));
	parser.writeLineF("      %s %f\n", BaseViewer::axisKeyword(BaseViewer::TickDeltaKeyword), axes.tickDelta(axis));
	parser.writeLineF("      %s %f %f %f\n", BaseViewer::axisKeyword(BaseViewer::TickDirectionKeyword), axes.tickDirection(axis).x, axes.tickDirection(axis).y, axes.tickDirection(axis).z);
	parser.writeLineF("      %s %s\n", BaseViewer::axisKeyword(BaseViewer::TitleAnchorKeyword), TextPrimitive::textAnchor(axes.titleAnchor(axis)));
	parser.writeLineF("      %s '%s'\n", BaseViewer::axisKeyword(BaseViewer::TitleKeyword), axes.title(axis));
	parser.writeLineF("      %s %f %f %f %f\n", BaseViewer::axisKeyword(BaseViewer::TitleOrientationKeyword), axes.titleOrientation(axis).x, axes.titleOrientation(axis).y, axes.titleOrientation(axis).z, axes.titleOrientation(axis).w);
	parser.writeLineF("      %s %s\n", BaseViewer::axisKeyword(BaseViewer::VisibleAxisKeyword), DissolveSys::btoa(axes.visible(axis)));
	parser.writeLineF("    %s\n", BaseViewer::axisKeyword(BaseViewer::EndAxisKeyword));

	return true;
}

/*
 * Renderable Keywords
 */

// Renderable Block Keywords
const char* BaseViewerRenderableBlockKeywords[] = {
	"ColourAlphaIsGlobal", "ColourCustom", "ColourGlobalAlpha", "ColourHSVEnd", "ColourHSVStart", "ColourRGBEnd", "ColourRGBStart", "ColourSingle", "ColourStyle",
	"EndRenderable",
	"Group",
	"LineStyle",
	"Style",
	"TransformX", "TransformY", "TransformZ",
	"Visible"
};

// Renderable Block NArguments
int BaseViewerRenderableKeywordNArguments[] = {
	1, 5, 1, 5, 5, 5, 5, 4, 1,
	0,	
	1,
	2,
	1,
	2, 2, 2,
	1
};

// Convert text string to RenderableKeyword
BaseViewer::RenderableKeyword BaseViewer::renderableKeyword(const char* s)
{
	for (int n=0; n<nRenderableKeywords; ++n) if (DissolveSys::sameString(s, BaseViewerRenderableBlockKeywords[n])) return (BaseViewer::RenderableKeyword) n;
	return nRenderableKeywords;
}

// Convert RenderableKeyword to text string
const char* BaseViewer::renderableKeyword(BaseViewer::RenderableKeyword kwd)
{
	return BaseViewerRenderableBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int BaseViewer::renderableKeywordNArguments(BaseViewer::RenderableKeyword kwd)
{
	return BaseViewerRenderableKeywordNArguments[kwd];
}

// Read RenderableBlock keywords
bool BaseViewer::readRenderableBlock(LineParser& parser, Renderable* renderable, bool strictBlockEnd)
{
	int xyz;
	double alpha;
	ColourDefinition::ColourStyle cs;
	ColourDefinition& colourDefinition = renderable->colour();
	QColor hsvColour;
	int ds;
	LineStipple::StippleType stipple;

	while (!parser.eofOrBlank())
	{
		// Get line from file
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks + LineParser::SemiColonLineBreaks);

		// Get keyword and check number of arguments provided
		BaseViewer::RenderableKeyword renderableKwd = renderableKeyword(parser.argc(0));
		if ((renderableKwd != BaseViewer::nRenderableKeywords) && (renderableKeywordNArguments(renderableKwd) > (parser.nArgs()-1)))
		{
			Messenger::error("Renderable keyword '%s' requires %i arguments, but only %i have been provided.\n", renderableKeyword(renderableKwd), renderableKeywordNArguments(renderableKwd), parser.nArgs()-1);
			return false;
		}
		switch (renderableKwd)
		{
			// Colour alpha control
			case (BaseViewer::ColourAlphaIsGlobalKeyword):
				colourDefinition.setUseGlobalAlpha(parser.argb(1));
				break;
			// Colour Custom Gradient point definition
			case (BaseViewer::ColourCustomGradientKeyword):
				colourDefinition.addCustomGradientPoint(parser.argd(1), QColor(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)));
				break;
			// Colour alpha fixed value
			case (BaseViewer::ColourGlobalAlphaKeyword):
				alpha = parser.argd(1);
				if ((alpha < 0.0) || (alpha > 1.0))
				{
					Messenger::warn("Alpha value (%f) is out of range for %s keyword - it will be reset to 1.0.\n", alpha, renderableKeyword(renderableKwd));
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
				colourDefinition.setRGBGradientEnd(parser.argd(1), QColor(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)));
				break;
			// Colour RGB Gradient start point definition
			case (BaseViewer::ColourRGBGradientStartKeyword):
				colourDefinition.setRGBGradientStart(parser.argd(1), QColor(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)));
				break;
			// Colour single colour definition
			case (BaseViewer::ColourSingleKeyword):
				colourDefinition.setSingleColour(QColor(parser.argi(1), parser.argi(2), parser.argi(3), parser.argi(4)));
				break;
			// Colour style
			case (BaseViewer::ColourStyleKeyword):
				cs = ColourDefinition::colourStyle(parser.argc(1));
				if (cs == ColourDefinition::nColourStyles)
				{
					Messenger::warn("Unrecognised colour style '%s'. Defaulting to '%s'.\n", parser.argc(1), ColourDefinition::colourStyle(ColourDefinition::SingleColourStyle));
					cs = ColourDefinition::SingleColourStyle;
				}
				colourDefinition.setStyle(cs);
				break;
			// End input block
			case (BaseViewer::EndRenderableKeyword):
				return true;
				break;
			// Group
			case (BaseViewer::GroupKeyword):
				groupManager_.addToGroup(renderable, parser.argc(1));
				break;
			// Line style
			case (BaseViewer::LineStyleKeyword):
				renderable->lineStyle().setWidth(parser.argd(1));
				stipple = LineStipple::stippleType(parser.argc(2));
				if (stipple == LineStipple::nStippleTypes)
				{
					Messenger::warn("Unrecognised line stipple type '%s'. Defaulting to 'NoStipple'.\n", parser.argc(2));
					stipple = LineStipple::NoStipple;
				}
				renderable->lineStyle().setStipple(stipple);
				break;
			// Display style
			case (BaseViewer::StyleKeyword):
				ds = renderable->displayStyle(parser.argc(1));
				if (ds == -1) Messenger::warn("Unrecognised display style '%s'.\n", parser.argc(1));
				else renderable->setDisplayStyle(ds);
				break;
			// Transforms
			case (BaseViewer::TransformXKeyword):
			case (BaseViewer::TransformYKeyword):
			case (BaseViewer::TransformZKeyword):
				xyz = renderableKwd - BaseViewer::TransformXKeyword;
				renderable->setTransformEnabled(xyz, parser.argb(1));
				renderable->setTransformEquation(xyz,  parser.argc(2));
				break;
			// Visible flag
			case (BaseViewer::VisibleKeyword):
				renderable->setVisible(parser.argb(1));
				break;
			// Unrecognised Keyword
			default:
				Messenger::warn("Unrecognised renderable keyword: %s\n", parser.argc(0));
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
bool BaseViewer::writeRenderableBlock(LineParser& parser, Renderable* renderable, int indentLevel)
{
	// Construct indent string
	char* indent = new char[indentLevel*2+1];
	for (int n=0; n<indentLevel*2; ++n) indent[n] = ' ';
	indent[indentLevel*2] = '\0';

	parser.writeLineF("%s%s  %s  '%s'  '%s'  '%s'\n", indent, BaseViewer::inputBlock(BaseViewer::RenderableBlock), Renderable::renderableType(renderable->type()), renderable->objectTag(), renderable->name());

	// -- Transforms
	parser.writeLineF("%s  %s %s %s\n", indent, BaseViewer::renderableKeyword(BaseViewer::TransformXKeyword), DissolveSys::btoa(renderable->transformEnabled(0)), renderable->transformEquation(0));
	parser.writeLineF("%s  %s %s %s\n", indent, BaseViewer::renderableKeyword(BaseViewer::TransformYKeyword), DissolveSys::btoa(renderable->transformEnabled(1)), renderable->transformEquation(1));
	parser.writeLineF("%s  %s %s %s\n", indent, BaseViewer::renderableKeyword(BaseViewer::TransformZKeyword), DissolveSys::btoa(renderable->transformEnabled(2)), renderable->transformEquation(2));

	// Colour Setup
	const ColourDefinition& colourDef = renderable->colour();
	parser.writeLineF("%s  %s '%s'\n", indent, BaseViewer::renderableKeyword(BaseViewer::ColourStyleKeyword), ColourDefinition::colourStyle(colourDef.style()));
	QColor colour;
	double value;
	// -- Single Colour
	colour = colourDef.singleColour();
	parser.writeLineF("%s  %s %i %i %i %i\n", indent, BaseViewer::renderableKeyword(BaseViewer::ColourSingleKeyword), colour.red(), colour.green(), colour.blue(), colour.alpha());
	// -- RGB Gradient
	colour = colourDef.rgbGradientStartColour();
	value = colourDef.rgbGradientStartValue();
	parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, BaseViewer::renderableKeyword(BaseViewer::ColourRGBGradientStartKeyword), value, colour.red(), colour.green(), colour.blue(), colour.alpha());
	colour = colourDef.rgbGradientEndColour();
	value = colourDef.rgbGradientEndValue();
	parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, BaseViewer::renderableKeyword(BaseViewer::ColourRGBGradientEndKeyword), value, colour.red(), colour.green(), colour.blue(), colour.alpha());
	// -- HSV Gradient
	colour = colourDef.hsvGradientStartColour();
	value = colourDef.hsvGradientStartValue();
	parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, BaseViewer::renderableKeyword(BaseViewer::ColourHSVGradientStartKeyword), value, colour.hue(), colour.saturation(), colour.value(), colour.alpha());
	colour = colourDef.hsvGradientEndColour();
	value = colourDef.hsvGradientEndValue();
	parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, BaseViewer::renderableKeyword(BaseViewer::ColourHSVGradientEndKeyword), value, colour.hue(), colour.saturation(), colour.value(), colour.alpha());
	// -- Custom Gradient
	const Array<ColourScalePoint> customGradient = colourDef.customGradientPoints();
	for (int n=0; n<customGradient.nItems(); ++n)
	{
		const ColourScalePoint& point = customGradient.at(n);
		parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, BaseViewer::renderableKeyword(BaseViewer::ColourCustomGradientKeyword), point.value(), point.colour().red(), point.colour().green(), point.colour().blue(), point.colour().alpha());
	}
	// -- Alpha control
	parser.writeLineF("%s  %s %s\n", indent, BaseViewer::renderableKeyword(BaseViewer::ColourAlphaIsGlobalKeyword), DissolveSys::btoa(colourDef.useGlobalAlpha()));
	parser.writeLineF("%s  %s %f\n", indent, BaseViewer::renderableKeyword(BaseViewer::ColourGlobalAlphaKeyword), colourDef.globalAlpha());

	// Display
	parser.writeLineF("%s  %s %f '%s'\n", indent, BaseViewer::renderableKeyword(BaseViewer::LineStyleKeyword), renderable->lineStyle().width(), LineStipple::stipple[renderable->lineStyle().stipple()].name);
	parser.writeLineF("%s  %s %s\n", indent, BaseViewer::renderableKeyword(BaseViewer::StyleKeyword), renderable->displayStyle(renderable->displayStyle()));
	parser.writeLineF("%s  %s %s\n", indent, BaseViewer::renderableKeyword(BaseViewer::VisibleKeyword), DissolveSys::btoa(renderable->isVisible()));

	// Write Group if set
	if (renderable->group()) parser.writeLineF("%s  %s '%s'\n", indent, BaseViewer::renderableKeyword(BaseViewer::GroupKeyword), renderable->group()->name());

	parser.writeLineF("%s%s\n", indent, BaseViewer::renderableKeyword(BaseViewer::EndRenderableKeyword));

	return true;
}

/*
 * View Keywords
 */

// View Block Keywords
const char* BaseViewerViewBlockKeywords[] = {
	"AutoFollowType", "AutoPositionTitles", "Axis",
	"BoundingBox", "BoundingBoxPlaneY",
	"EndView",
	"FlatLabels",
	"LabelPointSize",
	"Perspective",
	"RotationX", "RotationY", "RotationZ",
	"TitlePointSize", "Translation",
	"UseBestFlatView",
	"VerticalShift", "ViewType"
};

// View Block NArguments
int BaseViewerViewKeywordNArguments[] = {
	1, 1, 1, 
	1, 1,
	0,
	1,
	1,
	1,
	3, 3, 3,
	1, 3,
	1,
	1, 1
};

// Convert text string to ViewKeyword
BaseViewer::ViewKeyword BaseViewer::viewKeyword(const char* s)
{
	for (int n=0; n<nViewKeywords; ++n) if (DissolveSys::sameString(s, BaseViewerViewBlockKeywords[n])) return (BaseViewer::ViewKeyword) n;
	return nViewKeywords;
}

// Convert ViewKeyword to text string
const char* BaseViewer::viewKeyword(BaseViewer::ViewKeyword kwd)
{
	return BaseViewerViewBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int BaseViewer::viewKeywordNArguments(BaseViewer::ViewKeyword kwd)
{
	return BaseViewerViewKeywordNArguments[kwd];
}

// Read ViewBlock keywords
bool BaseViewer::readViewBlock(LineParser& parser, bool strictBlockEnd)
{
	int xyz, axis;
	Matrix4 mat;
	View::ViewType vt;
	View::AutoFollowType aft;

	while (!parser.eofOrBlank())
	{
		// Get line from file
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks + LineParser::SemiColonLineBreaks);

		// Get keyword and check number of arguments provided
		BaseViewer::ViewKeyword viewKwd = BaseViewer::viewKeyword(parser.argc(0));
		if ((viewKwd != BaseViewer::nViewKeywords) && (BaseViewer::viewKeywordNArguments(viewKwd) > (parser.nArgs()-1)))
		{
			Messenger::print("Error : View keyword '%s' requires %i arguments, but only %i have been provided.\n", BaseViewer::viewKeyword(viewKwd), BaseViewer::viewKeywordNArguments(viewKwd), parser.nArgs()-1);
			return false;
		}
		switch (viewKwd)
		{
			// Auto-follow type
			case (BaseViewer::AutoFollowTypeKeyword):
				aft = View::autoFollowType(parser.argc(1));
				if (aft == View::nAutoFollowTypes)
				{
					Messenger::warn("Unrecognised auto-follow type '%s'. Defaulting to 'None'.\n", parser.argc(1));
					view_.setAutoFollowType(View::NoAutoFollow);
				}
				else view_.setAutoFollowType(aft);
				break;
			// Auto Position Axis Titles
			case (BaseViewer::AutoPositionTitlesKeyword):
				view_.axes().setAutoPositionTitles(parser.argb(1));
				break;
			// Axis block
			case (BaseViewer::AxisBlockKeyword):
				// Get target axis...
				axis = parser.argi(1);
				if ((axis < 0) || (axis > 2)) Messenger::warn("Axis index is out of range in input file. Defaults will be used.\n");
				else if (!readAxisBlock(parser, view_.axes(), axis)) return false;
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
				xyz = viewKwd - BaseViewer::RotationXKeyword;
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
				groupManager_.setVerticalShiftAmount((RenderableGroupManager::VerticalShiftAmount) parser.argi(1));
				break;
			// View Type
			case (BaseViewer::ViewTypeKeyword):
				vt = View::viewType(parser.argc(1));
				if (vt == View::nViewTypes)
				{
					Messenger::warn("Unrecognised view type '%s'. Defaulting to '%s'.\n", parser.argc(1), View::viewType(View::NormalView));
					vt = View::NormalView;
					return false;
				}
				view_.setViewType(vt);
				break;
			// Unrecognised Keyword
			default:
				Messenger::warn("Unrecognised view keyword: %s\n", parser.argc(0));
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
bool BaseViewer::writeViewBlock(LineParser& parser)
{
	parser.writeLineF("  %s\n", BaseViewer::inputBlock(BaseViewer::ViewBlock));
	parser.writeLineF("    %s %s\n", BaseViewer::viewKeyword(BaseViewer::AutoFollowTypeKeyword), View::autoFollowType(view_.autoFollowType()));
	parser.writeLineF("    %s %s\n", BaseViewer::viewKeyword(BaseViewer::AutoPositionTitlesKeyword), DissolveSys::btoa(view_.axes().autoPositionTitles()));
	for (int axis=0; axis < 3; ++axis) writeAxisBlock(parser, view_.axes(), axis);
	parser.writeLineF("    %s %s\n", BaseViewer::viewKeyword(BaseViewer::FlatLabelsKeyword), DissolveSys::btoa(view_.flatLabelsIn3D()));
	parser.writeLineF("    %s %f\n", BaseViewer::viewKeyword(BaseViewer::LabelPointSizeKeyword), view_.labelPointSize());
	parser.writeLineF("    %s %f\n", BaseViewer::viewKeyword(BaseViewer::TitlePointSizeKeyword), view_.titlePointSize());
	Matrix4 mat = view_.viewRotation();
	Vec3<double> trans = view_.viewTranslation();
	parser.writeLineF("    %s %f %f %f\n", BaseViewer::viewKeyword(BaseViewer::RotationXKeyword), mat[0], mat[1], mat[2]);
	parser.writeLineF("    %s %f %f %f\n", BaseViewer::viewKeyword(BaseViewer::RotationYKeyword), mat[4], mat[5], mat[6]);
	parser.writeLineF("    %s %f %f %f\n", BaseViewer::viewKeyword(BaseViewer::RotationZKeyword), mat[8], mat[9], mat[10]);
	parser.writeLineF("    %s %f %f %f\n", BaseViewer::viewKeyword(BaseViewer::TranslationKeyword), trans.x, trans.y, trans.z);
	parser.writeLineF("    %s %s\n", BaseViewer::viewKeyword(BaseViewer::PerspectiveKeyword), DissolveSys::btoa(view_.hasPerspective()));
	parser.writeLineF("    %s %s\n", BaseViewer::viewKeyword(BaseViewer::UseBestFlatViewKeyword), DissolveSys::btoa(view_.axes().useBestFlatView()));
	parser.writeLineF("    %s %i\n", BaseViewer::viewKeyword(BaseViewer::VerticalShiftKeyword), groupManager_.verticalShiftAmount());
	parser.writeLineF("    %s '%s'\n", BaseViewer::viewKeyword(BaseViewer::ViewTypeKeyword), View::viewType(view_.viewType()));
	parser.writeLineF("  %s\n", BaseViewer::viewKeyword(BaseViewer::EndViewKeyword));

	return true;
}

/*
 * Session I/O
 */

// Read custom input block through specified LineParser
bool BaseViewer::readCustomInputBlock(LineParser& parser)
{
	return false;
}

// Write custom input blocks through specified LineParser
bool BaseViewer::writeCustomInputBlocks(LineParser& parser)
{
	return true;
}

// Read session through parser specified
bool BaseViewer::readSession(LineParser& parser)
{
	// Clear existing data
	clear();

	// Parse input blocks
	bool success = parseInputBlocks(parser);

	// Show a message if we encountered problems...
	if (!success) Messenger::warn("Errors were encountered while reading the session data.\n");

	return success;
}

// Write session through parser specified
bool BaseViewer::writeSession(LineParser& parser)
{
	// Write Renderable data
	for (Renderable* rend = renderables_.first(); rend != NULL; rend = rend->next) if (!writeRenderableBlock(parser, rend)) return false;

	// Write View Data
	if (!writeViewBlock(parser)) return false;

	// Write end of session marker
	if (!parser.writeLineF("EndSession\n")) return false;

	return true;
}
