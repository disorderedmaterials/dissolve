/*
	*** Data Viewer - I/O
	*** src/gui/viewer/dataviewer_io.cpp
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

#include "gui/viewer/dataviewer.hui"
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
const char* DataViewerInputBlockKeywords[] = { "EndDataViewer", "Renderable", "View" };

// Convert text string to InputBlock
DataViewer::InputBlock DataViewer::inputBlock(const char* s)
{
	for (int n=0; n<DataViewer::nInputBlocks; ++n) if (DissolveSys::sameString(s, DataViewerInputBlockKeywords[n])) return (DataViewer::InputBlock) n;
	return DataViewer::nInputBlocks;
}

// Convert InputBlock to text string
const char* DataViewer::inputBlock(DataViewer::InputBlock id)
{
	return DataViewerInputBlockKeywords[id];
}

/*
 * Axis Keywords
 */

// Axis Block Keywords
const char* DataViewerAxisBlockKeywords[] = { "AutoScale", "AutoTicks", "EndAxis", "FirstTick", "FractionalPositioning", "GridLines", "GridLineMajorStyle", "GridLineMinorStyle", "Invert", "LabelAnchor", "LabelOrientation", "Limits", "Logarithmic", "MinorTicks", "NumberFormat", "PositionFractional", "PositionReal", "Stretch", "TickDelta", "TickDirection", "Title", "TitleAnchor", "TitleOrientation", "Visible" };

// Axis Block NArguments
int DataViewerAxisKeywordNArguments[] = { 1, 1, 0, 1, 1, 3, 6, 6, 1, 1, 3, 2, 1, 1, 4, 3, 3, 1, 1, 3, 1, 1, 4, 1 };

// Convert text string to AxisKeyword
DataViewer::AxisKeyword DataViewer::axisKeyword(const char* s)
{
	for (int n=0; n<nAxisKeywords; ++n) if (DissolveSys::sameString(s, DataViewerAxisBlockKeywords[n])) return (DataViewer::AxisKeyword) n;
	return nAxisKeywords;
}

// Convert DataViewerAxisBlockKeywords to text string
const char* DataViewer::axisKeyword(DataViewer::AxisKeyword kwd)
{
	return DataViewerAxisBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int DataViewer::axisKeywordNArguments(DataViewer::AxisKeyword kwd)
{
	return DataViewerAxisKeywordNArguments[kwd];
}

/*
 * Collection Keywords
 */

// Collection Block Keywords
const char* DataViewerRenderableBlockKeywords[] = {
	"ColourAlphaControl", "ColourAlphaFixed", "ColourCustomGradient", "ColourLinearRGBA", "ColourLinearRGBB", "ColourLinearHSVA", "ColourLinearHSVB", "ColourSingle", "ColourSource",
	"EndRenderable",
	"Group",
	"LineStyle",
	"Shininess", "Style",
	"TransformX", "TransformY", "TransformZ",
	"Visible" };

// Collection Block NArguments
int DataViewerRenderableKeywordNArguments[] = {
	1, 1, 5, 5, 5, 5, 5, 4, 1,
	0,	
	1,
	2,
	1, 1,
	2, 2, 2,
	1 };

// Convert text string to RenderableKeyword
DataViewer::RenderableKeyword DataViewer::renderableKeyword(const char* s)
{
	for (int n=0; n<nRenderableKeywords; ++n) if (DissolveSys::sameString(s, DataViewerRenderableBlockKeywords[n])) return (DataViewer::RenderableKeyword) n;
	return nRenderableKeywords;
}

// Convert RenderableKeyword to text string
const char* DataViewer::renderableKeyword(DataViewer::RenderableKeyword kwd)
{
	return DataViewerRenderableBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int DataViewer::renderableKeywordNArguments(DataViewer::RenderableKeyword kwd)
{
	return DataViewerRenderableKeywordNArguments[kwd];
}

/*
 * View Keywords
 */

// View Block Keywords
const char* DataViewerViewBlockKeywords[] = {
	"AutoFollowType", "AutoPositionTitles", "Axis",
	"BoundingBox", "BoundingBoxPlaneY",
	"EndViewPane",
	"FlatLabels",
	"LabelPointSize",
	"Perspective",
	"RotationX", "RotationY", "RotationZ",
	"TitlePointSize", "Translation",
	"UseBestFlatView",
	"VerticalShift", "ViewType"
};

// View Block NArguments
int DataViewerViewKeywordNArguments[] = {
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
DataViewer::ViewKeyword DataViewer::viewKeyword(const char* s)
{
	for (int n=0; n<nViewKeywords; ++n) if (DissolveSys::sameString(s, DataViewerViewBlockKeywords[n])) return (DataViewer::ViewKeyword) n;
	return nViewKeywords;
}

// Convert ViewKeyword to text string
const char* DataViewer::viewKeyword(DataViewer::ViewKeyword kwd)
{
	return DataViewerViewBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int DataViewer::viewKeywordNArguments(DataViewer::ViewKeyword kwd)
{
	return DataViewerViewKeywordNArguments[kwd];
}

/*
 * Read Routines
 */

// Parse AxisBlock keywords
bool DataViewer::readAxisBlock(LineParser& parser, Axes& axes, int axis, bool strictBlockEnd)
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
		DataViewer::AxisKeyword axisKwd = DataViewer::axisKeyword(parser.argc(0));
		if ((axisKwd != DataViewer::nAxisKeywords) && (DataViewer::axisKeywordNArguments(axisKwd) > (parser.nArgs()-1)))
		{
			Messenger::print("Error : Axis keyword '%s' requires %i arguments, but only %i have been provided.\n", DataViewer::axisKeyword(axisKwd), DataViewer::axisKeywordNArguments(axisKwd), parser.nArgs()-1);
			return false;
		}
		switch (axisKwd)
		{
			// Autoscale method
			case (DataViewer::AutoScaleKeyword):
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
			case (DataViewer::AutoTicksKeyword):
				axes.setAutoTicks(axis, parser.argb(1));
				break;
			// End input block
			case (DataViewer::EndAxisKeyword):
				return true;
				break;
			// First ticks
			case (DataViewer::FirstTickKeyword):
				axes.setFirstTick(axis, parser.argd(1));
				break;
			// Fractional positioning flag
			case (DataViewer::FractionalPositioningKeyword):
				axes.setPositionIsFractional(axis, parser.argb(1));
				break;
			// GridLines
			case (DataViewer::GridLinesKeyword):
				axes.setGridLinesMajor(axis, parser.argb(1));
				axes.setGridLinesMinor(axis, parser.argb(2));
				axes.setGridLinesFull(axis, parser.argb(3));
				break;
			// GridLine major style
			case (DataViewer::GridLineMajorStyleKeyword):
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
			case (DataViewer::GridLineMinorStyleKeyword):
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
			case (DataViewer::InvertKeyword):
				axes.setInverted(axis, parser.argb(1));
				break;
			// Axis label anchor
			case (DataViewer::LabelAnchorKeyword):
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
			case (DataViewer::LabelOrientationKeyword):
				axes.setLabelOrientation(axis, 0, parser.argd(1));
				axes.setLabelOrientation(axis, 1, parser.argd(2));
				axes.setLabelOrientation(axis, 2, parser.argd(3));
				break;
			// Limits
			case (DataViewer::LimitsKeyword):
				axes.setMin(axis, parser.argd(1));
				axes.setMax(axis, parser.argd(2));
				break;
			// Axis logarithmic flag
			case (DataViewer::LogarithmicKeyword):
				axes.setLogarithmic(axis, parser.argb(1));
				break;
			// Axis minor ticks
			case (DataViewer::MinorTicksKeyword):
				axes.setMinorTicks(axis, parser.argi(1));
				break;
			// Number Format
			case (DataViewer::NumberFormatKeyword):
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
			case (DataViewer::PositionFractionalKeyword):
				axes.setPositionFractional(axis, 0, parser.argd(1));
				axes.setPositionFractional(axis, 1, parser.argd(2));
				axes.setPositionFractional(axis, 2, parser.argd(3));
				break;
			// Axis position (in real surface-space coordinates)
			case (DataViewer::PositionRealKeyword):
				axes.setPositionReal(axis, 0, parser.argd(1));
				axes.setPositionReal(axis, 1, parser.argd(2));
				axes.setPositionReal(axis, 2, parser.argd(3));
				break;
			// Axis stretch factors
			case (DataViewer::StretchKeyword):
				axes.setStretch(axis, parser.argd(1));
				break;
			// Axis tick deltas
			case (DataViewer::TickDeltaKeyword):
				axes.setTickDelta(axis, parser.argd(1));
				break;
			// Axis tick direction
			case (DataViewer::TickDirectionKeyword):
				axes.setTickDirection(axis, 0, parser.argd(1));
				axes.setTickDirection(axis, 1, parser.argd(2));
				axes.setTickDirection(axis, 2, parser.argd(3));
				break;
			// Axis title
			case (DataViewer::TitleKeyword):
				axes.setTitle(axis, parser.argc(1));
				break;
			// Axis title anchor
			case (DataViewer::TitleAnchorKeyword):
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
			case (DataViewer::TitleOrientationKeyword):
				axes.setTitleOrientation(axis, 0, parser.argd(1));
				axes.setTitleOrientation(axis, 1, parser.argd(2));
				axes.setTitleOrientation(axis, 2, parser.argd(3));
				axes.setTitleOrientation(axis, 3, parser.argd(4));
				break;
			// Axis visibility
			case (DataViewer::VisibleAxisKeyword):
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

// Read RenderableBlock keywords
bool DataViewer::readRenderableBlock(LineParser& parser, Renderable* renderable, bool strictBlockEnd)
{
	DataSet* dataSet;
	int xyz;
	double alpha;
	ColourDefinition::AlphaControl ac;
	ColourDefinition::ColourSource cs;
	ColourDefinition& colourDefinition = renderable->colour();
	Renderable::DisplayStyle ds;
	LineStipple::StippleType stipple;

	while (!parser.eofOrBlank())
	{
		// Get line from file
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks + LineParser::SemiColonLineBreaks);

		// Get keyword and check number of arguments provided
		DataViewer::RenderableKeyword renderableKwd = renderableKeyword(parser.argc(0));
		if ((renderableKwd != DataViewer::nRenderableKeywords) && (renderableKeywordNArguments(renderableKwd) > (parser.nArgs()-1)))
		{
			Messenger::error("Renderable keyword '%s' requires %i arguments, but only %i have been provided.\n", renderableKeyword(renderableKwd), renderableKeywordNArguments(renderableKwd), parser.nArgs()-1);
			return false;
		}
		switch (renderableKwd)
		{
			// Colour alpha control
			case (DataViewer::ColourAlphaControlKeyword):
				ac = ColourDefinition::alphaControl(parser.argc(1));
				if (ac == ColourDefinition::nAlphaControls)
				{
					Messenger::warn("Unrecognised alpha control type '%s'. Defaulting to '%s'.\n", parser.argc(1), ColourDefinition::alphaControl(ColourDefinition::OwnAlpha));
					ac = ColourDefinition::OwnAlpha;
				}
				colourDefinition.setAlphaControl(ac);
				break;
			// Colour alpha fixed value
			case (DataViewer::ColourAlphaFixedKeyword):
				alpha = parser.argd(1);
				if ((alpha < 0.0) || (alpha > 1.0))
				{
					Messenger::warn("Alpha value (%f) is out of range for %s keyword - it will be reset to 1.0.\n", alpha, renderableKeyword(renderableKwd));
					alpha = 1.0;
				}
				colourDefinition.setFixedAlpha(alpha);
				break;
			// Colour Custom Gradient point definition
			case (DataViewer::ColourCustomGradientKeyword):
				colourDefinition.addCustomColourScalePoint(parser.argd(1), QColor(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)));
				break;
			// Colour Linear Gradient point definition
			case (DataViewer::ColourRGBGradientAKeyword):
			case (DataViewer::ColourRGBGradientBKeyword):
				colourDefinition.setColourScalePoint(ColourDefinition::RGBGradientSource, QColor(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)), parser.argd(1), renderableKwd == DataViewer::ColourRGBGradientAKeyword ? 0 : 1);
				break;
			// Colour Linear HSV Gradient point definition
			case (DataViewer::ColourHSVGradientAKeyword):
			case (DataViewer::ColourHSVGradientBKeyword):
				colourDefinition.setColourScalePoint(ColourDefinition::HSVGradientSource, QColor::fromHsv(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)), parser.argd(1), renderableKwd == DataViewer::ColourHSVGradientAKeyword ? 0 : 1);
				break;
			// Colour single colour definition
			case (DataViewer::ColourSingleKeyword):
				colourDefinition.setColourScalePoint(ColourDefinition::SingleColourSource, QColor(parser.argi(1), parser.argi(2), parser.argi(3), parser.argi(4)));
				break;
			// Colour source
			case (DataViewer::ColourSourceKeyword):
				cs = ColourDefinition::colourSource(parser.argc(1));
				if (cs == ColourDefinition::nColourSources)
				{
					Messenger::warn("Unrecognised colour source '%s'. Defaulting to '%s'.\n", parser.argc(1), ColourDefinition::colourSource(ColourDefinition::SingleColourSource));
					cs = ColourDefinition::SingleColourSource;
				}
				colourDefinition.setColourSource(cs);
				break;
			// End input block
			case (DataViewer::EndRenderableKeyword):
				return true;
				break;
			// Group
			case (DataViewer::GroupKeyword):
				renderable->setGroupName(parser.argc(1));
				break;
			// Line style
			case (DataViewer::LineStyleKeyword):
				renderable->lineStyle().setWidth(parser.argd(1));
				stipple = LineStipple::stippleType(parser.argc(2));
				if (stipple == LineStipple::nStippleTypes)
				{
					Messenger::warn("Unrecognised line stipple type '%s'. Defaulting to 'NoStipple'.\n", parser.argc(2));
					stipple = LineStipple::NoStipple;
				}
				renderable->lineStyle().setStipple(stipple);
				break;
			// Surface shininess
			case (DataViewer::ShininessKeyword):
				renderable->setDisplaySurfaceShininess(parser.argd(1));
				break;
			// Display style
			case (DataViewer::StyleKeyword):
				ds = Renderable::displayStyle(parser.argc(1));
				if (ds == Renderable::nDisplayStyles) Messenger::warn("Unrecognised display style '%s'.\n", parser.argc(1));
				else renderable->setDisplayStyle(ds);
				break;
			// Transforms
			case (DataViewer::TransformXKeyword):
			case (DataViewer::TransformYKeyword):
			case (DataViewer::TransformZKeyword):
				xyz = renderableKwd - DataViewer::TransformXKeyword;
				renderable->setTransformEnabled(xyz, parser.argb(1));
				renderable->setTransformEquation(xyz,  parser.argc(2));
				break;
			// Visible flag
			case (DataViewer::VisibleKeyword):
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

// Read ViewBlock keywords
bool DataViewer::readViewBlock(LineParser& parser, bool strictBlockEnd)
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
		DataViewer::ViewKeyword viewKwd = DataViewer::viewKeyword(parser.argc(0));
		if ((viewKwd != DataViewer::nViewKeywords) && (DataViewer::viewKeywordNArguments(viewKwd) > (parser.nArgs()-1)))
		{
			Messenger::print("Error : View keyword '%s' requires %i arguments, but only %i have been provided.\n", DataViewer::viewKeyword(viewKwd), DataViewer::viewKeywordNArguments(viewKwd), parser.nArgs()-1);
			return false;
		}
		switch (viewKwd)
		{
			// Auto-follow type
			case (DataViewer::AutoFollowTypeKeyword):
				aft = View::autoFollowType(parser.argc(1));
				if (aft == View::nAutoFollowTypes)
				{
					Messenger::warn("Unrecognised auto-follow type '%s'. Defaulting to 'None'.\n", parser.argc(1));
					view_.setAutoFollowType(View::NoAutoFollow);
				}
				else view_.setAutoFollowType(aft);
				break;
			// Auto Position Axis Titles
			case (DataViewer::AutoPositionTitlesKeyword):
				view_.axes().setAutoPositionTitles(parser.argb(1));
				break;
			// Axis block
			case (DataViewer::AxisBlockKeyword):
				// Get target axis...
				axis = parser.argi(1);
				if ((axis < 0) || (axis > 2)) Messenger::warn("Axis index is out of range in input file. Defaults will be used.\n");
				else if (!readAxisBlock(parser, view_.axes(), axis)) return false;
				break;
			// Bounding Box
			case (DataViewer::BoundingBoxKeyword):
				if ((parser.argi(1) < 0) || (parser.argi(1) >= View::nBoundingBoxes)) Messenger::warn("Value is out of range for %s keyword.\n", DataViewer::viewKeyword(viewKwd));
				else view_.setBoundingBox((View::BoundingBox) parser.argi(1));
				break;
			// Bounding Box plane y intercept
			case (DataViewer::BoundingBoxPlaneYKeyword):
				view_.setBoundingBoxPlaneY(parser.argd(1));
				break;
			// End input block
			case (DataViewer::EndViewKeyword):
				return true;
				break;
			// Flat labels flag
			case (DataViewer::FlatLabelsKeyword):
				view_.setFlatLabelsIn3D(parser.argb(1));
				break;
			// Label scale
			case (DataViewer::LabelPointSizeKeyword):
				view_.setLabelPointSize(parser.argd(1));
				break;
			// Rotation
			case (DataViewer::RotationXKeyword):
			case (DataViewer::RotationYKeyword):
			case (DataViewer::RotationZKeyword):
				xyz = viewKwd - DataViewer::RotationXKeyword;
				view_.setViewRotationColumn(xyz, parser.argd(1), parser.argd(2), parser.argd(3));
				break;
			// Perspective
			case (DataViewer::PerspectiveKeyword):
				view_.setHasPerspective(parser.argb(1));
				break;
			// Title scale
			case (DataViewer::TitlePointSizeKeyword):
				view_.setTitlePointSize(parser.argd(1));
				break;
			// Translation
			case (DataViewer::TranslationKeyword):
				view_.setViewTranslation(parser.argd(1), parser.argd(2), parser.argd(3));
				break;
			// Use best flat view
			case (DataViewer::UseBestFlatViewKeyword):
				view_.axes().setUseBestFlatView(parser.argb(1));
				break;
			// Vertical shift (renderable group manager)
			case (DataViewer::VerticalShiftKeyword):
				groupManager_.setVerticalShift((RenderableGroupManager::VerticalShift) parser.argi(1));
				break;
			// View Type
			case (DataViewer::ViewTypeKeyword):
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

// Parse main input blocks through specified parser
bool DataViewer::parseInputBlocks(LineParser& parser)
{
	// Read line from file and decide what to do with it
	DataViewer::InputBlock block;
	Renderable* renderable;
	Renderable::RenderableType rt;
	bool success = true;
	while (!parser.eofOrBlank())
	{
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks + LineParser::SemiColonLineBreaks);

		// We expect a block keyword in this loop...
		block = DataViewer::inputBlock(parser.argc(0));
		switch (block)
		{
			// End of DataViewer Input
			case (DataViewer::EndDataViewerBlock):
				return true;
				break;
			// Renderable Block
			case (DataViewer::RenderableBlock):
				// Check that two arguments have been given (type and object tag)
				if (!parser.hasArg(2)) return Messenger::error("Type and object tag for Renderable expected, but none found.\n");

				// Determine Renderable type
				rt = Renderable::renderableType(parser.argc(1));
				if (rt == Renderable::nRenderableTypes) return Messenger::error("Unknown Renderable type '%s' found.\n", parser.argc(1));

				renderable = RenderableFactory::create(rt, parser.argc(2));
				if (!renderable) return false;

				success = readRenderableBlock(parser, renderable);
				break;
			// View
			case (DataViewer::ViewBlock):
				success = readViewBlock(parser);
				break;
			default:
				Messenger::warn("Unrecognised input block: %s\n", parser.argc(0));
				return false;
				break;
		}

		// If we have failed for any reason, exit now
		if (!success) break;
	}

	return success;
}

// Read session through parser specified
bool DataViewer::readSession(LineParser& parser)
{
	// Clear existing data
	clear();

	// Parse input blocks
	bool success = parseInputBlocks(parser);

	// Show a message if we encountered problems...
	if (!success) Messenger::warn("Errors were encountered while reading the session data.\n");

	return success;
}

/*
 * Write Routines
 */

// Return boolean string based on integer value
const char* stringBool(int i)
{
	return (i > 0 ? "true" : "false");
}

// Return boolean string based on boolean value
const char* stringBool(bool b)
{
	return (b ? "true" : "false");
}

// Write AxisBlock keywords
bool DataViewer::writeAxisBlock(LineParser& parser, Axes& axes, int axis)
{
	parser.writeLineF("    %s %i\n", DataViewer::viewKeyword(DataViewer::AxisBlockKeyword), axis);
	parser.writeLineF("      %s %s\n", DataViewer::axisKeyword(DataViewer::AutoScaleKeyword), Axes::autoScaleMethod(axes.autoScale(axis)));
	parser.writeLineF("      %s %s\n", DataViewer::axisKeyword(DataViewer::AutoTicksKeyword), stringBool(axes.autoTicks(axis)));
	parser.writeLineF("      %s %f\n", DataViewer::axisKeyword(DataViewer::FirstTickKeyword), axes.tickFirst(axis));
	parser.writeLineF("      %s %s\n", DataViewer::axisKeyword(DataViewer::FractionalPositioningKeyword), stringBool(axes.positionIsFractional(axis)));
	parser.writeLineF("      %s %s %s %s\n", DataViewer::axisKeyword(DataViewer::GridLinesKeyword), stringBool(axes.gridLinesMajor(axis)), stringBool(axes.gridLinesMinor(axis)), stringBool(axes.gridLinesFull(axis)));
	LineStyle style = axes.gridLineMajorStyle(axis);
	parser.writeLineF("      %s %f '%s' %f %f %f %f\n", DataViewer::axisKeyword(DataViewer::GridLineMajorStyleKeyword), style.width(), LineStipple::stipple[style.stipple()].name, style.colour().redF(), style.colour().greenF(), style.colour().blueF(), style.colour().alphaF());
	style = axes.gridLineMinorStyle(axis);
	parser.writeLineF("      %s %f '%s' %f %f %f %f\n", DataViewer::axisKeyword(DataViewer::GridLineMinorStyleKeyword), style.width(), LineStipple::stipple[style.stipple()].name, style.colour().redF(), style.colour().greenF(), style.colour().blueF(), style.colour().alphaF());
	parser.writeLineF("      %s %s\n", DataViewer::axisKeyword(DataViewer::InvertKeyword), stringBool(axes.inverted(axis)));
	parser.writeLineF("      %s %s\n", DataViewer::axisKeyword(DataViewer::LabelAnchorKeyword), TextPrimitive::textAnchor(axes.labelAnchor(axis)));
	parser.writeLineF("      %s %f %f %f\n", DataViewer::axisKeyword(DataViewer::LabelOrientationKeyword), axes.labelOrientation(axis).x, axes.labelOrientation(axis).y, axes.labelOrientation(axis).z);
	parser.writeLineF("      %s %f %f\n", DataViewer::axisKeyword(DataViewer::LimitsKeyword), axes.min(axis), axes.max(axis));
	parser.writeLineF("      %s %s\n", DataViewer::axisKeyword(DataViewer::LogarithmicKeyword), stringBool(axes.logarithmic(axis)));
	parser.writeLineF("      %s %i\n", DataViewer::axisKeyword(DataViewer::MinorTicksKeyword), axes.minorTicks(axis));
	NumberFormat fmt = axes.numberFormat(axis);
	parser.writeLineF("      %s '%s' %i %s %s\n", DataViewer::axisKeyword(DataViewer::NumberFormatKeyword), NumberFormat::formatType(fmt.type()), fmt.nDecimals(), stringBool(fmt.useUpperCaseExponent()), stringBool(fmt.forcePrecedingPlus()));
	parser.writeLineF("      %s %f %f %f\n", DataViewer::axisKeyword(DataViewer::PositionFractionalKeyword), axes.positionFractional(axis).x, axes.positionFractional(axis).y, axes.positionFractional(axis).z);
	parser.writeLineF("      %s %f %f %f\n", DataViewer::axisKeyword(DataViewer::PositionRealKeyword), axes.positionReal(axis).x, axes.positionReal(axis).y, axes.positionReal(axis).z);
	parser.writeLineF("      %s %f\n", DataViewer::axisKeyword(DataViewer::StretchKeyword), axes.stretch(axis));
	parser.writeLineF("      %s %f\n", DataViewer::axisKeyword(DataViewer::TickDeltaKeyword), axes.tickDelta(axis));
	parser.writeLineF("      %s %f %f %f\n", DataViewer::axisKeyword(DataViewer::TickDirectionKeyword), axes.tickDirection(axis).x, axes.tickDirection(axis).y, axes.tickDirection(axis).z);
	parser.writeLineF("      %s %s\n", DataViewer::axisKeyword(DataViewer::TitleAnchorKeyword), TextPrimitive::textAnchor(axes.titleAnchor(axis)));
	parser.writeLineF("      %s '%s'\n", DataViewer::axisKeyword(DataViewer::TitleKeyword), axes.title(axis));
	parser.writeLineF("      %s %f %f %f %f\n", DataViewer::axisKeyword(DataViewer::TitleOrientationKeyword), axes.titleOrientation(axis).x, axes.titleOrientation(axis).y, axes.titleOrientation(axis).z, axes.titleOrientation(axis).w);
	parser.writeLineF("      %s %s\n", DataViewer::axisKeyword(DataViewer::VisibleAxisKeyword), stringBool(axes.visible(axis)));
	parser.writeLineF("    %s\n", DataViewer::axisKeyword(DataViewer::EndAxisKeyword));

	return true;
}

// Write RenderableBlock keywords
bool DataViewer::writeRenderableBlock(LineParser& parser, Renderable* renderable, int indentLevel)
{
	// Construct indent string
	char* indent = new char[indentLevel*2+1];
	for (int n=0; n<indentLevel*2; ++n) indent[n] = ' ';
	indent[indentLevel*2] = '\0';

	parser.writeLineF("%s%s  %s  '%s'\n", indent, DataViewer::inputBlock(DataViewer::RenderableBlock), Renderable::renderableType(renderable->type()), renderable->objectTag());

	// -- Transforms
	parser.writeLineF("%s  %s %s %s\n", indent, DataViewer::renderableKeyword(DataViewer::TransformXKeyword), stringBool(renderable->transformEnabled(0)), renderable->transformEquation(0));
	parser.writeLineF("%s  %s %s %s\n", indent, DataViewer::renderableKeyword(DataViewer::TransformYKeyword), stringBool(renderable->transformEnabled(1)), renderable->transformEquation(1));
	parser.writeLineF("%s  %s %s %s\n", indent, DataViewer::renderableKeyword(DataViewer::TransformZKeyword), stringBool(renderable->transformEnabled(2)), renderable->transformEquation(2));

	// Colour Setup
	const ColourDefinition& colourDef = renderable->colour();
	parser.writeLineF("%s  %s '%s'\n", indent, DataViewer::renderableKeyword(DataViewer::ColourSourceKeyword), ColourDefinition::colourSource(colourDef.colourSource()));
	ColourScalePoint* csp;
	QColor colour;
	double value;
	// -- Single Colour
	colour = colourDef.colourScalePointColour(ColourDefinition::SingleColourSource);
	parser.writeLineF("%s  %s %i %i %i %i\n", indent, DataViewer::renderableKeyword(DataViewer::ColourSingleKeyword), colour.red(), colour.green(), colour.blue(), colour.alpha());
	// -- RGB Gradient
	colour = colourDef.colourScalePointColour(ColourDefinition::RGBGradientSource, 0);
	value = colourDef.colourScalePointValue(ColourDefinition::RGBGradientSource, 0);
	parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, DataViewer::renderableKeyword(DataViewer::ColourRGBGradientAKeyword), value, colour.red(), colour.green(), colour.blue(), colour.alpha());
	colour = colourDef.colourScalePointColour(ColourDefinition::RGBGradientSource, 1);
	value = colourDef.colourScalePointValue(ColourDefinition::RGBGradientSource, 1);
	parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, DataViewer::renderableKeyword(DataViewer::ColourRGBGradientBKeyword), value, colour.red(), colour.green(), colour.blue(), colour.alpha());
	// -- HSV Gradient
	colour = colourDef.colourScalePointColour(ColourDefinition::HSVGradientSource, 0);
	value = colourDef.colourScalePointValue(ColourDefinition::HSVGradientSource, 0);
	parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, DataViewer::renderableKeyword(DataViewer::ColourHSVGradientAKeyword), value, colour.hue(), colour.saturation(), colour.value(), colour.alpha());
	colour = colourDef.colourScalePointColour(ColourDefinition::HSVGradientSource, 1);
	value = colourDef.colourScalePointValue(ColourDefinition::HSVGradientSource, 1);
	parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, DataViewer::renderableKeyword(DataViewer::ColourHSVGradientBKeyword), value, colour.hue(), colour.saturation(), colour.value(), colour.alpha());
	// -- Custom Gradient
	const Array<ColourScalePoint> customColourScale = colourDef.customColourScalePoints();
	for (int n=0; n<customColourScale.nItems(); ++n)
	{
		const ColourScalePoint& point = customColourScale.at(n);
		parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, DataViewer::renderableKeyword(DataViewer::ColourCustomGradientKeyword), point.value(), point.colour().red(), point.colour().green(), point.colour().blue(), point.colour().alpha());
	}
	// -- Alpha control
	parser.writeLineF("%s  %s '%s'\n", indent, DataViewer::renderableKeyword(DataViewer::ColourAlphaControlKeyword), ColourDefinition::alphaControl(colourDef.alphaControl()));
	parser.writeLineF("%s  %s %f\n", indent, DataViewer::renderableKeyword(DataViewer::ColourAlphaFixedKeyword), colourDef.fixedAlpha());

	// Display
	parser.writeLineF("%s  %s %f '%s'\n", indent, DataViewer::renderableKeyword(DataViewer::LineStyleKeyword), renderable->lineStyle().width(), LineStipple::stipple[renderable->lineStyle().stipple()].name);
	parser.writeLineF("%s  %s %f\n", indent, DataViewer::renderableKeyword(DataViewer::ShininessKeyword), renderable->displaySurfaceShininess());
	parser.writeLineF("%s  %s %s\n", indent, DataViewer::renderableKeyword(DataViewer::StyleKeyword), Renderable::displayStyle(renderable->displayStyle()));
	parser.writeLineF("%s  %s %s\n", indent, DataViewer::renderableKeyword(DataViewer::VisibleKeyword), stringBool(renderable->isVisible()));

	// Write Group if set
	if (renderable->hasGroupName()) parser.writeLineF("%s  %s '%s'\n", indent, DataViewer::renderableKeyword(DataViewer::GroupKeyword), renderable->groupName());

	parser.writeLineF("%s%s\n", indent, DataViewer::renderableKeyword(DataViewer::EndRenderableKeyword));

	return true;
}

// Write ViewBlock keywords
bool DataViewer::writeViewBlock(LineParser& parser)
{
	parser.writeLineF("  %s\n", DataViewer::inputBlock(DataViewer::ViewBlock));
	parser.writeLineF("    %s %s\n", DataViewer::viewKeyword(DataViewer::AutoFollowTypeKeyword), View::autoFollowType(view_.autoFollowType()));
	parser.writeLineF("    %s %s\n", DataViewer::viewKeyword(DataViewer::AutoPositionTitlesKeyword), stringBool(view_.axes().autoPositionTitles()));
	for (int axis=0; axis < 3; ++axis) writeAxisBlock(parser, view_.axes(), axis);
	parser.writeLineF("    %s %i\n", DataViewer::viewKeyword(DataViewer::BoundingBoxKeyword), view_.boundingBox());
	parser.writeLineF("    %s %f\n", DataViewer::viewKeyword(DataViewer::BoundingBoxPlaneYKeyword), view_.boundingBoxPlaneY());
	parser.writeLineF("    %s %s\n", DataViewer::viewKeyword(DataViewer::FlatLabelsKeyword), stringBool(view_.flatLabelsIn3D()));
	parser.writeLineF("    %s %f\n", DataViewer::viewKeyword(DataViewer::LabelPointSizeKeyword), view_.labelPointSize());
	parser.writeLineF("    %s %f\n", DataViewer::viewKeyword(DataViewer::TitlePointSizeKeyword), view_.titlePointSize());
	Matrix4 mat = view_.viewRotation();
	Vec3<double> trans = view_.viewTranslation();
	parser.writeLineF("    %s %f %f %f\n", DataViewer::viewKeyword(DataViewer::RotationXKeyword), mat[0], mat[1], mat[2]);
	parser.writeLineF("    %s %f %f %f\n", DataViewer::viewKeyword(DataViewer::RotationYKeyword), mat[4], mat[5], mat[6]);
	parser.writeLineF("    %s %f %f %f\n", DataViewer::viewKeyword(DataViewer::RotationZKeyword), mat[8], mat[9], mat[10]);
	parser.writeLineF("    %s %f %f %f\n", DataViewer::viewKeyword(DataViewer::TranslationKeyword), trans.x, trans.y, trans.z);
	parser.writeLineF("    %s %s\n", DataViewer::viewKeyword(DataViewer::PerspectiveKeyword), stringBool(view_.hasPerspective()));
	parser.writeLineF("    %s %s\n", DataViewer::viewKeyword(DataViewer::UseBestFlatViewKeyword), stringBool(view_.axes().useBestFlatView()));
	parser.writeLineF("    %s %i\n", DataViewer::viewKeyword(DataViewer::VerticalShiftKeyword), groupManager_.verticalShift());
	parser.writeLineF("    %s '%s'\n", DataViewer::viewKeyword(DataViewer::ViewTypeKeyword), View::viewType(view_.viewType()));
	parser.writeLineF("  %s\n", DataViewer::viewKeyword(DataViewer::EndViewKeyword));

	return true;
}

// Write session through parser specified
bool DataViewer::writeSession(LineParser& parser)
{
	// Write Renderable data
	for (Renderable* rend = renderables_.first(); rend != NULL; rend = rend->next) if (!writeRenderableBlock(parser, rend)) return false;

	// Write View Data
	if (!writeViewBlock(parser)) return false;

	// Write end of DataViewer input marker
	if (!parser.writeLineF("EndDataViewer\n")) return false;

	return true;
}
