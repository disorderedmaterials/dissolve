/*
	*** Colour Definition
	*** src/gui/uchroma/classes/colourdef.cpp
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

#include "gui/uchroma/classes/colourdef.h"
#include "base/sysfunc.h"

// Constructor
ColourDefinition::ColourDefinition()
{
	colourScale_.clear();
	colourSinglePoint_.set(0.0, QColor(0,0,0));
	colourRGBGradientAPoint_.set(0.0, QColor(255,255,255));
	colourRGBGradientBPoint_.set(1.0, QColor(0,0,255));
	colourHSVGradientAPoint_.set(0.0, QColor(255,0,0));
	colourHSVGradientBPoint_.set(1.0, QColor(100,40,255));
	customColourScale_.clear();
	colourSource_ = SingleColourSource;
	alphaControl_ = ColourDefinition::OwnAlpha;
	fixedAlpha_ = 0.5;
	colourVersion_ = 0;
	colourScaleGeneratedAt_ = -1;
}

// Destructor
ColourDefinition::~ColourDefinition()
{
}

// Copy constructor
ColourDefinition::ColourDefinition(const ColourDefinition& source)
{
	(*this) = source;
}

// Assignment operator
void ColourDefinition::operator=(const ColourDefinition& source)
{
	colourSource_ = source.colourSource_;
	colourSinglePoint_ = source.colourSinglePoint_;
	colourRGBGradientAPoint_ = source.colourRGBGradientAPoint_;
	colourRGBGradientBPoint_ = source.colourRGBGradientBPoint_;
	colourHSVGradientAPoint_ = source.colourHSVGradientAPoint_;
	colourHSVGradientBPoint_ = source.colourHSVGradientBPoint_;
	colourScale_ = source.colourScale_;
	customColourScale_ = source.customColourScale_;
	alphaControl_ = source.alphaControl_;
	fixedAlpha_ = source.fixedAlpha_;
	colourVersion_ = source.colourVersion_;
	colourScaleGeneratedAt_ = source.colourScaleGeneratedAt_;
}

/*
 * Enumerations
 */

// ColourSource Keywords
const char* ColourSourceKeywords[] = { "SingleColour", "RGBGradient", "HSVGradient", "CustomGradient" };

// Convert text string to ColourSource
ColourDefinition::ColourSource ColourDefinition::colourSource(const char* s)
{
	for (int n=0; n<ColourDefinition::nColourSources; ++n) if (DissolveSys::sameString(s, ColourSourceKeywords[n])) return (ColourDefinition::ColourSource) n;
	return ColourDefinition::nColourSources;
}

// Convert ColourSource to text string
const char* ColourDefinition::colourSource(ColourSource cs)
{
	return ColourSourceKeywords[cs];
}

// AlphaControl keywords
const char* AlphaControlKeywords[] = { "OwnAlpha", "FixedAlpha" };

// Convert text string to AlphaControl
ColourDefinition::AlphaControl ColourDefinition::alphaControl(const char* s)
{
	for (int n=0; n<ColourDefinition::nAlphaControls; ++n) if (DissolveSys::sameString(s, AlphaControlKeywords[n])) return (ColourDefinition::AlphaControl) n;
	return ColourDefinition::nAlphaControls;
}

// Convert AlphaControl to text string
const char* ColourDefinition::alphaControl(ColourDefinition::AlphaControl as)
{
	return AlphaControlKeywords[as];
}

// Stock Colours
QColor StockColours[] = {
	QColor(0, 0, 0),	/* BlackStockColour */
	QColor(200, 0, 0),	/* RedStockColour */
	QColor(0, 200, 0),	/* GreenStockColour */
	QColor(0, 0, 200),	/* BlueStockColour */
	QColor(200, 0, 200),	/* PurpleStockColour */
	QColor(0, 200, 200),	/* CyanStockColour */
	QColor(255, 128, 0),	/* OrangeStockColour */
	QColor(255, 100, 255),	/* PinkStockColour */
	QColor(255, 100, 100),	/* LightRedStockColour */
	QColor(100, 255, 100),	/* LightGreenStockColour */
	QColor(100, 100, 255),	/* LightBlueStockColour */
	QColor(128, 128, 128),	/* SilverStockColour */
	QColor(128, 128, 50)	/* GoldStockColour */
};

// Return stock colour (as QColor)
QColor ColourDefinition::stockColour(ColourDefinition::StockColour colour)
{
	return StockColours[colour];
}

/*
 * Colour Definition
 */

// Update colour scale
void ColourDefinition::updateColourScale()
{
	if (colourVersion_ == colourScaleGeneratedAt_) return;

	colourScale_.clear();
	if (colourSource_ == ColourDefinition::SingleColourSource) colourScale_.addPoint(0.0, colourSinglePoint_.colour());
	else if (colourSource_ == ColourDefinition::RGBGradientSource)
	{
		colourScale_.setUseHSV(false);
		colourScale_.addPoint(colourRGBGradientAPoint_.value(), colourRGBGradientAPoint_.colour());
		colourScale_.addPoint(colourRGBGradientBPoint_.value(), colourRGBGradientBPoint_.colour());
	}
	else if (colourSource_ == ColourDefinition::HSVGradientSource)
	{
		colourScale_.setUseHSV(true);
		colourScale_.addPoint(colourHSVGradientAPoint_.value(), colourHSVGradientAPoint_.colour());
		colourScale_.addPoint(colourHSVGradientBPoint_.value(), colourHSVGradientBPoint_.colour());
	}
	else if (colourSource_ == ColourDefinition::CustomGradientSource) colourScale_ = customColourScale_;

	// Set alpha value for all points if alphaControl_ == FixedAlpha
	if (alphaControl_ == ColourDefinition::FixedAlpha) colourScale_.setAllAlpha(fixedAlpha_);

	colourScaleGeneratedAt_ = colourVersion_;
}

// Set colourscale source to use
void ColourDefinition::setColourSource(ColourSource source)
{
	colourSource_ = source;

	++colourVersion_;
}

// Return colourscale source to use
ColourDefinition::ColourSource ColourDefinition::colourSource() const
{
	return colourSource_;
}

// Set colourscale point colour
void ColourDefinition::setColourScalePoint(ColourSource source, QColor colour, double value, int index)
{
	switch (source)
	{
		case (ColourDefinition::SingleColourSource):
			colourSinglePoint_.setColour(colour);
			break;
		case (ColourDefinition::RGBGradientSource):
			if (index == 0) colourRGBGradientAPoint_.set(value, colour);
			else colourRGBGradientBPoint_.set(value, colour);
			break;
		case (ColourDefinition::HSVGradientSource):
			if (index == 0) colourHSVGradientAPoint_.set(value, colour);
			else colourHSVGradientBPoint_.set(value, colour);
			break;
		case (ColourDefinition::CustomGradientSource):
			customColourScale_.setPoint(index, value, colour);
			break;
		default:
			Messenger::print("Unhandled ColourSource in ColourDefinition::setColourScalePoint().\n");
	}

	// Update colourscale?
	if (source == colourSource_) ++colourVersion_;
}

// Return colourscale point specified
const ColourScalePoint* ColourDefinition::colourScalePoint(ColourSource source, int index)
{
	switch (source)
	{
		case (ColourDefinition::SingleColourSource):
			return &colourSinglePoint_;
			break;
		case (ColourDefinition::RGBGradientSource):
			return (index == 0 ? &colourRGBGradientAPoint_ : &colourRGBGradientBPoint_);
			break;
		case (ColourDefinition::HSVGradientSource):
			return (index == 0 ? &colourHSVGradientAPoint_ : &colourHSVGradientBPoint_);
			break;
		case (ColourDefinition::CustomGradientSource):
			return customColourScale_.point(index);
			break;
		default:
			Messenger::print("Unhandled ColourSource in ColourDefinition::colourScalePoint().\n");
	}

	return NULL;
}

// Return colour of point specified
QColor ColourDefinition::colourScalePointColour(ColourSource source, int index)
{
	const ColourScalePoint* point = colourScalePoint(source, index);
	if (point) return point->colour();
	else return Qt::black;
}

// Return value of point specified
double ColourDefinition::colourScalePointValue(ColourSource source, int index)
{
	const ColourScalePoint* point = colourScalePoint(source, index);
	if (point) return point->value();
	else return 0.0;
}

// Add point to custom colour scale
void ColourDefinition::addCustomColourScalePoint()
{
	customColourScale_.addPoint(customColourScale_.lastPoint() ? customColourScale_.lastPoint()->value() + 1.0 : 0.0, Qt::white);

	// Update colourscale?
	if (colourSource_ == ColourDefinition::CustomGradientSource) ++colourVersion_;
}

// Add point to custom colourscale
void ColourDefinition::addCustomColourScalePoint(double value, QColor colour)
{
	customColourScale_.addPoint(value, colour);
}

// Return number of custom colourscale points
int ColourDefinition::nCustomColourScalePoints()
{
	return customColourScale_.nPoints();
}

// Return first custom colourscale point in list
ColourScalePoint* ColourDefinition::customColourScalePoints()
{
	return customColourScale_.firstPoint();
}

// Return custom colourscale point with index specified
ColourScalePoint* ColourDefinition::customColourScalePoint(int id)
{
	return customColourScale_.point(id);
}

// Remove specified colourscale point
void ColourDefinition::removeCustomColourScalePoint(ColourScalePoint* point)
{
	customColourScale_.removePoint(point);

	// Update colourscale?
	if (colourSource_ == ColourDefinition::CustomGradientSource) ++colourVersion_;
}

// Set alpha control
void ColourDefinition::setAlphaControl(ColourDefinition::AlphaControl alpha)
{
	alphaControl_ = alpha;

	++colourVersion_;
}

// Return current alpha control
ColourDefinition::AlphaControl ColourDefinition::alphaControl() const
{
	return alphaControl_;
}

// Set fixed alpha value
void ColourDefinition::setFixedAlpha(double alpha)
{
	fixedAlpha_ = alpha;

	++colourVersion_;
}

// Return fixed alpha value
double ColourDefinition::fixedAlpha() const
{
	return fixedAlpha_;
}

// Return current colourscale
const ColourScale& ColourDefinition::colourScale()
{
	// Does the colourscale need updating first?
	updateColourScale();
	
	return colourScale_;
}

// Return colour version
int ColourDefinition::colourVersion() const
{
	return colourVersion_;
}

