/*
	*** Data Viewer - I/O
	*** src/gui/viewer/dataviewer_io.cpp
	Copyright T. Youngs 2013-2019

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/viewer/dataviewer.hui"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "base/messenger.h"
#include <string.h>
#include <QMessageBox>

/*
 * Input Block Keywords
 */

// Input File Block Keywords
const char* DataViewerInputBlockKeywords[] = { "Collection", "EndDataViewer", "View" };

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
const char* DataViewerCollectionBlockKeywords[] = {
	"ColourAlphaControl", "ColourAlphaFixed", "ColourCustomGradient", "ColourLinearRGBA", "ColourLinearRGBB", "ColourLinearHSVA", "ColourLinearHSVB", "ColourSingle", "ColourSource",
	"DataSet",
	"EndCollection",
	"Group",
	"LineStyle",
	"Shininess", "Style",
	"TransformX", "TransformY", "TransformZ",
	"Visible" };

// Collection Block NArguments
int DataViewerCollectionKeywordNArguments[] = {
	1, 1, 5, 5, 5, 5, 5, 4, 1,
	1,
	0,	
	1,
	2,
	1, 1,
	2, 2, 2,
	1 };

// Convert text string to CollectionKeyword
DataViewer::CollectionKeyword DataViewer::collectionKeyword(const char* s)
{
	for (int n=0; n<nCollectionKeywords; ++n) if (DissolveSys::sameString(s, DataViewerCollectionBlockKeywords[n])) return (DataViewer::CollectionKeyword) n;
	return nCollectionKeywords;
}

// Convert CollectionKeyword to text string
const char* DataViewer::collectionKeyword(DataViewer::CollectionKeyword kwd)
{
	return DataViewerCollectionBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int DataViewer::collectionKeywordNArguments(DataViewer::CollectionKeyword kwd)
{
	return DataViewerCollectionKeywordNArguments[kwd];
}

/*
 * DataSet Keywords
 */

// DataSet Block Keywords
const char* DataViewerDataSetBlockKeywords[] = { "Data", "EndDataSet", "Source", "Z" };

// DataSet Block NArguments
int DataViewerDataSetKeywordNArguments[] = { 0, 0, 1, 1 };

// Convert text string to DataSetKeyword
DataViewer::DataSetKeyword DataViewer::dataSetKeyword(const char* s)
{
	for (int n=0; n<nDataSetKeywords; ++n) if (DissolveSys::sameString(s, DataViewerDataSetBlockKeywords[n])) return (DataViewer::DataSetKeyword) n;
	return nDataSetKeywords;
}

// Convert DataSetKeyword to text string
const char* DataViewer::dataSetKeyword(DataViewer::DataSetKeyword kwd)
{
	return DataViewerDataSetBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int DataViewer::dataSetKeywordNArguments(DataViewer::DataSetKeyword kwd)
{
	return DataViewerDataSetKeywordNArguments[kwd];
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
	"RoleTargetCollection", "RotationX", "RotationY", "RotationZ",
	"TitlePointSize", "Translation",
	"UseBestFlatView",
	"VerticalShift",
	"ViewType"
};

// View Block NArguments
int DataViewerViewKeywordNArguments[] = {
	1, 1, 1, 
	1, 1,
	0,
	1,
	1,
	1,
	1, 3, 3, 3,
	1, 3,
	1,
	1,
	1
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

// Read CollectionBlock keywords
bool DataViewer::readCollectionBlock(LineParser& parser, Collection* collection, bool strictBlockEnd)
{
	DataSet* dataSet;
	int xyz;
	double alpha;
	ColourDefinition::AlphaControl ac;
	ColourDefinition::ColourSource cs;
	ColourDefinition& colourDefinition = collection->colour();
	Collection::DisplayStyle ds;
	LineStipple::StippleType stipple;

	while (!parser.eofOrBlank())
	{
		// Get line from file
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks + LineParser::SemiColonLineBreaks);

		// Get keyword and check number of arguments provided
		DataViewer::CollectionKeyword collectionKwd = collectionKeyword(parser.argc(0));
		if ((collectionKwd != DataViewer::nCollectionKeywords) && (collectionKeywordNArguments(collectionKwd) > (parser.nArgs()-1)))
		{
			Messenger::error("Collection keyword '%s' requires %i arguments, but only %i have been provided.\n", collectionKeyword(collectionKwd), collectionKeywordNArguments(collectionKwd), parser.nArgs()-1);
			return false;
		}
		switch (collectionKwd)
		{
			// Colour alpha control
			case (DataViewer::ColourAlphaControlKeyword):
				ac = ColourDefinition::alphaControl(parser.argc(1));
				if (ac == ColourDefinition::nAlphaControls)
				{
					Messenger::warn("Unrecognised alpha control type '%s'. Defaulting to '%s'.\n", parser.argc(1), ColourDefinition::alphaControl(ColourDefinition::OwnAlpha));
					ac = ColourDefinition::OwnAlpha;
					return false;
				}
				colourDefinition.setAlphaControl(ac);
				break;
			// Colour alpha fixed value
			case (DataViewer::ColourAlphaFixedKeyword):
				alpha = parser.argd(1);
				if ((alpha < 0.0) || (alpha > 1.0))
				{
					Messenger::warn("Alpha value (%f) is out of range for %s keyword - it will be reset to 1.0.\n", alpha, collectionKeyword(collectionKwd));
					alpha = 1.0;
					return false;
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
				colourDefinition.setColourScalePoint(ColourDefinition::RGBGradientSource, QColor(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)), parser.argd(1), collectionKwd == DataViewer::ColourRGBGradientAKeyword ? 0 : 1);
				break;
			// Colour Linear HSV Gradient point definition
			case (DataViewer::ColourHSVGradientAKeyword):
			case (DataViewer::ColourHSVGradientBKeyword):
				colourDefinition.setColourScalePoint(ColourDefinition::HSVGradientSource, QColor::fromHsv(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)), parser.argd(1), collectionKwd == DataViewer::ColourHSVGradientAKeyword ? 0 : 1);
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
					return false;
				}
				colourDefinition.setColourSource(cs);
				break;
			// DataSet
			case (DataViewer::DataSetDefinitionKeyword):
				// Create new dataset
				dataSet = collection->addDataSet();
				dataSet->setName(parser.argc(1));
				if (!readDataSetBlock(parser, dataSet, collection)) return false;
				break;
			// End input block
			case (DataViewer::EndCollectionKeyword):
				return true;
				break;
			// Group
			case (DataViewer::GroupKeyword):
				collection->setGroupName(parser.argc(1));
				break;
			// Line style
			case (DataViewer::LineStyleKeyword):
				collection->displayLineStyle().setWidth(parser.argd(1));
				stipple = LineStipple::stippleType(parser.argc(2));
				if (stipple == LineStipple::nStippleTypes)
				{
					Messenger::warn("Unrecognised line stipple type '%s'. Defaulting to 'NoStipple'.\n", parser.argc(2));
					stipple = LineStipple::NoStipple;
					return false;
				}
				collection->displayLineStyle().setStipple(stipple);
				break;
			// Surface shininess
			case (DataViewer::ShininessKeyword):
				collection->setDisplaySurfaceShininess(parser.argd(1));
				break;
			// Display style
			case (DataViewer::StyleKeyword):
				ds = Collection::displayStyle(parser.argc(1));
				if (ds == Collection::nDisplayStyles)
				{
					Messenger::warn("Unrecognised display style '%s'.\n", parser.argc(1));
					return false;
				}
				collection->setDisplayStyle(ds);
				break;
			// Data Transform
			case (DataViewer::TransformXKeyword):
			case (DataViewer::TransformYKeyword):
			case (DataViewer::TransformZKeyword):
				xyz = collectionKwd - DataViewer::TransformXKeyword;
				collection->setTransformEnabled(xyz, parser.argb(1));
				collection->setTransformEquation(xyz,  parser.argc(2));
				break;
			// Visible flag
			case (DataViewer::VisibleCollectionKeyword):
				collection->setVisible(parser.argb(1));
				break;
			// Unrecognised Keyword
			default:
				Messenger::warn("Unrecognised collection keyword: %s\n", parser.argc(0));
				return false;
				break;
		}
	}

	if (strictBlockEnd)
	{
		Messenger::print("Error : Unterminated 'Collection' block.\n");
		return false;
	}

	return true;
}

// Read DataSetBlock keywords
bool DataViewer::readDataSetBlock(LineParser& parser, DataSet* dataSet, Collection* collection, bool strictBlockEnd)
{
	bool foundEnd;
	DataSet::DataSource source;
	Data1D data;
	while (!parser.eofOrBlank())
	{
		// Get line from file
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks + LineParser::SemiColonLineBreaks);

		// Get keyword and check number of arguments provided
		DataViewer::DataSetKeyword dataSetKwd = dataSetKeyword(parser.argc(0));
		if ((dataSetKwd != DataViewer::nDataSetKeywords) && (dataSetKeywordNArguments(dataSetKwd) > (parser.nArgs()-1)))
		{
			Messenger::print("Error : DataSet keyword '%s' requires %i arguments, but only %i have been provided.\n", dataSetKeyword(dataSetKwd), dataSetKeywordNArguments(dataSetKwd), parser.nArgs()-1);
			return false;
		}
		switch (dataSetKwd)
		{
			case (DataViewer::DataKeyword):
				data.clear();
				foundEnd = false;
				do
				{
					parser.getArgsDelim(LineParser::Defaults);
					// Check for 'EndData'
					if (DissolveSys::sameString(parser.argc(0), "EndData")) foundEnd = true;
					else data.addPoint(parser.argd(0), parser.argd(1));
				} while ((!foundEnd) && (!parser.eofOrBlank()));
				if (!foundEnd)
				{
					Messenger::print("Error : Unterminated 'Data' block in dataSet '%s'.\n", qPrintable(dataSet->name()));
					return false;
				}

				dataSet->setData(data);
				break;
			case (DataViewer::EndDataSetKeyword):
				return true;
				break;
			case (DataViewer::SourceKeyword):
				source = DataSet::dataSource(parser.argc(1));
				if (source == DataSet::nDataSources)
				{
					Messenger::warn("Datasource for dataSet not recognised (%s)\n", parser.argc(1));
					return false;
					break;
				}
				dataSet->setDataSource(source);
				// Depending on the source, we might expect other data here...
				if (source == DataSet::FileSource)
				{
					if (parser.hasArg(2)) dataSet->setSourceFileName(parser.argc(2));
					else
					{
						Messenger::error("Expected data file name after 'Source File' declaration in dataSet '%s'.\n", qPrintable(dataSet->name()));
						return false;
					}
				}
				else if (source == DataSet::Data1DSource)
				{
					if (!parser.hasArg(2))
					{
						Messenger::error("Object name for Data1D expected, but none found.\n");
						return false;
					}
					// Locate Data1D
					Data1D* sourceData1D = Data1D::findObject(parser.argc(2));
					if (!sourceData1D) Messenger::printVerbose("Couldn't locate specified Data1D object (%s).\n", parser.argc(2));
					dataSet->setSourceData(parser.argc(2));
				}
				break;
			case (DataViewer::ZKeyword):
				collection->setDataSetZ(dataSet, parser.argd(1));
				break;
			// Unrecognised Keyword
			default:
				Messenger::warn("Unrecognised DataSet keyword: %s\n", parser.argc(0));
				return false;
				break;
		}
	}

	if (strictBlockEnd)
	{
		Messenger::print("Error : Unterminated 'DataSet' block.\n");
		return false;
	}

	return true;
}

// Read ViewBlock keywords
bool DataViewer::readViewBlock(LineParser& parser, bool strictBlockEnd)
{
	int xyz, axis;
	Collection* c;
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
			// Role associated collection
			case (DataViewer::RoleTargetCollectionKeyword):
				// Locate named collection
				c = collection(parser.argc(1));
				if (!c) Messenger::warn("Collection '%s' not found, and can't be associated to view.\n", parser.argc(1));
				else view_.addCollectionTarget(c);
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
			// Vertical shift (collection group manager)
			case (DataViewer::VerticalShiftKeyword):
				view_.collectionGroupManager().setVerticalShift((CollectionGroupManager::VerticalShift) parser.argi(1));
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
	bool success = true;
	while (!parser.eofOrBlank())
	{
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks + LineParser::SemiColonLineBreaks);

		// We expect a block keyword in this loop...
		block = DataViewer::inputBlock(parser.argc(0));
		switch (block)
		{
			// Collection Block
			case (DataViewer::CollectionBlock):
				// Create new master Collection and set its title
				currentCollection_ = addCollection(parser.argc(1));

				// Load the collection data
				success = readCollectionBlock(parser, currentCollection_);

				// Set it for display in the current ViewPane
				view_.addCollectionTarget(currentCollection_);
				break;
			// End of DataViewer Input
			case (DataViewer::EndDataViewerBlock):
				return true;
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
	startNewSession(false);

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

// Write CollectionBlock keywords
bool DataViewer::writeCollectionBlock(LineParser& parser, Collection* collection, int indentLevel)
{
	// Construct indent string
	char* indent = new char[indentLevel*2+1];
	for (int n=0; n<indentLevel*2; ++n) indent[n] = ' ';
	indent[indentLevel*2] = '\0';

	parser.writeLineF("%s%s '%s'\n", indent, DataViewer::inputBlock(DataViewer::CollectionBlock), collection->name());

	// -- Transforms
	parser.writeLineF("%s  %s %s %s\n", indent, DataViewer::collectionKeyword(DataViewer::TransformXKeyword), stringBool(collection->transformEnabled(0)), collection->transformEquation(0));
	parser.writeLineF("%s  %s %s %s\n", indent, DataViewer::collectionKeyword(DataViewer::TransformYKeyword), stringBool(collection->transformEnabled(1)), collection->transformEquation(1));
	parser.writeLineF("%s  %s %s %s\n", indent, DataViewer::collectionKeyword(DataViewer::TransformZKeyword), stringBool(collection->transformEnabled(2)), collection->transformEquation(2));


	// Colour Setup
	ColourDefinition& colourDef = collection->colour();
	parser.writeLineF("%s  %s '%s'\n", indent, DataViewer::collectionKeyword(DataViewer::ColourSourceKeyword), ColourDefinition::colourSource(colourDef.colourSource()));
	ColourScalePoint* csp;
	QColor colour;
	double value;
	// -- Single Colour
	colour = colourDef.colourScalePointColour(ColourDefinition::SingleColourSource);
	parser.writeLineF("%s  %s %i %i %i %i\n", indent, DataViewer::collectionKeyword(DataViewer::ColourSingleKeyword), colour.red(), colour.green(), colour.blue(), colour.alpha());
	// -- RGB Gradient
	colour = colourDef.colourScalePointColour(ColourDefinition::RGBGradientSource, 0);
	value = colourDef.colourScalePointValue(ColourDefinition::RGBGradientSource, 0);
	parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, DataViewer::collectionKeyword(DataViewer::ColourRGBGradientAKeyword), value, colour.red(), colour.green(), colour.blue(), colour.alpha());
	colour = colourDef.colourScalePointColour(ColourDefinition::RGBGradientSource, 1);
	value = colourDef.colourScalePointValue(ColourDefinition::RGBGradientSource, 1);
	parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, DataViewer::collectionKeyword(DataViewer::ColourRGBGradientBKeyword), value, colour.red(), colour.green(), colour.blue(), colour.alpha());
	// -- HSV Gradient
	colour = colourDef.colourScalePointColour(ColourDefinition::HSVGradientSource, 0);
	value = colourDef.colourScalePointValue(ColourDefinition::HSVGradientSource, 0);
	parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, DataViewer::collectionKeyword(DataViewer::ColourHSVGradientAKeyword), value, colour.hue(), colour.saturation(), colour.value(), colour.alpha());
	colour = colourDef.colourScalePointColour(ColourDefinition::HSVGradientSource, 1);
	value = colourDef.colourScalePointValue(ColourDefinition::HSVGradientSource, 1);
	parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, DataViewer::collectionKeyword(DataViewer::ColourHSVGradientBKeyword), value, colour.hue(), colour.saturation(), colour.value(), colour.alpha());
	// -- Custom Gradient
	for (csp = colourDef.customColourScalePoints(); csp != NULL; csp = csp->next)
	{
		parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, DataViewer::collectionKeyword(DataViewer::ColourCustomGradientKeyword), csp->value(), csp->colour().red(), csp->colour().green(), csp->colour().blue(), csp->colour().alpha());
	}
	// -- Alpha control
	parser.writeLineF("%s  %s '%s'\n", indent, DataViewer::collectionKeyword(DataViewer::ColourAlphaControlKeyword), ColourDefinition::alphaControl(colourDef.alphaControl()));
	parser.writeLineF("%s  %s %f\n", indent, DataViewer::collectionKeyword(DataViewer::ColourAlphaFixedKeyword), colourDef.fixedAlpha());

	// Display
	parser.writeLineF("%s  %s %f '%s'\n", indent, DataViewer::collectionKeyword(DataViewer::LineStyleKeyword), collection->displayLineStyle().width(), LineStipple::stipple[collection->displayLineStyle().stipple()].name);
	parser.writeLineF("%s  %s %f\n", indent, DataViewer::collectionKeyword(DataViewer::ShininessKeyword), collection->displaySurfaceShininess());
	parser.writeLineF("%s  %s %s\n", indent, DataViewer::collectionKeyword(DataViewer::StyleKeyword), Collection::displayStyle(collection->displayStyle()));
	parser.writeLineF("%s  %s %s\n", indent, DataViewer::collectionKeyword(DataViewer::VisibleCollectionKeyword), stringBool(collection->visible()));

	// Loop over datasets
	for (DataSet* dataSet = collection->dataSets(); dataSet != NULL; dataSet = dataSet->next) writeDataSetBlock(parser, dataSet, indentLevel);

	// Write Group if set
	if (collection->hasGroupName()) parser.writeLineF("%s  %s '%s'\n", indent, DataViewer::collectionKeyword(DataViewer::GroupKeyword), collection->groupName());

	parser.writeLineF("%s%s\n", indent, DataViewer::collectionKeyword(DataViewer::EndCollectionKeyword));

	return true;
}

// Write DataSetBlock keywords
bool DataViewer::writeDataSetBlock(LineParser& parser, DataSet* dataSet, int indentLevel)
{
	// Construct indent string
	char* indent = new char[indentLevel*2+1];
	for (int n=0; n<indentLevel*2; ++n) indent[n] = ' ';
	indent[indentLevel*2] = '\0';

	parser.writeLineF("%s  %s '%s'\n", indent, DataViewer::collectionKeyword(DataViewer::DataSetDefinitionKeyword), dataSet->name());
	if (dataSet->dataSource() == DataSet::FileSource) parser.writeLineF("%s    %s %s '%s'\n", indent, DataViewer::dataSetKeyword(DataViewer::SourceKeyword), DataSet::dataSource(dataSet->dataSource()), dataSet->sourceFileName());
	else if (dataSet->dataSource() == DataSet::Data1DSource)  parser.writeLineF("%s    %s %s '%s'\n", indent, DataViewer::dataSetKeyword(DataViewer::SourceKeyword), DataSet::dataSource(dataSet->dataSource()), dataSet->sourceData1D());
	else parser.writeLineF("%s    %s %s\n", indent, DataViewer::dataSetKeyword(DataViewer::SourceKeyword), DataSet::dataSource(dataSet->dataSource()));
	parser.writeLineF("%s    %s %f\n", indent, DataViewer::dataSetKeyword(DataViewer::ZKeyword), dataSet->z());
	if (dataSet->dataSource() == DataSet::InternalSource)
	{
		parser.writeLineF("%s    %s\n", indent, DataViewer::dataSetKeyword(DataViewer::DataKeyword));
		for (int n=0; n< dataSet->data().nValues(); ++n) parser.writeLineF("%s      %f  %f\n", indent, dataSet->data().constXAxis(n), dataSet->data().constValue(n));
		parser.writeLineF("%s    End%s\n", indent, DataViewer::dataSetKeyword(DataViewer::DataKeyword));
	}
	parser.writeLineF("%s  %s\n", indent, DataViewer::dataSetKeyword(DataViewer::EndDataSetKeyword));

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
	for (TargetData* target = view_.collectionTargets(); target != NULL; target = target->next)
	{
		if (!Collection::objectValid(target->collection(), "collection in DataViewer::writeViewBlock")) continue;
		parser.writeLineF("    %s '%s'\n", DataViewer::viewKeyword(DataViewer::RoleTargetCollectionKeyword), target->collection()->name());
	}
	parser.writeLineF("    %s %s\n", DataViewer::viewKeyword(DataViewer::UseBestFlatViewKeyword), stringBool(view_.axes().useBestFlatView()));
	parser.writeLineF("    %s %i\n", DataViewer::viewKeyword(DataViewer::VerticalShiftKeyword), view_.collectionGroupManager().verticalShift());
	parser.writeLineF("    %s '%s'\n", DataViewer::viewKeyword(DataViewer::ViewTypeKeyword), View::viewType(view_.viewType()));
	parser.writeLineF("  %s\n", DataViewer::viewKeyword(DataViewer::EndViewKeyword));

	return true;
}

// Write session through parser specified
bool DataViewer::writeSession(LineParser& parser)
{
	// Write Collection Data
	for (Collection* collection = collections_.first(); collection != NULL; collection = collection->next) if (!writeCollectionBlock(parser, collection)) return false;

	// Write View Data
	if (!writeViewBlock(parser)) return false;

	// Write end of DataViewer input marker
	if (!parser.writeLineF("EndDataViewer\n")) return false;

	return true;
}
