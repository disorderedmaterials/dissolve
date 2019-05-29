/*
	*** Font Instance
	*** src/gui/viewer/render/fontinstance.h
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

#ifndef DISSOLVE_FONTINSTANCE_H
#define DISSOLVE_FONTINSTANCE_H

#include "templates/vector3.h"
#include <FTGL/ftgl.h>
#include <QString>
#include <QResource>

// Forward Declarations
/* none */

// Font Instance
class FontInstance
{
	public:
	// Constructor
	FontInstance();


	/*
	 * Font Data
	 */
	private:
	// Font file last passed to setupFont()
	QString fontFile_;
	// Font data
	QResource* fontData_;
	// FTGL font for text
	FTFont* font_;
	// Font full height (from bottom of descender to top of ascender)
	double fontFullHeight_;
	// Font base height (from baseline to top of ascender)
	double fontBaseHeight_;
	// Width of double dot (used for correction of width of strings with trailing spaces)
	double dotWidth_;
	// General scaling factor for primitives rendered with this font instance
	double scaleFactor_;

	public:
	// Set up font with font filename specified
	bool setUp(QString fontFileName = QString());
	// Return whether font exists and is ready for use
	bool fontOK() const;
	// Return current font
	FTFont* font() const;
	// Return full height of font
	double fontFullHeight() const;
	// Return base height of font
	double fontBaseHeight() const;
	// Set general scaling factor for primitives rendered with this font instance
	void setScaleFactor(double scaleFactor);
	// Return general scaling factor for primitives rendered with this font instance
	double scaleFactor() const;


	/*
	 * Bounding Box Calculation
	 */
	private:
	// Return bounding box for specified string
	FTBBox boundingBox(QString text) const;

	public:
	// Calculate bounding box for specified string
	void boundingBox(QString text, Vec3<double>& lowerLeft, Vec3<double>& upperRight) const;
	// Calculate bounding box width for specified string
	double boundingBoxWidth(QString text) const;
	// Calculate bounding box height for specified string
	double boundingBoxHeight(QString text) const;
};

#endif
