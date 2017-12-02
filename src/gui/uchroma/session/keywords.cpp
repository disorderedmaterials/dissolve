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
const char* UChromaInputBlockKeywords[] = { "Collection", "Settings", "View" };

// Convert text string to InputBlock
UChromaBase::InputBlock UChromaBase::inputBlock(const char* s)
{
	for (int n=0; n<UChromaBase::nInputBlocks; ++n) if (DUQSys::sameString(s, UChromaInputBlockKeywords[n])) return (UChromaBase::InputBlock) n;
	return UChromaBase::nInputBlocks;
}

// Convert InputBlock to text string
const char* UChromaBase::inputBlock(UChromaBase::InputBlock id)
{
	return UChromaInputBlockKeywords[id];
}

/*
 * Axis Keywords
 */

// Axis Block Keywords
const char* UChromaAxisBlockKeywords[] = { "AutoScale", "AutoTicks", "EndAxis", "FirstTick", "FractionalPositioning", "GridLines", "GridLineMajorStyle", "GridLineMinorStyle", "Invert", "LabelAnchor", "LabelOrientation", "Limits", "Logarithmic", "MinorTicks", "NumberFormat", "PositionFractional", "PositionReal", "Stretch", "TickDelta", "TickDirection", "Title", "TitleAnchor", "TitleOrientation", "Visible" };

// Axis Block NArguments
int UChromaAxisKeywordNArguments[] = { 1, 1, 0, 1, 1, 3, 6, 6, 1, 1, 3, 2, 1, 1, 4, 3, 3, 1, 1, 3, 1, 1, 4, 1 };

// Convert text string to AxisKeyword
UChromaBase::AxisKeyword UChromaBase::axisKeyword(const char* s)
{
	for (int n=0; n<nAxisKeywords; ++n) if (DUQSys::sameString(s, UChromaAxisBlockKeywords[n])) return (UChromaBase::AxisKeyword) n;
	return nAxisKeywords;
}

// Convert UChromaAxisBlockKeywords to text string
const char* UChromaBase::axisKeyword(UChromaBase::AxisKeyword kwd)
{
	return UChromaAxisBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int UChromaBase::axisKeywordNArguments(UChromaBase::AxisKeyword kwd)
{
	return UChromaAxisKeywordNArguments[kwd];
}

/*
 * Collection Keywords
 */

// Collection Block Keywords
const char* UChromaCollectionBlockKeywords[] = { "ColourAlphaControl", "ColourAlphaFixed", "ColourCustomGradient", "ColourLinearRGBA", "ColourLinearRGBB", "ColourLinearHSVA", "ColourLinearHSVB", "ColourSingle", "ColourSource", "DataDirectory", "DataSet", "EndCollection", "Fit", "FitParameters", "Interpolate", "InterpolateConstrain", "InterpolateStep", "LineStyle", "Shininess", "Slice", "Style", "TransformX", "TransformY", "TransformZ", "Visible" };

// Collection Block NArguments
int UChromaCollectionKeywordNArguments[] = { 1, 1, 5, 5, 5, 5, 5, 4, 1, 1, 1, 0, 1, 0, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1 };

// Convert text string to CollectionKeyword
UChromaBase::CollectionKeyword UChromaBase::collectionKeyword(const char* s)
{
	for (int n=0; n<nCollectionKeywords; ++n) if (DUQSys::sameString(s, UChromaCollectionBlockKeywords[n])) return (UChromaBase::CollectionKeyword) n;
	return nCollectionKeywords;
}

// Convert CollectionKeyword to text string
const char* UChromaBase::collectionKeyword(UChromaBase::CollectionKeyword kwd)
{
	return UChromaCollectionBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int UChromaBase::collectionKeywordNArguments(UChromaBase::CollectionKeyword kwd)
{
	return UChromaCollectionKeywordNArguments[kwd];
}

/*
 * FitParameters Keywords
 */

// FitParameters Block Keywords
const char* UChromaFitParametersBlockKeywords[] = { "EndFitParameters", "Equation", "FitResults", "Global", "LimitStrength", "Orthogonal", "Reference", "Variable", "XRangeType", "XRangeAbsolute", "XRangeIndex", "XRangeSingle", "ZRangeType", "ZRangeAbsolute", "ZRangeIndex", "ZRangeSingle" };

// FitParameters Block NArguments
int UChromaFitParametersKeywordNArguments[] = { 0, 1, 1, 1, 1, 1, 7, 7, 1, 2, 2, 1, 1, 2, 2, 1 };

// Convert text string to FitParametersKeyword
UChromaBase::FitParametersKeyword UChromaBase::fitParametersKeyword(const char* s)
{
	for (int n=0; n<nFitParametersKeywords; ++n) if (DUQSys::sameString(s, UChromaFitParametersBlockKeywords[n])) return (UChromaBase::FitParametersKeyword) n;
	return nFitParametersKeywords;
}

// Convert FitParametersKeyword to text string
const char* UChromaBase::fitParametersKeyword(UChromaBase::FitParametersKeyword kwd)
{
	return UChromaFitParametersBlockKeywords[kwd];
}

 // Return minimum number of expected arguments
int UChromaBase::fitParametersKeywordNArguments(UChromaBase::FitParametersKeyword kwd)
{
	return UChromaFitParametersKeywordNArguments[kwd];
}

/*
 * FitResults Keywords
 */

// FitResults Block Keywords
const char* UChromaFitResultsBlockKeywords[] = { "EndFitResults", "Variable" };

// FitResults Block NArguments
int UChromaFitResultsKeywordNArguments[] = { 0, 1 };

// Convert text string to FitResultsKeyword
UChromaBase::FitResultsKeyword UChromaBase::fitResultsKeyword(const char* s)
{
	for (int n=0; n<nFitResultsKeywords; ++n) if (DUQSys::sameString(s, UChromaFitResultsBlockKeywords[n])) return (UChromaBase::FitResultsKeyword) n;
	return nFitResultsKeywords;
}

// Convert FitResultsKeyword to text string
const char* UChromaBase::fitResultsKeyword(UChromaBase::FitResultsKeyword kwd)
{
	return UChromaFitResultsBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int UChromaBase::fitResultsKeywordNArguments(UChromaBase::FitResultsKeyword kwd)
{
	return UChromaFitResultsKeywordNArguments[kwd];
}

/*
 * Settings Keywords
 */

// Settings Block Keywords
const char* UChromaSettingsBlockKeywords[] = { "EndSettings", "ImageExport" };

// Settings Block NArguments
int UChromaSettingsKeywordNArguments[] = { 0, 5 };

// Convert text string to SettingsKeyword
UChromaBase::SettingsKeyword UChromaBase::settingsKeyword(const char* s)
{
	for (int n=0; n<nSettingsKeywords; ++n) if (DUQSys::sameString(s, UChromaSettingsBlockKeywords[n])) return (UChromaBase::SettingsKeyword) n;
	return nSettingsKeywords;
}

// Convert SettingsKeyword to text string
const char* UChromaBase::settingsKeyword(UChromaBase::SettingsKeyword kwd)
{
	return UChromaSettingsBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int UChromaBase::settingsKeywordNArguments(UChromaBase::SettingsKeyword kwd)
{
	return UChromaSettingsKeywordNArguments[kwd];
}

/*
 * DataSet Keywords
 */

// DataSet Block Keywords
const char* UChromaDataSetBlockKeywords[] = { "Data", "EndDataSet", "Source", "Z" };

// DataSet Block NArguments
int UChromaDataSetKeywordNArguments[] = { 0, 0, 1, 1 };

// Convert text string to DataSetKeyword
UChromaBase::DataSetKeyword UChromaBase::dataSetKeyword(const char* s)
{
	for (int n=0; n<nDataSetKeywords; ++n) if (DUQSys::sameString(s, UChromaDataSetBlockKeywords[n])) return (UChromaBase::DataSetKeyword) n;
	return nDataSetKeywords;
}

// Convert DataSetKeyword to text string
const char* UChromaBase::dataSetKeyword(UChromaBase::DataSetKeyword kwd)
{
	return UChromaDataSetBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int UChromaBase::dataSetKeywordNArguments(UChromaBase::DataSetKeyword kwd)
{
	return UChromaDataSetKeywordNArguments[kwd];
}

/*
 * View Keywords
 */

// View Block Keywords
const char* UChromaViewBlockKeywords[] = { "EndView", "Grid", "ViewPane" };

// View Block NArguments
int UChromaViewKeywordNArguments[] = { 0, 2, 1 };

// Convert text string to ViewKeyword
UChromaBase::ViewKeyword UChromaBase::viewKeyword(const char* s)
{
	for (int n=0; n<nViewKeywords; ++n) if (DUQSys::sameString(s, UChromaViewBlockKeywords[n])) return (UChromaBase::ViewKeyword) n;
	return nViewKeywords;
}

// Convert ViewKeyword to text string
const char* UChromaBase::viewKeyword(UChromaBase::ViewKeyword kwd)
{
	return UChromaViewBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int UChromaBase::viewKeywordNArguments(UChromaBase::ViewKeyword kwd)
{
	return UChromaViewKeywordNArguments[kwd];
}

/*
 * ViewPane Keywords
 */

// ViewPane Block Keywords
const char* UChromaViewPaneBlockKeywords[] = { "AutoPositionTitles", "Axis", "BoundingBox", "BoundingBoxPlaneY", "EndViewPane", "FlatLabels", "Geometry", "LabelPointSize", "Perspective", "Role", "RoleTargetCollection", "RoleTargetPane", "RotationX", "RotationY", "RotationZ", "TitlePointSize", "Translation", "UseBestFlatView", "ViewType" };

// ViewPane Block NArguments
int UChromaViewPaneKeywordNArguments[] = { 1, 1, 1, 1, 0, 1, 4, 1, 1, 1, 1, 1, 3, 3, 3, 1, 3, 1, 1 };

// Convert text string to ViewPaneKeyword
UChromaBase::ViewPaneKeyword UChromaBase::viewPaneKeyword(const char* s)
{
	for (int n=0; n<nViewPaneKeywords; ++n) if (DUQSys::sameString(s, UChromaViewPaneBlockKeywords[n])) return (UChromaBase::ViewPaneKeyword) n;
	return nViewPaneKeywords;
}

// Convert ViewPaneKeyword to text string
const char* UChromaBase::viewPaneKeyword(UChromaBase::ViewPaneKeyword kwd)
{
	return UChromaViewPaneBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int UChromaBase::viewPaneKeywordNArguments(UChromaBase::ViewPaneKeyword kwd)
{
	return UChromaViewPaneKeywordNArguments[kwd];
}
