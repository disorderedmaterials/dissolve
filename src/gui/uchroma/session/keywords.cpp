/*
	*** Session Keywords
	*** src/gui/uchroma/session/keywords.cpp
	Copyright T. Youngs 2013-2017

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
#include "base/sysfunc.h"
#include "base/messenger.h"
#include <string.h>

/*
 * Input Block Keywords
 */

// Input File Block Keywords
const char* InputBlockKeywords[] = { "Collection", "Settings", "View" };

// Convert text string to InputBlock
UChromaBase::InputBlock UChromaBase::inputBlock(const char* s)
{
	for (int n=0; n<UChromaBase::nInputBlocks; ++n) if (DUQSys::sameString(s, InputBlockKeywords[n])) return (UChromaBase::InputBlock) n;
	return UChromaBase::nInputBlocks;
}

// Convert InputBlock to text string
const char* UChromaBase::inputBlock(UChromaBase::InputBlock id)
{
	return InputBlockKeywords[id];
}

/*
 * Axis Keywords
 */

// Axis Block Keywords
const char* AxisBlockKeywords[] = { "AutoScale", "AutoTicks", "EndAxis", "FirstTick", "FractionalPositioning", "GridLines", "GridLineMajorStyle", "GridLineMinorStyle", "Invert", "LabelAnchor", "LabelOrientation", "Limits", "Logarithmic", "MinorTicks", "NumberFormat", "PositionFractional", "PositionReal", "Stretch", "TickDelta", "TickDirection", "Title", "TitleAnchor", "TitleOrientation", "Visible" };

// Axis Block NArguments
int AxisKeywordNArguments[] = { 1, 1, 0, 1, 1, 3, 6, 6, 1, 1, 3, 2, 1, 1, 4, 3, 3, 1, 1, 3, 1, 1, 4, 1 };

// Convert text string to AxisKeyword
UChromaBase::AxisKeyword UChromaBase::axisKeyword(const char* s)
{
	for (int n=0; n<nAxisKeywords; ++n) if (DUQSys::sameString(s, AxisBlockKeywords[n])) return (UChromaBase::AxisKeyword) n;
	return nAxisKeywords;
}

// Convert AxisBlockKeywords to text string
const char* UChromaBase::axisKeyword(UChromaBase::AxisKeyword kwd)
{
	return AxisBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int UChromaBase::axisKeywordNArguments(UChromaBase::AxisKeyword kwd)
{
	return AxisKeywordNArguments[kwd];
}

/*
 * Collection Keywords
 */

// Collection Block Keywords
const char* CollectionBlockKeywords[] = { "ColourAlphaControl", "ColourAlphaFixed", "ColourCustomGradient", "ColourLinearRGBA", "ColourLinearRGBB", "ColourLinearHSVA", "ColourLinearHSVB", "ColourSingle", "ColourSource", "DataDirectory", "DataSet", "EndCollection", "Fit", "FitParameters", "Interpolate", "InterpolateConstrain", "InterpolateStep", "LineStyle", "Shininess", "Slice", "Style", "TransformX", "TransformY", "TransformZ", "Visible" };

// Collection Block NArguments
int CollectionKeywordNArguments[] = { 1, 1, 5, 5, 5, 5, 5, 4, 1, 1, 1, 0, 1, 0, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1 };

// Convert text string to CollectionKeyword
UChromaBase::CollectionKeyword UChromaBase::collectionKeyword(const char* s)
{
	for (int n=0; n<nCollectionKeywords; ++n) if (DUQSys::sameString(s, CollectionBlockKeywords[n])) return (UChromaBase::CollectionKeyword) n;
	return nCollectionKeywords;
}

// Convert CollectionKeyword to text string
const char* UChromaBase::collectionKeyword(UChromaBase::CollectionKeyword kwd)
{
	return CollectionBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int UChromaBase::collectionKeywordNArguments(UChromaBase::CollectionKeyword kwd)
{
	return CollectionKeywordNArguments[kwd];
}

/*
 * FitParameters Keywords
 */

// FitParameters Block Keywords
const char* FitParametersBlockKeywords[] = { "EndFitParameters", "Equation", "FitResults", "Global", "LimitStrength", "Orthogonal", "Reference", "Variable", "XRangeType", "XRangeAbsolute", "XRangeIndex", "XRangeSingle", "ZRangeType", "ZRangeAbsolute", "ZRangeIndex", "ZRangeSingle" };

// FitParameters Block NArguments
int FitParametersKeywordNArguments[] = { 0, 1, 1, 1, 1, 1, 7, 7, 1, 2, 2, 1, 1, 2, 2, 1 };

// Convert text string to FitParametersKeyword
UChromaBase::FitParametersKeyword UChromaBase::fitParametersKeyword(const char* s)
{
	for (int n=0; n<nFitParametersKeywords; ++n) if (DUQSys::sameString(s, FitParametersBlockKeywords[n])) return (UChromaBase::FitParametersKeyword) n;
	return nFitParametersKeywords;
}

// Convert FitParametersKeyword to text string
const char* UChromaBase::fitParametersKeyword(UChromaBase::FitParametersKeyword kwd)
{
	return FitParametersBlockKeywords[kwd];
}

 // Return minimum number of expected arguments
int UChromaBase::fitParametersKeywordNArguments(UChromaBase::FitParametersKeyword kwd)
{
	return FitParametersKeywordNArguments[kwd];
}

/*
 * FitResults Keywords
 */

// FitResults Block Keywords
const char* FitResultsBlockKeywords[] = { "EndFitResults", "Variable" };

// FitResults Block NArguments
int FitResultsKeywordNArguments[] = { 0, 1 };

// Convert text string to FitResultsKeyword
UChromaBase::FitResultsKeyword UChromaBase::fitResultsKeyword(const char* s)
{
	for (int n=0; n<nFitResultsKeywords; ++n) if (DUQSys::sameString(s, FitResultsBlockKeywords[n])) return (UChromaBase::FitResultsKeyword) n;
	return nFitResultsKeywords;
}

// Convert FitResultsKeyword to text string
const char* UChromaBase::fitResultsKeyword(UChromaBase::FitResultsKeyword kwd)
{
	return FitResultsBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int UChromaBase::fitResultsKeywordNArguments(UChromaBase::FitResultsKeyword kwd)
{
	return FitResultsKeywordNArguments[kwd];
}

/*
 * Settings Keywords
 */

// Settings Block Keywords
const char* SettingsBlockKeywords[] = { "EndSettings", "ImageExport" };

// Settings Block NArguments
int SettingsKeywordNArguments[] = { 0, 5 };

// Convert text string to SettingsKeyword
UChromaBase::SettingsKeyword UChromaBase::settingsKeyword(const char* s)
{
	for (int n=0; n<nSettingsKeywords; ++n) if (DUQSys::sameString(s, SettingsBlockKeywords[n])) return (UChromaBase::SettingsKeyword) n;
	return nSettingsKeywords;
}

// Convert SettingsKeyword to text string
const char* UChromaBase::settingsKeyword(UChromaBase::SettingsKeyword kwd)
{
	return SettingsBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int UChromaBase::settingsKeywordNArguments(UChromaBase::SettingsKeyword kwd)
{
	return SettingsKeywordNArguments[kwd];
}

/*
 * DataSet Keywords
 */

// DataSet Block Keywords
const char* DataSetBlockKeywords[] = { "Data", "EndDataSet", "Source", "Z" };

// DataSet Block NArguments
int DataSetKeywordNArguments[] = { 0, 0, 1, 1 };

// Convert text string to DataSetKeyword
UChromaBase::DataSetKeyword UChromaBase::dataSetKeyword(const char* s)
{
	for (int n=0; n<nDataSetKeywords; ++n) if (DUQSys::sameString(s, DataSetBlockKeywords[n])) return (UChromaBase::DataSetKeyword) n;
	return nDataSetKeywords;
}

// Convert DataSetKeyword to text string
const char* UChromaBase::dataSetKeyword(UChromaBase::DataSetKeyword kwd)
{
	return DataSetBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int UChromaBase::dataSetKeywordNArguments(UChromaBase::DataSetKeyword kwd)
{
	return DataSetKeywordNArguments[kwd];
}

/*
 * View Keywords
 */

// View Block Keywords
const char* ViewBlockKeywords[] = { "EndView", "Grid", "ViewPane" };

// View Block NArguments
int ViewKeywordNArguments[] = { 0, 2, 1 };

// Convert text string to ViewKeyword
UChromaBase::ViewKeyword UChromaBase::viewKeyword(const char* s)
{
	for (int n=0; n<nViewKeywords; ++n) if (DUQSys::sameString(s, ViewBlockKeywords[n])) return (UChromaBase::ViewKeyword) n;
	return nViewKeywords;
}

// Convert ViewKeyword to text string
const char* UChromaBase::viewKeyword(UChromaBase::ViewKeyword kwd)
{
	return ViewBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int UChromaBase::viewKeywordNArguments(UChromaBase::ViewKeyword kwd)
{
	return ViewKeywordNArguments[kwd];
}

/*
 * ViewPane Keywords
 */

// ViewPane Block Keywords
const char* ViewPaneBlockKeywords[] = { "AutoPositionTitles", "Axis", "BoundingBox", "BoundingBoxPlaneY", "EndViewPane", "FlatLabels", "Geometry", "LabelPointSize", "Perspective", "Role", "RoleTargetCollection", "RoleTargetPane", "RotationX", "RotationY", "RotationZ", "TitlePointSize", "Translation", "UseBestFlatView", "ViewType" };

// ViewPane Block NArguments
int ViewPaneKeywordNArguments[] = { 1, 1, 1, 1, 0, 1, 4, 1, 1, 1, 1, 1, 3, 3, 3, 1, 3, 1, 1 };

// Convert text string to ViewPaneKeyword
UChromaBase::ViewPaneKeyword UChromaBase::viewPaneKeyword(const char* s)
{
	for (int n=0; n<nViewPaneKeywords; ++n) if (DUQSys::sameString(s, ViewPaneBlockKeywords[n])) return (UChromaBase::ViewPaneKeyword) n;
	return nViewPaneKeywords;
}

// Convert ViewPaneKeyword to text string
const char* UChromaBase::viewPaneKeyword(UChromaBase::ViewPaneKeyword kwd)
{
	return ViewPaneBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int UChromaBase::viewPaneKeywordNArguments(UChromaBase::ViewPaneKeyword kwd)
{
	return ViewPaneKeywordNArguments[kwd];
}
