/*
	*** Colour Definition
	*** src/gui/uchroma/classes/colourdef.h
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

#ifndef DUQ_UCHROMACOLOURDEF_H
#define DUQ_UCHROMACOLOURDEF_H

#include "gui/uchroma/classes/colourscale.h"

// Forward Declarations
/* none */

// Colour Definition
class ColourDefinition
{
	public:
	// Constructor
	ColourDefinition();
	// Destructor
	~ColourDefinition();
	// Copy constructor
	ColourDefinition(const ColourDefinition& source);
	// Assignment operator
	void operator=(const ColourDefinition& source);


	/*
	 * Enumerations
	 */
	public:
	// Available colourscale sources
	enum ColourSource { SingleColourSource, RGBGradientSource, HSVGradientSource, CustomGradientSource, nColourSources };
	// Convert text string to ColourSource
	static ColourSource colourSource(const char* s);
	// Convert ColourSource to text string
	static const char* colourSource(ColourDefinition::ColourSource cs);
	// Alpha control options
	enum AlphaControl { OwnAlpha, FixedAlpha, nAlphaControls };
	// Convert text string to AlphaControl
	static AlphaControl alphaControl(const char* s);
	// Convert AlphaControl to text string
	static const char* alphaControl(ColourDefinition::AlphaControl as);
	// Stock colours enum
	enum StockColour { BlackStockColour, RedStockColour, GreenStockColour, BlueStockColour, PurpleStockColour, CyanStockColour, OrangeStockColour, PinkStockColour, LightRedStockColour, LightGreenStockColour, LightBlueStockColour, SilverStockColour, GoldStockColour, nStockColours };
	// Return stock colour (as QColor)
	static QColor stockColour(StockColour colour);


	/*
	 * Colour Definiton
	 */
	private:
	// Current colourscale source to use
	ColourSource colourSource_;
	// Points for SingleColour, RGBGradient and HSVGradient sources
	ColourScalePoint colourSinglePoint_;
	ColourScalePoint colourRGBGradientAPoint_, colourRGBGradientBPoint_;
	ColourScalePoint colourHSVGradientAPoint_, colourHSVGradientBPoint_;
	// Actual ColourScale used by surface
	ColourScale colourScale_;
	// Custom ColourScale source
	ColourScale customColourScale_;
	// Current alpha control
	AlphaControl alphaControl_;
	// Fixed alpha value (for FixedAlpha option)
	double fixedAlpha_;
	// Version for colourscale
	int colourVersion_;
	// Version at which the colourscale was last generated at
	int colourScaleGeneratedAt_;

	public:
	// Update colour scale
	void updateColourScale();
	// Set colourscale source to use
	void setColourSource(ColourSource source);
	// Return colourscale source to use
	ColourSource colourSource() const;
	// Set colourscale point colour
	void setColourScalePoint(ColourSource source, QColor colour, double value = 0.0, int index = -1);
	// Return colourscale point specified
	const ColourScalePoint* colourScalePoint(ColourSource source, int index = -1);
	// Return colour of point specified
	QColor colourScalePointColour(ColourSource source, int index = -1);
	// Retu5rn value of point specified
	double colourScalePointValue(ColourSource source, int index = -1);
	// Add empty point to end of custom colourscale
	void addCustomColourScalePoint();
	// Add point to custom colourscale
	void addCustomColourScalePoint(double value, QColor colour);
	// Return number of custom colourscale points
	int nCustomColourScalePoints();
	// Return first custom colourscale point in list
	ColourScalePoint* customColourScalePoints();
	// Return custom colourscale point with index specified
	ColourScalePoint* customColourScalePoint(int id);
	// Remove specified colourscale point
	void removeCustomColourScalePoint(ColourScalePoint* point);
	// Set alpha control
	void setAlphaControl(AlphaControl alpha);
	// Return current alpha control
	AlphaControl alphaControl() const;
	// Set fixed alpha value
	void setFixedAlpha(double alpha);
	// Return fixed alpha value
	double fixedAlpha() const;
	// Return current colourscale
	const ColourScale& colourScale();
	// Return colour version
	int colourVersion() const;
};

#endif
