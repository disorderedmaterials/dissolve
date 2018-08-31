/*
	*** Session Load
	*** src/gui/uchroma/session/load.cpp
	Copyright T. Youngs 2013-2018

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

#include "gui/uchroma/uchromabase.h"
#include "gui/uchroma/kernels/fit.h"
#include "base/lineparser.h"
#include <QMessageBox>
#include <QFileDialog>

#define CHECKIOFAIL { if (hardIOFail_) { return false; } else { break; } }

// Parse AxisBlock keywords
bool UChromaBase::readAxisBlock(LineParser& parser, Axes& axes, int axis, bool strictBlockEnd)
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
		UChromaBase::AxisKeyword axisKwd = UChromaBase::axisKeyword(parser.argc(0));
		if ((axisKwd != UChromaBase::nAxisKeywords) && (UChromaBase::axisKeywordNArguments(axisKwd) > (parser.nArgs()-1)))
		{
			Messenger::print("Error : Axis keyword '%s' requires %i arguments, but only %i have been provided.\n", UChromaBase::axisKeyword(axisKwd), UChromaBase::axisKeywordNArguments(axisKwd), parser.nArgs()-1);
			return false;
		}
		switch (axisKwd)
		{
			// Autoscale method
			case (UChromaBase::AutoScaleKeyword):
				as = Axes::autoScaleMethod(parser.argc(1));
				if (as == Axes::nAutoScaleMethods)
				{
					Messenger::warn("Unrecognised autoscale method '%s'. Defaulting to '%s'.\n", parser.argc(1), Axes::autoScaleMethod(Axes::NoAutoScale));
					as = Axes::NoAutoScale;
					CHECKIOFAIL
				}
				axes.setAutoScale(axis, as);
				break;
			// Auto ticks
			case (UChromaBase::AutoTicksKeyword):
				axes.setAutoTicks(axis, parser.argb(1));
				break;
			// End input block
			case (UChromaBase::EndAxisKeyword):
				return true;
				break;
			// First ticks
			case (UChromaBase::FirstTickKeyword):
				axes.setFirstTick(axis, parser.argd(1));
				break;
			// Fractional positioning flag
			case (UChromaBase::FractionalPositioningKeyword):
				axes.setPositionIsFractional(axis, parser.argb(1));
				break;
			// GridLines
			case (UChromaBase::GridLinesKeyword):
				axes.setGridLinesMajor(axis, parser.argb(1));
				axes.setGridLinesMinor(axis, parser.argb(2));
				axes.setGridLinesFull(axis, parser.argb(3));
				break;
			// GridLine major style
			case (UChromaBase::GridLineMajorStyleKeyword):
				axes.gridLineMajorStyle(axis).setWidth(parser.argd(1));
				stipple = LineStipple::stippleType(parser.argc(2));
				if (stipple == LineStipple::nStippleTypes)
				{
					Messenger::warn("Unrecognised line stipple type '%s'. Defaulting to 'NoStipple'.\n", parser.argc(2));
					stipple = LineStipple::NoStipple;
					CHECKIOFAIL
				}
				axes.gridLineMajorStyle(axis).setStipple(stipple);
				axes.gridLineMajorStyle(axis).setColour(parser.argd(3), parser.argd(4), parser.argd(5), parser.argd(6));
				break;
			// GridLine minor style
			case (UChromaBase::GridLineMinorStyleKeyword):
				axes.gridLineMinorStyle(axis).setWidth(parser.argd(1));
				stipple = LineStipple::stippleType(parser.argc(2));
				if (stipple == LineStipple::nStippleTypes)
				{
					Messenger::warn("Unrecognised line stipple type '%s'. Defaulting to 'NoStipple'.\n", parser.argc(2));
					stipple = LineStipple::NoStipple;
					CHECKIOFAIL
				}
				axes.gridLineMinorStyle(axis).setStipple(stipple);
				axes.gridLineMinorStyle(axis).setColour(parser.argd(3), parser.argd(4), parser.argd(5), parser.argd(6));
				break;
			// Invert
			case (UChromaBase::InvertKeyword):
				axes.setInverted(axis, parser.argb(1));
				break;
			// Axis label anchor
			case (UChromaBase::LabelAnchorKeyword):
				anchor = TextPrimitive::textAnchor(parser.argc(1));
				if (anchor == TextPrimitive::nTextAnchors)
				{
					Messenger::warn("Unrecognised text anchor '%s'. Defaulting to '%s'.\n", parser.argc(1), TextPrimitive::textAnchor(TextPrimitive::TopMiddleAnchor));
					anchor = TextPrimitive::TopMiddleAnchor;
					CHECKIOFAIL
				}
				axes.setLabelAnchor(axis, anchor);
				break;
			// Axis label orientation
			case (UChromaBase::LabelOrientationKeyword):
				axes.setLabelOrientation(axis, 0, parser.argd(1));
				axes.setLabelOrientation(axis, 1, parser.argd(2));
				axes.setLabelOrientation(axis, 2, parser.argd(3));
				break;
			// Limits
			case (UChromaBase::LimitsKeyword):
				axes.setMin(axis, parser.argd(1));
				axes.setMax(axis, parser.argd(2));
				break;
			// Axis logarithmic flag
			case (UChromaBase::LogarithmicKeyword):
				axes.setLogarithmic(axis, parser.argb(1));
				break;
			// Axis minor ticks
			case (UChromaBase::MinorTicksKeyword):
				axes.setMinorTicks(axis, parser.argi(1));
				break;
			// Number Format
			case (UChromaBase::NumberFormatKeyword):
				ft = NumberFormat::formatType(parser.argc(1));
				if (ft == NumberFormat::nNumberFormats)
				{
					Messenger::warn("Unrecognised number format '%s'. Defaulting to '%s'.\n", parser.argc(1), NumberFormat::formatType(NumberFormat::DecimalFormat));
					ft = NumberFormat::DecimalFormat;
					CHECKIOFAIL
				}
				axes.numberFormat(axis).setType(ft);
				axes.numberFormat(axis).setNDecimals(parser.argi(2));
				axes.numberFormat(axis).setUseUpperCaseExponent(parser.argb(3));
				axes.numberFormat(axis).setForcePrecedingPlus(parser.argb(4));
				break;
			// Axis position (in fractional axis coordinates)
			case (UChromaBase::PositionFractionalKeyword):
				axes.setPositionFractional(axis, 0, parser.argd(1));
				axes.setPositionFractional(axis, 1, parser.argd(2));
				axes.setPositionFractional(axis, 2, parser.argd(3));
				break;
			// Axis position (in real surface-space coordinates)
			case (UChromaBase::PositionRealKeyword):
				axes.setPositionReal(axis, 0, parser.argd(1));
				axes.setPositionReal(axis, 1, parser.argd(2));
				axes.setPositionReal(axis, 2, parser.argd(3));
				break;
			// Axis stretch factors
			case (UChromaBase::StretchKeyword):
				axes.setStretch(axis, parser.argd(1));
				break;
			// Axis tick deltas
			case (UChromaBase::TickDeltaKeyword):
				axes.setTickDelta(axis, parser.argd(1));
				break;
			// Axis tick direction
			case (UChromaBase::TickDirectionKeyword):
				axes.setTickDirection(axis, 0, parser.argd(1));
				axes.setTickDirection(axis, 1, parser.argd(2));
				axes.setTickDirection(axis, 2, parser.argd(3));
				break;
			// Axis title
			case (UChromaBase::TitleKeyword):
				axes.setTitle(axis, parser.argc(1));
				break;
			// Axis title anchor
			case (UChromaBase::TitleAnchorKeyword):
				anchor = TextPrimitive::textAnchor(parser.argc(1));
				if (anchor == TextPrimitive::nTextAnchors)
				{
					Messenger::warn("Unrecognised text anchor '%s'. Defaulting to 'TopMiddle'.\n");
					anchor = TextPrimitive::TopMiddleAnchor;
					CHECKIOFAIL
				}
				axes.setTitleAnchor(axis, anchor);
				break;
			// Axis title orientation
			case (UChromaBase::TitleOrientationKeyword):
				axes.setTitleOrientation(axis, 0, parser.argd(1));
				axes.setTitleOrientation(axis, 1, parser.argd(2));
				axes.setTitleOrientation(axis, 2, parser.argd(3));
				axes.setTitleOrientation(axis, 3, parser.argd(4));
				break;
			// Axis visibility
			case (UChromaBase::VisibleAxisKeyword):
				axes.setVisible(axis, parser.argb(1));
				break;
			// Unrecognised keyword
			default:
				Messenger::warn("Unrecognised axis keyword: %s\n", parser.argc(0));
				CHECKIOFAIL
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
bool UChromaBase::readCollectionBlock(LineParser& parser, Collection* collection, bool strictBlockEnd)
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
		UChromaBase::CollectionKeyword collectionKwd = collectionKeyword(parser.argc(0));
		if ((collectionKwd != UChromaBase::nCollectionKeywords) && (collectionKeywordNArguments(collectionKwd) > (parser.nArgs()-1)))
		{
			Messenger::error("Collection keyword '%s' requires %i arguments, but only %i have been provided.\n", collectionKeyword(collectionKwd), collectionKeywordNArguments(collectionKwd), parser.nArgs()-1);
			return false;
		}
		switch (collectionKwd)
		{
			// Colour alpha control
			case (UChromaBase::ColourAlphaControlKeyword):
				ac = ColourDefinition::alphaControl(parser.argc(1));
				if (ac == ColourDefinition::nAlphaControls)
				{
					Messenger::warn("Unrecognised alpha control type '%s'. Defaulting to '%s'.\n", parser.argc(1), ColourDefinition::alphaControl(ColourDefinition::OwnAlpha));
					ac = ColourDefinition::OwnAlpha;
					CHECKIOFAIL
				}
				colourDefinition.setAlphaControl(ac);
				break;
			// Colour alpha fixed value
			case (UChromaBase::ColourAlphaFixedKeyword):
				alpha = parser.argd(1);
				if ((alpha < 0.0) || (alpha > 1.0))
				{
					Messenger::warn("Alpha value (%f) is out of range for %s keyword - it will be reset to 1.0.\n", alpha, collectionKeyword(collectionKwd));
					alpha = 1.0;
					CHECKIOFAIL
				}
				colourDefinition.setFixedAlpha(alpha);
				break;
			// Colour Custom Gradient point definition
			case (UChromaBase::ColourCustomGradientKeyword):
				colourDefinition.addCustomColourScalePoint(parser.argd(1), QColor(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)));
				break;
			// Colour Linear Gradient point definition
			case (UChromaBase::ColourRGBGradientAKeyword):
			case (UChromaBase::ColourRGBGradientBKeyword):
				colourDefinition.setColourScalePoint(ColourDefinition::RGBGradientSource, QColor(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)), parser.argd(1), collectionKwd == UChromaBase::ColourRGBGradientAKeyword ? 0 : 1);
				break;
			// Colour Linear HSV Gradient point definition
			case (UChromaBase::ColourHSVGradientAKeyword):
			case (UChromaBase::ColourHSVGradientBKeyword):
				colourDefinition.setColourScalePoint(ColourDefinition::HSVGradientSource, QColor::fromHsv(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)), parser.argd(1), collectionKwd == UChromaBase::ColourHSVGradientAKeyword ? 0 : 1);
				break;
			// Colour single colour definition
			case (UChromaBase::ColourSingleKeyword):
				colourDefinition.setColourScalePoint(ColourDefinition::SingleColourSource, QColor(parser.argi(1), parser.argi(2), parser.argi(3), parser.argi(4)));
				break;
			// Colour source
			case (UChromaBase::ColourSourceKeyword):
				cs = ColourDefinition::colourSource(parser.argc(1));
				if (cs == ColourDefinition::nColourSources)
				{
					Messenger::warn("Unrecognised colour source '%s'. Defaulting to '%s'.\n", parser.argc(1), ColourDefinition::colourSource(ColourDefinition::SingleColourSource));
					cs = ColourDefinition::SingleColourSource;
					CHECKIOFAIL
				}
				colourDefinition.setColourSource(cs);
				break;
			// Dataset directory
			case (UChromaBase::DataDirectoryKeyword):
				collection->setDataFileDirectory(QDir(parser.argc(1)));
				if (!collection->dataFileDirectory().isReadable())
				{
					QMessageBox::StandardButton button = QMessageBox::warning(parent_, "Error", "The data directory specified (" + collection->dataFileDirectory().absolutePath() + ") does not exist or is unreadable.\nDo you want to reset the datafile location?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
					if (button == QMessageBox::Yes)
					{
						QString dir = QFileDialog::getExistingDirectory(parent_, "Data Directory", "Choose the directory containing the required files:");
						if (!dir.isEmpty()) collection->setDataFileDirectory(dir);
					}
				}
				break;
			// DataSet
			case (UChromaBase::DataSetDefinitionKeyword):
				// Create new dataset
				dataSet = collection->addDataSet();
				dataSet->setName(parser.argc(1));
				if (!readDataSetBlock(parser, dataSet, collection)) return false;
				break;
			// End input block
			case (UChromaBase::EndCollectionKeyword):
				return true;
				break;
			// Fit data block
			case (UChromaBase::FitBlockKeyword):
				if (!readCollectionBlock(parser, collection->addFit(parser.argc(1)))) return false;
				break;
			// Fit parameters block
			case (UChromaBase::FitParametersBlockKeyword):
				// Check that a FitKernel exists in the current collection
				if (!collection->fitKernel())
				{
					Messenger::warn("No FitKernel exists in the collection '%s', but a FitParameters block was found.\nCreating a new one...\n", qPrintable(collection->name()));
					collection->addFitKernel();
				}
				if (!readFitParametersBlock(parser, collection->fitKernel())) return false;
				break;
			// Group
			case (UChromaBase::GroupKeyword):
				collection->setGroupName(parser.argc(1));
				break;
			// Interpolate flags
			case (UChromaBase::InterpolateKeyword):
				collection->setInterpolate(0, parser.argb(1));
				collection->setInterpolate(2, parser.argb(2));
				break;
			// Interpolate constrain flags
			case (UChromaBase::InterpolateConstrainKeyword):
				collection->setInterpolateConstrained(0, parser.argb(1));
				collection->setInterpolateConstrained(2, parser.argb(2));
				break;
			// Interpolation step flags
			case (UChromaBase::InterpolateStepKeyword):
				collection->setInterpolationStep(0, parser.argd(1));
				collection->setInterpolationStep(2, parser.argd(2));
				break;
			// Line style
			case (UChromaBase::LineStyleKeyword):
				collection->displayLineStyle().setWidth(parser.argd(1));
				stipple = LineStipple::stippleType(parser.argc(2));
				if (stipple == LineStipple::nStippleTypes)
				{
					Messenger::warn("Unrecognised line stipple type '%s'. Defaulting to 'NoStipple'.\n", parser.argc(2));
					stipple = LineStipple::NoStipple;
					CHECKIOFAIL
				}
				collection->displayLineStyle().setStipple(stipple);
				break;
			// Surface shininess
			case (UChromaBase::ShininessKeyword):
				collection->setDisplaySurfaceShininess(parser.argd(1));
				break;
			// Slice data block
			case (UChromaBase::SliceBlockKeyword):
				if (!readCollectionBlock(parser, collection->addSlice(parser.argc(1)))) return false;
				break;
			// Display style
			case (UChromaBase::StyleKeyword):
				ds = Collection::displayStyle(parser.argc(1));
				if (ds == Collection::nDisplayStyles)
				{
					Messenger::warn("Unrecognised display style '%s'.\n", parser.argc(1));
					CHECKIOFAIL
				}
				collection->setDisplayStyle(ds);
				break;
			// Data Transform
			case (UChromaBase::TransformXKeyword):
			case (UChromaBase::TransformYKeyword):
			case (UChromaBase::TransformZKeyword):
				xyz = collectionKwd - UChromaBase::TransformXKeyword;
				collection->setTransformEnabled(xyz, parser.argb(1));
				collection->setTransformEquation(xyz,  parser.argc(2));
				break;
			// Visible flag
			case (UChromaBase::VisibleCollectionKeyword):
				collection->setVisible(parser.argb(1));
				break;
			// Unrecognised Keyword
			default:
				Messenger::warn("Unrecognised collection keyword: %s\n", parser.argc(0));
				CHECKIOFAIL
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
bool UChromaBase::readDataSetBlock(LineParser& parser, DataSet* dataSet, Collection* collection, bool strictBlockEnd)
{
	bool foundEnd;
	DataSet::DataSource source;
	Data1D data;
	while (!parser.eofOrBlank())
	{
		// Get line from file
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks + LineParser::SemiColonLineBreaks);

		// Get keyword and check number of arguments provided
		UChromaBase::DataSetKeyword dataSetKwd = dataSetKeyword(parser.argc(0));
		if ((dataSetKwd != UChromaBase::nDataSetKeywords) && (dataSetKeywordNArguments(dataSetKwd) > (parser.nArgs()-1)))
		{
			Messenger::print("Error : DataSet keyword '%s' requires %i arguments, but only %i have been provided.\n", dataSetKeyword(dataSetKwd), dataSetKeywordNArguments(dataSetKwd), parser.nArgs()-1);
			return false;
		}
		switch (dataSetKwd)
		{
			case (UChromaBase::DataKeyword):
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
			case (UChromaBase::EndDataSetKeyword):
				return true;
				break;
			case (UChromaBase::SourceKeyword):
				source = DataSet::dataSource(parser.argc(1));
				if (source == DataSet::nDataSources)
				{
					Messenger::warn("Datasource for dataSet not recognised (%s)\n", parser.argc(1));
					CHECKIOFAIL
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
			case (UChromaBase::ZKeyword):
				collection->setDataSetZ(dataSet, parser.argd(1));
				break;
			// Unrecognised Keyword
			default:
				Messenger::warn("Unrecognised DataSet keyword: %s\n", parser.argc(0));
				CHECKIOFAIL
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

// Read FitParametersBlock keywords
bool UChromaBase::readFitParametersBlock(LineParser& parser, FitKernel* fitKernel, bool strictBlockEnd)
{
	FitKernel::RangeType rangeType;
	IndexData::IndexType indexType;
	EquationVariable* eqVar;
	ReferenceVariable* refVar;
	DataSpaceRange* dataSpaceRange;
	while (!parser.eofOrBlank())
	{
		// Get line from file
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks + LineParser::SemiColonLineBreaks);

		// Get keyword and check number of arguments provided
		UChromaBase::FitParametersKeyword fitParamsKwd = fitParametersKeyword(parser.argc(0));
		if ((fitParamsKwd != UChromaBase::nFitParametersKeywords) && (fitParametersKeywordNArguments(fitParamsKwd) > (parser.nArgs()-1)))
		{
			Messenger::print("Error : FitParameters keyword '%s' requires %i arguments, but only %i have been provided.\n", fitParametersKeyword(fitParamsKwd), fitParametersKeywordNArguments(fitParamsKwd), parser.nArgs()-1);
			return false;
		}
		switch (fitParamsKwd)
		{
			case (UChromaBase::EndFitParametersKeyword):
				return true;
				break;
			case (UChromaBase::EquationKeyword):
				fitKernel->setEquation(parser.argc(1));
				break;
			case (UChromaBase::FitResultsBlockKeyword):
				// Make sure that the DataSpace has been initialised....
				dataSpaceRange = NULL;
				if (!fitKernel->initialiseDataSpace()) Messenger::warn("Failed to create DataSpace for FitKernel, so cannot store FitResults.\n");
				else
				{
					// Get DataSpaceRange with index specified
					dataSpaceRange = fitKernel->dataSpaceRange(parser.argi(1)-1);
					if (dataSpaceRange == NULL) Messenger::warn("Failed to get range with index %i in FitKernel. FitResults will be ignored.\n", parser.argi(1));
				}
				if (!readFitResultsBlock(parser, dataSpaceRange)) return false;
				break;
			case (UChromaBase::GlobalKeyword):
				fitKernel->setGlobal(parser.argb(1));
				break;
			case (UChromaBase::LimitStrengthKeyword):
				fitKernel->setLimitStrength(parser.argd(1));
				break;
			case (UChromaBase::OrthogonalKeyword):
				fitKernel->setOrthogonal(parser.argb(1));
				break;
			case (UChromaBase::ReferenceKeyword):
				// Create new reference with this name
				refVar = fitKernel->addReference(parser.argc(1));
				if (!refVar) CHECKIOFAIL
				indexType = IndexData::indexType(parser.argc(2));
				if (indexType == IndexData::nIndexTypes)
				{
					Messenger::warn("Unrecognised type '%s' for reference '%s' - defaulting to 'Normal'.\n", parser.argc(2), parser.argc(1));
					CHECKIOFAIL
				}
				refVar->xIndex().setType(indexType);
				refVar->xIndex().setIndex(parser.argi(3));
				refVar->xIndex().setOffset(parser.argi(4));
				indexType = IndexData::indexType(parser.argc(5));
				if (indexType == IndexData::nIndexTypes)
				{
					Messenger::warn("Unrecognised type '%s' for reference '%s' - defaulting to 'Normal'.\n", parser.argc(5), parser.argc(1));
					CHECKIOFAIL
				}
				refVar->zIndex().setType(indexType);
				refVar->zIndex().setIndex(parser.argi(6));
				refVar->zIndex().setOffset(parser.argi(7));
				refVar->setZDataSetName(parser.argc(8));
				break;
			case (UChromaBase::VariableKeyword):
				// First, see if named variable exists
				eqVar = fitKernel->variable(parser.argc(1));
				if (!eqVar)
				{
					Messenger::warn("Variable '%s' is not in fit equation, so will ignore settings in FitParameters.\n");
					CHECKIOFAIL
				}
				eqVar->setFit(parser.argb(2));
				eqVar->setValue(parser.argd(3));
				eqVar->setMinimumLimit(parser.argb(4), parser.argd(5));
				eqVar->setMaximumLimit(parser.argb(6), parser.argd(7));
				break;
			case (UChromaBase::XRangeTypeKeyword):
				rangeType = FitKernel::rangeType(parser.argc(1));
				if (rangeType == FitKernel::nRangeTypes)
				{
					Messenger::warn("Unrecognised range type '%s' given for X in fit. Defaulting to '%s'\n", parser.argc(1), FitKernel::rangeType(FitKernel::AbsoluteRange));
					rangeType = FitKernel::AbsoluteRange;
					CHECKIOFAIL
				}
				fitKernel->setXRange(rangeType);
				break;
			case (UChromaBase::XRangeAbsoluteKeyword):
				fitKernel->setAbsoluteXMin(parser.argd(1));
				fitKernel->setAbsoluteXMax(parser.argd(2));
				break;
			case (UChromaBase::XRangeIndexKeyword):
				fitKernel->setIndexXMin(parser.argi(1)-1);
				fitKernel->setIndexXMax(parser.argi(2)-1);
				break;
			case (UChromaBase::XRangeIndexSingleKeyword):
				fitKernel->setIndexXSingle(parser.argi(1)-1);
				break;
			case (UChromaBase::ZRangeTypeKeyword):
				rangeType = FitKernel::rangeType(parser.argc(1));
				if (rangeType == FitKernel::nRangeTypes)
				{
					Messenger::warn("Unrecognised range type '%s' given for Z in fit. Defaulting to '%s'\n", parser.argc(1), FitKernel::rangeType(FitKernel::AbsoluteRange));
					rangeType = FitKernel::AbsoluteRange;
					CHECKIOFAIL
				}
				fitKernel->setZRange(rangeType);
				break;
			case (UChromaBase::ZRangeAbsoluteKeyword):
				fitKernel->setAbsoluteZMin(parser.argd(1));
				fitKernel->setAbsoluteZMax(parser.argd(2));
				break;
			case (UChromaBase::ZRangeIndexKeyword):
				fitKernel->setIndexZMin(parser.argi(1)-1);
				fitKernel->setIndexZMax(parser.argi(2)-1);
				break;
			case (UChromaBase::ZRangeIndexSingleKeyword):
				fitKernel->setIndexZSingle(parser.argi(1)-1);
				break;
			// Unrecognised Keyword
			default:
				Messenger::warn("Unrecognised FitParameters keyword: %s\n", parser.argc(0));
				CHECKIOFAIL
				break;
		}
	}

	if (strictBlockEnd)
	{
		Messenger::print("Error : Unterminated 'FitParameters' block.\n");
		return false;
	}

	return true;
}

// Read FitResultsBlock keywords
bool UChromaBase::readFitResultsBlock(LineParser& parser, DataSpaceRange* dataSpaceRange, bool strictBlockEnd)
{
	while (!parser.eofOrBlank())
	{
		// Get line from file
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks + LineParser::SemiColonLineBreaks);

		// Get keyword and check number of arguments provided
		UChromaBase::FitResultsKeyword fitParamsKwd = fitResultsKeyword(parser.argc(0));
		if ((fitParamsKwd != UChromaBase::nFitResultsKeywords) && (fitResultsKeywordNArguments(fitParamsKwd) > (parser.nArgs()-1)))
		{
			Messenger::print("Error : FitResults keyword '%s' requires %i arguments, but only %i have been provided.\n", fitResultsKeyword(fitParamsKwd), fitResultsKeywordNArguments(fitParamsKwd), parser.nArgs()-1);
			return false;
		}
		switch (fitParamsKwd)
		{
			case (UChromaBase::EndFitResultsKeyword):
				return true;
				break;
			case (UChromaBase::FittedValueKeyword):
				if (!dataSpaceRange) continue;
				dataSpaceRange->setFittedValue(parser.argc(1), parser.argd(2));
				break;
			// Unrecognised Keyword
			default:
				Messenger::warn("Unrecognised FitResults keyword: %s\n", parser.argc(0));
				CHECKIOFAIL
				break;
		}
	}

	if (strictBlockEnd)
	{
		Messenger::print("Error : Unterminated 'FitResults' block.\n");
		return false;
	}

	return true;
}

// Read SettingsBlock keywords
bool UChromaBase::readSettingsBlock(LineParser& parser, bool strictBlockEnd)
{
	UChromaBase::ImageFormat fmt;
	while (!parser.eofOrBlank())
	{
		// Get line from file
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks + LineParser::SemiColonLineBreaks);

		// Get keyword and check number of arguments provided
		UChromaBase::SettingsKeyword settingsKwd = UChromaBase::settingsKeyword(parser.argc(0));
		if ((settingsKwd != UChromaBase::nSettingsKeywords) && (UChromaBase::settingsKeywordNArguments(settingsKwd) > (parser.nArgs()-1)))
		{
			Messenger::print("Error : Settings keyword '%s' requires %i arguments, but only %i have been provided.\n", UChromaBase::settingsKeyword(settingsKwd), UChromaBase::settingsKeywordNArguments(settingsKwd), parser.nArgs()-1);
			return false;
		}
		switch (settingsKwd)
		{
			// End input block
			case (UChromaBase::EndSettingsKeyword):
				return true;
				break;
			// Image Export info
			case (UChromaBase::ImageExportKeyword):
				imageExportFileName_ = parser.argc(1);
				imageExportWidth_ = parser.argi(2);
				imageExportHeight_ = parser.argi(3);
				fmt = UChromaBase::imageFormat(parser.argc(4));
				if (fmt == UChromaBase::nImageFormats)
				{
					Messenger::warn("Unrecognised image format '%s'. Defaulting to '%s'.\n", parser.argc(4), UChromaBase::imageFormatExtension(UChromaBase::PNGFormat));
					fmt = UChromaBase::PNGFormat;
					CHECKIOFAIL
				}
				imageExportFormat_ = fmt;
				imageExportMaintainAspect_ = parser.argi(5);
				break;
			// Unrecognised Keyword
			default:
				Messenger::warn("Unrecognised settings keyword: %s\n", parser.argc(0));
				CHECKIOFAIL
				break;
		}
	}

	if (strictBlockEnd)
	{
		Messenger::print("Error : Unterminated 'Settings' block.\n");
		return false;
	}

	return true;
}

// Read ViewBlock keywords
bool UChromaBase::readViewBlock(LineParser& parser, bool strictBlockEnd)
{
	ViewPane* pane;
	while (!parser.eofOrBlank())
	{
		// Get line from file
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks + LineParser::SemiColonLineBreaks);

		// Get keyword and check number of arguments provided
		UChromaBase::ViewKeyword viewKwd = UChromaBase::viewKeyword(parser.argc(0));
		if ((viewKwd != UChromaBase::nViewKeywords) && (UChromaBase::viewKeywordNArguments(viewKwd) > (parser.nArgs()-1)))
		{
			Messenger::print("Error : View keyword '%s' requires %i arguments, but only %i have been provided.\n", UChromaBase::viewKeyword(viewKwd), UChromaBase::viewKeywordNArguments(viewKwd), parser.nArgs()-1);
			return false;
		}
		switch (viewKwd)
		{
			// End input block
			case (UChromaBase::EndViewKeyword):
				return true;
				break;
			// Grid specification
			case (UChromaBase::GridKeyword):
				viewLayout_.setGrid(parser.argi(1), parser.argi(2));
				break;
			// ViewPane definition
			case (UChromaBase::ViewPaneBlockKeyword):
				// Check to see if pane has already been created (through it being referenced by another pane)
				pane = viewLayout_.pane(parser.argc(1));
				if (!pane) pane = viewLayout_.addPane(parser.argc(1));
				if (!readViewPaneBlock(parser, pane)) return false;
				break;
			// Unrecognised Keyword
			default:
				Messenger::warn("Unrecognised view keyword: %s\n", parser.argc(0));
				CHECKIOFAIL
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

// Read ViewPane keywords
bool UChromaBase::readViewPaneBlock(LineParser& parser, ViewPane* pane, bool strictBlockEnd)
{
	int xyz, axis;
	Collection* collection;
	Matrix4 mat;
	ViewPane* associatedPane;
	ViewPane::PaneRole role;
	ViewPane::ViewType vt;
	ViewPane::AutoFollowType aft;
	while (!parser.eofOrBlank())
	{
		// Get line from file
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks + LineParser::SemiColonLineBreaks);

		// Get keyword and check number of arguments provided
		UChromaBase::ViewPaneKeyword viewPaneKwd = UChromaBase::viewPaneKeyword(parser.argc(0));
		if ((viewPaneKwd != UChromaBase::nViewPaneKeywords) && (UChromaBase::viewPaneKeywordNArguments(viewPaneKwd) > (parser.nArgs()-1)))
		{
			Messenger::error("ViewPane keyword '%s' requires %i arguments, but only %i have been provided.\n", UChromaBase::viewPaneKeyword(viewPaneKwd), UChromaBase::viewPaneKeywordNArguments(viewPaneKwd), parser.nArgs()-1);
			return false;
		}
		switch (viewPaneKwd)
		{
			// Auto-follow type
			case (UChromaBase::AutoFollowTypeKeyword):
				aft = ViewPane::autoFollowType(parser.argc(1));
				if (aft == ViewPane::nAutoFollowTypes)
				{
					Messenger::warn("Unrecognised auto-follow type '%s' for pane '%s'. Defaulting to 'None'.\n", parser.argc(1), pane->name());
					CHECKIOFAIL
				}
				pane->setAutoFollowType(aft);
				break;
			// Auto Position Axis Titles
			case (UChromaBase::AutoPositionTitlesKeyword):
				pane->axes().setAutoPositionTitles(parser.argb(1));
				break;
			// Axis block
			case (UChromaBase::AxisBlockKeyword):
				// Get target axis...
				axis = parser.argi(1);
				if ((axis < 0) || (axis > 2))
				{
					Messenger::warn("Axis index is out of range in input file. Defaults will be used.\n");
					CHECKIOFAIL
				}
				if (!readAxisBlock(parser, pane->axes(), axis)) return false;
				break;
			// Bounding Box
			case (UChromaBase::BoundingBoxKeyword):
				if ((parser.argi(1) < 0) || (parser.argi(1) >= ViewPane::nBoundingBoxes))
				{
					Messenger::warn("Value is out of range for %s keyword.\n", UChromaBase::viewPaneKeyword(viewPaneKwd));
					CHECKIOFAIL
				}
				pane->setBoundingBox((ViewPane::BoundingBox) parser.argi(1));
				break;
			// Bounding Box plane y intercept
			case (UChromaBase::BoundingBoxPlaneYKeyword):
				pane->setBoundingBoxPlaneY(parser.argd(1));
				break;
			// End input block
			case (UChromaBase::EndViewPaneKeyword):
				return true;
				break;
			// Flat labels flag
			case (UChromaBase::FlatLabelsKeyword):
				pane->setFlatLabelsIn3D(parser.argb(1));
				break;
			// Name
			case (UChromaBase::GeometryKeyword):
				pane->setBottomLeft(parser.argi(1), parser.argi(2));
				pane->setSize(parser.argi(3), parser.argi(4));
				break;
			// Label scale
			case (UChromaBase::LabelPointSizeKeyword):
				pane->setLabelPointSize(parser.argd(1));
				break;
			// Rotation
			case (UChromaBase::RotationXKeyword):
			case (UChromaBase::RotationYKeyword):
			case (UChromaBase::RotationZKeyword):
				xyz = viewPaneKwd - UChromaBase::RotationXKeyword;
				pane->setViewRotationColumn(xyz, parser.argd(1), parser.argd(2), parser.argd(3));
				break;
			// Perspective
			case (UChromaBase::PerspectiveKeyword):
				pane->setHasPerspective(parser.argb(1));
				break;
			// Role
			case (UChromaBase::RoleKeyword):
				role = ViewPane::paneRole(parser.argc(1));
				if (role == ViewPane::nPaneRoles)
				{
					Messenger::warn("Unrecognised role '%s' for pane '%s'. Defaulting to '%s'.\n", parser.argc(1), qPrintable(pane->name()), ViewPane::paneRole(ViewPane::StandardRole));
					role = ViewPane::StandardRole;
					CHECKIOFAIL
				}
				pane->setRole(role);
				break;
			// Role associated collection
			case (UChromaBase::RoleTargetCollectionKeyword):
				// Locate named collection
				collection = locateCollection(parser.argc(1));
				if (!collection)
				{
					Messenger::warn("Collection '%s' not found, and can't be associated to pane '%s'.\n", parser.argc(1), qPrintable(pane->name()));
					CHECKIOFAIL
				}
				pane->addCollectionTarget(collection);
				break;
			// Role target pane
			case (UChromaBase::RoleTargetPaneKeyword):
				associatedPane = viewLayout_.pane(parser.argc(1));
				if (!associatedPane) associatedPane = viewLayout_.addPane(parser.argc(1));
				pane->addPaneTarget(associatedPane);
				break;
			// Title scale
			case (UChromaBase::TitlePointSizeKeyword):
				pane->setTitlePointSize(parser.argd(1));
				break;
			// Translation
			case (UChromaBase::TranslationKeyword):
				pane->setViewTranslation(parser.argd(1), parser.argd(2), parser.argd(3));
				break;
			// Use best flat view
			case (UChromaBase::UseBestFlatViewKeyword):
				pane->axes().setUseBestFlatView(parser.argb(1));
				break;
			// Vertical shift (collection group manager)
			case (UChromaBase::VerticalShiftKeyword):
				pane->collectionGroupManager().setVerticalShift((CollectionGroupManager::VerticalShift) parser.argi(1));
				break;
			// View Type
			case (UChromaBase::ViewTypeKeyword):
				vt = ViewPane::viewType(parser.argc(1));
				if (vt == ViewPane::nViewTypes)
				{
					Messenger::warn("Unrecognised view type '%s'. Defaulting to '%s'.\n", parser.argc(1), ViewPane::viewType(ViewPane::NormalView));
					vt = ViewPane::NormalView;
					CHECKIOFAIL
				}
				pane->setViewType(vt);
				break;
			// Unrecognised Keyword
			default:
				Messenger::warn("Unrecognised ViewPane keyword: %s\n", parser.argc(0));
				CHECKIOFAIL
				break;
		}
	}

	if (strictBlockEnd)
	{
		Messenger::print("Error : Unterminated 'ViewPane' block.\n");
		return false;
	}

	return true;
}

// Set whether to enforce hard fail on input file error
void UChromaBase::setHardIOFail(bool hardFail)
{
	hardIOFail_ = hardFail;
}

// Load session from file specified
bool UChromaBase::loadSession(const char* fileName)
{
	LineParser parser;
	parser.openInput(fileName);

	if (!parser.isFileGoodForReading())
	{
		QMessageBox::warning(parent_, "Error", "Can't open specified file for reading.");
		return false;
	}

	// Clear existing data
	startNewSession(false);

	// Set input file directory
	sessionFileDirectory_ = fileName;

	// Parse input blocks
	bool success = parseInputBlocks(parser);
	parser.closeFiles();

	// Show a message if we encountered problems...
	if (!success)
	{
		QMessageBox::warning(parent_, "Problems Loading File", "Errors were encountered while loading the file.\nCheck the Log window for possible error messages.");
	}

	// Set necessary variables
	currentViewPane_ = viewLayout_.panes();

	// Set current project data
	setInputFile(fileName);
	setAsNotModified();

	return success;
}

// Parse main input blocks through specified parser
bool UChromaBase::parseInputBlocks(LineParser& parser)
{
	// Read line from file and decide what to do with it
	UChromaBase::InputBlock block;
	bool success = true;
	while (!parser.eofOrBlank())
	{
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks + LineParser::SemiColonLineBreaks);

		// We expect a block keyword in this loop...
		block = UChromaBase::inputBlock(parser.argc(0));
		switch (block)
		{
			// Collection Block
			case (UChromaBase::CollectionBlock):
				// Create new master Collection and set its title
				currentCollection_ = addCollection(parser.argc(1));

				// Load the collection data
				success = readCollectionBlock(parser, currentCollection_);

				// Set it for display in the current ViewPane
				if (currentViewPane_) currentViewPane_->addCollectionTarget(currentCollection_);
				break;
			// End of UChroma Input
			case (UChromaBase::EndUChromaBlock):
				return true;
				break;
			// Settings
			case (UChromaBase::SettingsBlock):
				success = readSettingsBlock(parser);
				break;
			// View
			case (UChromaBase::ViewBlock):
				success = readViewBlock(parser);
				break;
			default:
				Messenger::warn("Unrecognised input block: %s\n", parser.argc(0));
				CHECKIOFAIL
				break;
		}

		// If we have failed for any reason, exit now
		if (!success) break;
	}

	return success;
}

// Read session through parser specified
bool UChromaBase::readSession(LineParser& parser)
{
	// Clear existing data
	startNewSession(false);

	// Parse input blocks
	bool success = parseInputBlocks(parser);

	// Show a message if we encountered problems...
	if (!success) Messenger::warn("Errors were encountered while reading the session data.\n");

	// Set necessary variables
	currentViewPane_ = viewLayout_.panes();

	// Set current project data
	setAsNotModified();

	return success;
}
