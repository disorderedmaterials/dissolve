/*
	*** Session Save
	*** src/gui/uchroma/session/save.cpp
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
bool UChromaBase::writeAxisBlock(LineParser& parser, Axes& axes, int axis)
{
	parser.writeLineF("    %s %i\n", UChromaBase::viewPaneKeyword(UChromaBase::AxisBlockKeyword), axis);
	parser.writeLineF("      %s %s\n", UChromaBase::axisKeyword(UChromaBase::AutoScaleKeyword), Axes::autoScaleMethod(axes.autoScale(axis)));
	parser.writeLineF("      %s %s\n", UChromaBase::axisKeyword(UChromaBase::AutoTicksKeyword), stringBool(axes.autoTicks(axis)));
	parser.writeLineF("      %s %f\n", UChromaBase::axisKeyword(UChromaBase::FirstTickKeyword), axes.tickFirst(axis));
	parser.writeLineF("      %s %s\n", UChromaBase::axisKeyword(UChromaBase::FractionalPositioningKeyword), stringBool(axes.positionIsFractional(axis)));
	parser.writeLineF("      %s %s %s %s\n", UChromaBase::axisKeyword(UChromaBase::GridLinesKeyword), stringBool(axes.gridLinesMajor(axis)), stringBool(axes.gridLinesMinor(axis)), stringBool(axes.gridLinesFull(axis)));
	LineStyle style = axes.gridLineMajorStyle(axis);
	parser.writeLineF("      %s %f '%s' %f %f %f %f\n", UChromaBase::axisKeyword(UChromaBase::GridLineMajorStyleKeyword), style.width(), LineStipple::stipple[style.stipple()].name, style.colour().redF(), style.colour().greenF(), style.colour().blueF(), style.colour().alphaF());
	style = axes.gridLineMinorStyle(axis);
	parser.writeLineF("      %s %f '%s' %f %f %f %f\n", UChromaBase::axisKeyword(UChromaBase::GridLineMinorStyleKeyword), style.width(), LineStipple::stipple[style.stipple()].name, style.colour().redF(), style.colour().greenF(), style.colour().blueF(), style.colour().alphaF());
	parser.writeLineF("      %s %s\n", UChromaBase::axisKeyword(UChromaBase::InvertKeyword), stringBool(axes.inverted(axis)));
	parser.writeLineF("      %s %s\n", UChromaBase::axisKeyword(UChromaBase::LabelAnchorKeyword), TextPrimitive::textAnchor(axes.labelAnchor(axis)));
	parser.writeLineF("      %s %f %f %f\n", UChromaBase::axisKeyword(UChromaBase::LabelOrientationKeyword), axes.labelOrientation(axis).x, axes.labelOrientation(axis).y, axes.labelOrientation(axis).z);
	parser.writeLineF("      %s %f %f\n", UChromaBase::axisKeyword(UChromaBase::LimitsKeyword), axes.min(axis), axes.max(axis));
	parser.writeLineF("      %s %s\n", UChromaBase::axisKeyword(UChromaBase::LogarithmicKeyword), stringBool(axes.logarithmic(axis)));
	parser.writeLineF("      %s %i\n", UChromaBase::axisKeyword(UChromaBase::MinorTicksKeyword), axes.minorTicks(axis));
	NumberFormat fmt = axes.numberFormat(axis);
	parser.writeLineF("      %s '%s' %i %s %s\n", UChromaBase::axisKeyword(UChromaBase::NumberFormatKeyword), NumberFormat::formatType(fmt.type()), fmt.nDecimals(), stringBool(fmt.useUpperCaseExponent()), stringBool(fmt.forcePrecedingPlus()));
	parser.writeLineF("      %s %f %f %f\n", UChromaBase::axisKeyword(UChromaBase::PositionFractionalKeyword), axes.positionFractional(axis).x, axes.positionFractional(axis).y, axes.positionFractional(axis).z);
	parser.writeLineF("      %s %f %f %f\n", UChromaBase::axisKeyword(UChromaBase::PositionRealKeyword), axes.positionReal(axis).x, axes.positionReal(axis).y, axes.positionReal(axis).z);
	parser.writeLineF("      %s %f\n", UChromaBase::axisKeyword(UChromaBase::StretchKeyword), axes.stretch(axis));
	parser.writeLineF("      %s %f\n", UChromaBase::axisKeyword(UChromaBase::TickDeltaKeyword), axes.tickDelta(axis));
	parser.writeLineF("      %s %f %f %f\n", UChromaBase::axisKeyword(UChromaBase::TickDirectionKeyword), axes.tickDirection(axis).x, axes.tickDirection(axis).y, axes.tickDirection(axis).z);
	parser.writeLineF("      %s %s\n", UChromaBase::axisKeyword(UChromaBase::TitleAnchorKeyword), TextPrimitive::textAnchor(axes.titleAnchor(axis)));
	parser.writeLineF("      %s '%s'\n", UChromaBase::axisKeyword(UChromaBase::TitleKeyword), axes.title(axis));
	parser.writeLineF("      %s %f %f %f %f\n", UChromaBase::axisKeyword(UChromaBase::TitleOrientationKeyword), axes.titleOrientation(axis).x, axes.titleOrientation(axis).y, axes.titleOrientation(axis).z, axes.titleOrientation(axis).w);
	parser.writeLineF("      %s %s\n", UChromaBase::axisKeyword(UChromaBase::VisibleAxisKeyword), stringBool(axes.visible(axis)));
	parser.writeLineF("    %s\n", UChromaBase::axisKeyword(UChromaBase::EndAxisKeyword));

	return true;
}

// Write CollectionBlock keywords
bool UChromaBase::writeCollectionBlock(LineParser& parser, Collection* collection, Collection::CollectionType type, int indentLevel)
{
	// Construct indent string
	char* indent = new char[indentLevel*2+1];
	for (int n=0; n<indentLevel*2; ++n) indent[n] = ' ';
	indent[indentLevel*2] = '\0';

	if (type == Collection::MasterCollection) parser.writeLineF("%s%s '%s'\n", indent, UChromaBase::inputBlock(UChromaBase::CollectionBlock), collection->name());
	else if (type == Collection::FitCollection) parser.writeLineF("%s%s '%s'\n", indent, UChromaBase::collectionKeyword(UChromaBase::FitBlockKeyword), collection->name());
	else if (type == Collection::ExtractedCollection) parser.writeLineF("%s%s '%s'\n", indent, UChromaBase::collectionKeyword(UChromaBase::SliceBlockKeyword), collection->name());
	parser.writeLineF("%s  %s \"%s\"\n", indent, UChromaBase::collectionKeyword(UChromaBase::DataDirectoryKeyword), qPrintable(collection->dataFileDirectory().absolutePath()));

	// -- Transforms
	parser.writeLineF("%s  %s %s %s\n", indent, UChromaBase::collectionKeyword(UChromaBase::TransformXKeyword), stringBool(collection->transformEnabled(0)), collection->transformEquation(0));
	parser.writeLineF("%s  %s %s %s\n", indent, UChromaBase::collectionKeyword(UChromaBase::TransformYKeyword), stringBool(collection->transformEnabled(1)), collection->transformEquation(1));
	parser.writeLineF("%s  %s %s %s\n", indent, UChromaBase::collectionKeyword(UChromaBase::TransformZKeyword), stringBool(collection->transformEnabled(2)), collection->transformEquation(2));

	// -- Interpolation
	parser.writeLineF("%s  %s %s %s\n", indent, UChromaBase::collectionKeyword(UChromaBase::InterpolateKeyword), stringBool(collection->interpolate(0)), stringBool(collection->interpolate(2)));
	parser.writeLineF("%s  %s %s %s\n", indent, UChromaBase::collectionKeyword(UChromaBase::InterpolateConstrainKeyword), stringBool(collection->interpolateConstrained(0)), stringBool(collection->interpolateConstrained(2)));
	parser.writeLineF("%s  %s %f %f\n", indent, UChromaBase::collectionKeyword(UChromaBase::InterpolateStepKeyword), collection->interpolationStep(0), collection->interpolationStep(2));

	// Colour Setup
	ColourDefinition& colourDef = collection->colour();
	parser.writeLineF("%s  %s '%s'\n", indent, UChromaBase::collectionKeyword(UChromaBase::ColourSourceKeyword), ColourDefinition::colourSource(colourDef.colourSource()));
	ColourScalePoint* csp;
	QColor colour;
	double value;
	// -- Single Colour
	colour = colourDef.colourScalePointColour(ColourDefinition::SingleColourSource);
	parser.writeLineF("%s  %s %i %i %i %i\n", indent, UChromaBase::collectionKeyword(UChromaBase::ColourSingleKeyword), colour.red(), colour.green(), colour.blue(), colour.alpha());
	// -- RGB Gradient
	colour = colourDef.colourScalePointColour(ColourDefinition::RGBGradientSource, 0);
	value = colourDef.colourScalePointValue(ColourDefinition::RGBGradientSource, 0);
	parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, UChromaBase::collectionKeyword(UChromaBase::ColourRGBGradientAKeyword), value, colour.red(), colour.green(), colour.blue(), colour.alpha());
	colour = colourDef.colourScalePointColour(ColourDefinition::RGBGradientSource, 1);
	value = colourDef.colourScalePointValue(ColourDefinition::RGBGradientSource, 1);
	parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, UChromaBase::collectionKeyword(UChromaBase::ColourRGBGradientBKeyword), value, colour.red(), colour.green(), colour.blue(), colour.alpha());
	// -- HSV Gradient
	colour = colourDef.colourScalePointColour(ColourDefinition::HSVGradientSource, 0);
	value = colourDef.colourScalePointValue(ColourDefinition::HSVGradientSource, 0);
	parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, UChromaBase::collectionKeyword(UChromaBase::ColourHSVGradientAKeyword), value, colour.hue(), colour.saturation(), colour.value(), colour.alpha());
	colour = colourDef.colourScalePointColour(ColourDefinition::HSVGradientSource, 1);
	value = colourDef.colourScalePointValue(ColourDefinition::HSVGradientSource, 1);
	parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, UChromaBase::collectionKeyword(UChromaBase::ColourHSVGradientBKeyword), value, colour.hue(), colour.saturation(), colour.value(), colour.alpha());
	// -- Custom Gradient
	for (csp = colourDef.customColourScalePoints(); csp != NULL; csp = csp->next)
	{
		parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, UChromaBase::collectionKeyword(UChromaBase::ColourCustomGradientKeyword), csp->value(), csp->colour().red(), csp->colour().green(), csp->colour().blue(), csp->colour().alpha());
	}
	// -- Alpha control
	parser.writeLineF("%s  %s '%s'\n", indent, UChromaBase::collectionKeyword(UChromaBase::ColourAlphaControlKeyword), ColourDefinition::alphaControl(colourDef.alphaControl()));
	parser.writeLineF("%s  %s %f\n", indent, UChromaBase::collectionKeyword(UChromaBase::ColourAlphaFixedKeyword), colourDef.fixedAlpha());

	// Display
	parser.writeLineF("%s  %s %f '%s'\n", indent, UChromaBase::collectionKeyword(UChromaBase::LineStyleKeyword), collection->displayLineStyle().width(), LineStipple::stipple[collection->displayLineStyle().stipple()].name);
	parser.writeLineF("%s  %s %f\n", indent, UChromaBase::collectionKeyword(UChromaBase::ShininessKeyword), collection->displaySurfaceShininess());
	parser.writeLineF("%s  %s %s\n", indent, UChromaBase::collectionKeyword(UChromaBase::StyleKeyword), Collection::displayStyle(collection->displayStyle()));
	parser.writeLineF("%s  %s %s\n", indent, UChromaBase::collectionKeyword(UChromaBase::VisibleCollectionKeyword), stringBool(collection->visible()));

	// Loop over datasets
	for (DataSet* dataSet = collection->dataSets(); dataSet != NULL; dataSet = dataSet->next) writeDataSetBlock(parser, dataSet, indentLevel);

	// Write Group if set
	if (collection->hasGroupName()) parser.writeLineF("%s  %s '%s'\n", indent, UChromaBase::collectionKeyword(UChromaBase::GroupKeyword), collection->groupName());

	// Write FitKernel data if present
	if (collection->fitKernel()) writeFitParametersBlock(parser, collection->fitKernel(), indentLevel);

	// Additional data
	// -- Fits
	for (Collection* fit = collection->fits(); fit != NULL; fit = fit->next) writeCollectionBlock(parser, fit, Collection::FitCollection, indentLevel+1);
	// -- Extracted Data
	for (Collection* extract = collection->slices(); extract != NULL; extract = extract->next) writeCollectionBlock(parser, extract, Collection::ExtractedCollection, indentLevel+1);

	parser.writeLineF("%s%s\n", indent, UChromaBase::collectionKeyword(UChromaBase::EndCollectionKeyword));

	return true;
}

// Write DataSetBlock keywords
bool UChromaBase::writeDataSetBlock(LineParser& parser, DataSet* dataSet, int indentLevel)
{
	// Construct indent string
	char* indent = new char[indentLevel*2+1];
	for (int n=0; n<indentLevel*2; ++n) indent[n] = ' ';
	indent[indentLevel*2] = '\0';

	parser.writeLineF("%s  %s '%s'\n", indent, UChromaBase::collectionKeyword(UChromaBase::DataSetDefinitionKeyword), dataSet->name());
	if (dataSet->dataSource() == DataSet::FileSource) parser.writeLineF("%s    %s %s '%s'\n", indent, UChromaBase::dataSetKeyword(UChromaBase::SourceKeyword), DataSet::dataSource(dataSet->dataSource()), dataSet->sourceFileName());
	else if (dataSet->dataSource() == DataSet::XYDataSource)  parser.writeLineF("%s    %s %s '%s'\n", indent, UChromaBase::dataSetKeyword(UChromaBase::SourceKeyword), DataSet::dataSource(dataSet->dataSource()), dataSet->sourceXYData());
	else parser.writeLineF("%s    %s %s\n", indent, UChromaBase::dataSetKeyword(UChromaBase::SourceKeyword), DataSet::dataSource(dataSet->dataSource()));
	parser.writeLineF("%s    %s %f\n", indent, UChromaBase::dataSetKeyword(UChromaBase::ZKeyword), dataSet->data().constZ());
	if (dataSet->dataSource() == DataSet::InternalSource)
	{
		parser.writeLineF("%s    %s\n", indent, UChromaBase::dataSetKeyword(UChromaBase::DataKeyword));
		for (int n=0; n< dataSet->data().nPoints(); ++n) parser.writeLineF("%s      %f  %f\n", indent, dataSet->data().x(n), dataSet->data().y(n));
		parser.writeLineF("%s    End%s\n", indent, UChromaBase::dataSetKeyword(UChromaBase::DataKeyword));
	}
	parser.writeLineF("%s  %s\n", indent, UChromaBase::dataSetKeyword(UChromaBase::EndDataSetKeyword));

	return true;
}

// Write FitParametersBlock keywords
bool UChromaBase::writeFitParametersBlock(LineParser& parser, FitKernel* fitKernel, int indentLevel)
{
	// Construct indent string
	char* indent = new char[indentLevel*2+1];
	for (int n=0; n<indentLevel*2; ++n) indent[n] = ' ';
	indent[indentLevel*2] = '\0';

	parser.writeLineF("%s  %s\n", indent, UChromaBase::collectionKeyword(UChromaBase::FitParametersBlockKeyword));
	for (RefListItem<ReferenceVariable,bool>* ri = fitKernel->usedReferences(); ri != NULL; ri = ri->next)
	{
		ReferenceVariable* refVar = ri->item;
		parser.writeLineF("%s    %s %s %s %i %i %s %i %i '%s'\n", indent, UChromaBase::fitParametersKeyword(UChromaBase::ReferenceKeyword), refVar->name(), IndexData::indexType(refVar->xIndex().type()), refVar->xIndex().index(), refVar->xIndex().offset(), IndexData::indexType(refVar->zIndex().type()), refVar->zIndex().index(), refVar->zIndex().offset(), refVar->zDataSetName());
	}
	parser.writeLineF("%s    %s '%s'\n", indent, UChromaBase::fitParametersKeyword(UChromaBase::EquationKeyword), fitKernel->equationText());
	parser.writeLineF("%s    %s %s\n", indent, UChromaBase::fitParametersKeyword(UChromaBase::GlobalKeyword), stringBool(fitKernel->global()));
	parser.writeLineF("%s    %s %s\n", indent, UChromaBase::fitParametersKeyword(UChromaBase::OrthogonalKeyword), stringBool(fitKernel->orthogonal()));
	parser.writeLineF("%s    %s %f\n", indent, UChromaBase::fitParametersKeyword(UChromaBase::LimitStrengthKeyword), fitKernel->limitStrength());
	for (RefListItem<EquationVariable,bool>* ri = fitKernel->usedVariables(); ri != NULL; ri = ri->next)
	{
		EquationVariable* eqVar = ri->item;
		parser.writeLineF("%s    %s %s %s %f %s %f %s %f\n", indent, UChromaBase::fitParametersKeyword(UChromaBase::VariableKeyword), eqVar->name(), stringBool(eqVar->fit()), eqVar->value(), stringBool(eqVar->maximumLimitEnabled()), eqVar->minimumLimit(), stringBool(eqVar->maximumLimitEnabled()), eqVar->maximumLimit());
	}
	parser.writeLineF("%s    %s %s\n", indent, UChromaBase::fitParametersKeyword(UChromaBase::XRangeTypeKeyword), FitKernel::rangeType(fitKernel->xRange()));
	parser.writeLineF("%s    %s %f %f\n", indent, UChromaBase::fitParametersKeyword(UChromaBase::XRangeAbsoluteKeyword), fitKernel->absoluteXMin(), fitKernel->absoluteXMax());
	parser.writeLineF("%s    %s %i %i\n", indent, UChromaBase::fitParametersKeyword(UChromaBase::XRangeIndexKeyword), fitKernel->indexXMin()+1, fitKernel->indexXMax()+1);
	parser.writeLineF("%s    %s %i\n", indent, UChromaBase::fitParametersKeyword(UChromaBase::XRangeIndexSingleKeyword), fitKernel->indexXSingle()+1);
	parser.writeLineF("%s    %s %s\n", indent, UChromaBase::fitParametersKeyword(UChromaBase::ZRangeTypeKeyword), FitKernel::rangeType(fitKernel->zRange()));
	parser.writeLineF("%s    %s %f %f\n", indent, UChromaBase::fitParametersKeyword(UChromaBase::ZRangeAbsoluteKeyword), fitKernel->absoluteZMin(), fitKernel->absoluteZMax());
	parser.writeLineF("%s    %s %i %i\n", indent, UChromaBase::fitParametersKeyword(UChromaBase::ZRangeIndexKeyword), fitKernel->indexZMin()+1, fitKernel->indexZMax()+1);
	parser.writeLineF("%s    %s %i\n", indent, UChromaBase::fitParametersKeyword(UChromaBase::ZRangeIndexSingleKeyword), fitKernel->indexZSingle()+1);

	// Write fit results for each range
	int count = 1;
	for (DataSpaceRange* range = fitKernel->dataSpaceRanges(); range != NULL; range = range->next) writeFitResultsBlock(parser, range, count++, indentLevel+1);

	parser.writeLineF("%s  %s\n", indent, UChromaBase::fitParametersKeyword(UChromaBase::EndFitParametersKeyword));

	return true;
}

// Write FitResultsBlock keywords
bool UChromaBase::writeFitResultsBlock(LineParser& parser, DataSpaceRange* range, int rangeID, int indentLevel)
{
	// Construct indent string
	char* indent = new char[indentLevel*2+1];
	for (int n=0; n<indentLevel*2; ++n) indent[n] = ' ';
	indent[indentLevel*2] = '\0';

	parser.writeLineF("%s  %s %i\n", indent, UChromaBase::fitParametersKeyword(UChromaBase::FitResultsBlockKeyword), rangeID);
	for (NamedValue* value = range->fittedValues(); value != NULL; value = value->next) parser.writeLineF("%s    %s %s %e\n", indent, UChromaBase::fitResultsKeyword(UChromaBase::FittedValueKeyword), value->name(), value->value());
	parser.writeLineF("%s  %s\n", indent, UChromaBase::fitResultsKeyword(UChromaBase::EndFitResultsKeyword));

	return true;
}

// Write SettingsBlock keywords
bool UChromaBase::writeSettingsBlock(LineParser& parser)
{
	parser.writeLineF("%s\n", UChromaBase::inputBlock(UChromaBase::SettingsBlock));
	parser.writeLineF("  %s \"%s\" %i %i %s %i\n", UChromaBase::settingsKeyword(UChromaBase::ImageExportKeyword), imageExportFileName_.get(), imageExportWidth_, imageExportHeight_, UChromaBase::imageFormatExtension(imageExportFormat_), imageExportMaintainAspect_);
	parser.writeLineF("%s\n", UChromaBase::settingsKeyword(UChromaBase::EndSettingsKeyword));

	return true;
}

// Write ViewBlock keywords
bool UChromaBase::writeViewBlock(LineParser& parser)
{
	parser.writeLineF("%s\n", UChromaBase::inputBlock(UChromaBase::ViewBlock));
	parser.writeLineF("  %s %i %i\n", UChromaBase::viewKeyword(UChromaBase::GridKeyword), viewLayout_.nColumns(), viewLayout_.nRows());

	// Loop over defined panes
	for (ViewPane* pane = viewLayout_.panes(); pane != NULL; pane = pane->next) writeViewPaneBlock(parser, pane);

	parser.writeLineF("%s\n", UChromaBase::viewKeyword(UChromaBase::EndViewKeyword));

	return true;
}

// Write ViewPaneBlock keywords
bool UChromaBase::writeViewPaneBlock(LineParser& parser, ViewPane* pane)
{
	parser.writeLineF("  %s '%s'\n", UChromaBase::viewKeyword(UChromaBase::ViewPaneBlockKeyword), pane->name());
	parser.writeLineF("    %s %s\n", UChromaBase::viewPaneKeyword(UChromaBase::AutoFollowTypeKeyword), ViewPane::autoFollowType(pane->autoFollowType()));
	parser.writeLineF("    %s %s\n", UChromaBase::viewPaneKeyword(UChromaBase::AutoPositionTitlesKeyword), stringBool(pane->axes().autoPositionTitles()));
	for (int axis=0; axis < 3; ++axis) writeAxisBlock(parser, pane->axes(), axis);
	parser.writeLineF("    %s %i\n", UChromaBase::viewPaneKeyword(UChromaBase::BoundingBoxKeyword), pane->boundingBox());
	parser.writeLineF("    %s %f\n", UChromaBase::viewPaneKeyword(UChromaBase::BoundingBoxPlaneYKeyword), pane->boundingBoxPlaneY());
	parser.writeLineF("    %s %s\n", UChromaBase::viewPaneKeyword(UChromaBase::FlatLabelsKeyword), stringBool(pane->flatLabelsIn3D()));
	parser.writeLineF("    %s %i %i %i %i\n", UChromaBase::viewPaneKeyword(UChromaBase::GeometryKeyword), pane->bottomEdge(), pane->leftEdge(), pane->width(), pane->height()); 
	parser.writeLineF("    %s %f\n", UChromaBase::viewPaneKeyword(UChromaBase::LabelPointSizeKeyword), pane->labelPointSize());
	parser.writeLineF("    %s %f\n", UChromaBase::viewPaneKeyword(UChromaBase::TitlePointSizeKeyword), pane->titlePointSize());
	Matrix4 mat = pane->viewRotation();
	Vec3<double> trans = pane->viewTranslation();
	parser.writeLineF("    %s %f %f %f\n", UChromaBase::viewPaneKeyword(UChromaBase::RotationXKeyword), mat[0], mat[1], mat[2]);
	parser.writeLineF("    %s %f %f %f\n", UChromaBase::viewPaneKeyword(UChromaBase::RotationYKeyword), mat[4], mat[5], mat[6]);
	parser.writeLineF("    %s %f %f %f\n", UChromaBase::viewPaneKeyword(UChromaBase::RotationZKeyword), mat[8], mat[9], mat[10]);
	parser.writeLineF("    %s %f %f %f\n", UChromaBase::viewPaneKeyword(UChromaBase::TranslationKeyword), trans.x, trans.y, trans.z);
	parser.writeLineF("    %s %s\n", UChromaBase::viewPaneKeyword(UChromaBase::PerspectiveKeyword), stringBool(pane->hasPerspective()));
	parser.writeLineF("    %s '%s'\n", UChromaBase::viewPaneKeyword(UChromaBase::RoleKeyword), ViewPane::paneRole(pane->role()));
	for (TargetData* target = pane->collectionTargets(); target != NULL; target = target->next)
	{
		if (!Collection::objectValid(target->collection(), "collection in UChromaBase::writeViewPaneBlock")) continue;
		parser.writeLineF("    %s '%s'\n", UChromaBase::viewPaneKeyword(UChromaBase::RoleTargetCollectionKeyword), target->collection()->locator());
	}
	for (RefListItem<ViewPane,bool>* ri = pane->paneTargets(); ri != NULL; ri = ri->next) parser.writeLineF("    %s '%s'\n", UChromaBase::viewPaneKeyword(UChromaBase::RoleTargetPaneKeyword), ri->item->name());
	parser.writeLineF("    %s %s\n", UChromaBase::viewPaneKeyword(UChromaBase::UseBestFlatViewKeyword), stringBool(pane->axes().useBestFlatView()));
	parser.writeLineF("    %s %i\n", UChromaBase::viewPaneKeyword(UChromaBase::VerticalShiftKeyword), pane->collectionGroupManager().verticalShift());
	parser.writeLineF("    %s '%s'\n", UChromaBase::viewPaneKeyword(UChromaBase::ViewTypeKeyword), ViewPane::viewType(pane->viewType()));
	parser.writeLineF("  %s\n", UChromaBase::viewPaneKeyword(UChromaBase::EndViewPaneKeyword));

	return true;
}

// Save current data to file specified
bool UChromaBase::saveSession(const char* fileName)
{
	LineParser parser;
	parser.openOutput(fileName, true);
	if (!parser.isFileGoodForWriting())
	{
		QMessageBox::warning(parent_, "Error", "Can't open specified file for writing.");
		return false;
	}

	// Write Settings Data
	writeSettingsBlock(parser);

	// Write Collection Data
	for (Collection* collection = collections_.first(); collection != NULL; collection = collection->next) writeCollectionBlock(parser, collection);

	// Write View Data
	writeViewBlock(parser);

	parser.closeFiles();
	return true;
}

// Write session through parser specified
bool UChromaBase::writeSession(LineParser& parser)
{
	// Write Settings Data
	if (!writeSettingsBlock(parser)) return false;

	// Write Collection Data
	for (Collection* collection = collections_.first(); collection != NULL; collection = collection->next) if (!writeCollectionBlock(parser, collection)) return false;

	// Write View Data
	if (!writeViewBlock(parser)) return false;

	// Write end of UChroma input marker
	if (!parser.writeLineF("EndUChroma\n")) return false;

	return true;
}
