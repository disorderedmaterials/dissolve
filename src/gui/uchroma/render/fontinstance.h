/*
	*** Font (from FTGL)
	*** src/gui/uchroma/render/fontinstance.h
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

#ifndef DUQ_UCHROMAFONTINSTANCE_H
#define DUQ_UCHROMAFONTINSTANCE_H

#include "templates/vector3.h"
#include <FTGL/ftgl.h>
#include <QString>
#include <QResource>

// Forward Declarations
/* none */

// Static Font Instance
class FontInstance
{
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

	public:
	// Setup font specified
	bool setup(QString fontFileName);
	// Return whether font exists and is ready for use
	bool fontOK();
	// Return current font
	FTFont* font();
	// Return full height of font
	double fontFullHeight();
	// Return base height of font
	double fontBaseHeight();
	// Return bounding box for specified string
	FTBBox boundingBox(QString text);
	// Calculate bounding box for specified string
	void boundingBox(QString text, Vec3<double>& lowerLeft, Vec3<double>& upperRight);
	// Calculate bounding box width for specified string
	double boundingBoxWidth(QString text);
	// Calculate bounding box height for specified string
	double boundingBoxHeight(QString text);
};

#endif
