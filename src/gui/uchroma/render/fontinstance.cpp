/*
	*** Font Instance
	*** src/gui/uchroma/render/fontinstance.cpp
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

#include "gui/uchroma/render/fontinstance.h"
#include "base/messenger.h"

// Constructor
FontInstance::FontInstance()
{
	fontData_ = NULL;
	font_ = NULL;
	fontBaseHeight_ = 0.0;
	fontFullHeight_ = 0.0;
	dotWidth_ = 0.0;
}

// Setup font specified
bool FontInstance::setup(QString fontFileName)
{
	// Delete any previous font
	if (font_) delete font_;
	font_ = NULL;
	if (fontData_) delete fontData_;
	fontData_ = NULL;

	// Check the fontFileName - if it's empty then we try to load the default font from our resource
	fontFile_ = fontFileName;
	if (fontFile_.isEmpty()) fontData_ = new QResource(":/freefont/freefont/FreeSans.ttf");
	else fontData_ = new QResource(fontFileName);
	if (fontData_->size() <= 0)
	{
		Messenger::print("Font data is empty - correct resource path specified?\n");
		return false;
	}

	// Construct font
	FTPolygonFont* newFont = new FTPolygonFont(fontData_->data(), fontData_->size());
	if (newFont->Error())
	{
		printf("Error generating font.\n");
		delete newFont;
		fontBaseHeight_ = 1.0;
		return false;
	}
	else
	{
		font_ = newFont;

		// Request unicode character mapping...
		if (!font_->CharMap(ft_encoding_unicode)) Messenger::print("Failed to set unicode character mapping for font - special characters may not render correctly.\n");

// 		font_->Depth(3.0);
// 		font_->Outset(-.5, 1.5);
		font_->FaceSize(1);
		FTBBox boundingBox = font_->BBox("0123456789");
		fontBaseHeight_ = boundingBox.Upper().Y() - boundingBox.Lower().Y();
		boundingBox = font_->BBox("ABCDEfghijKLMNOpqrstUVWXYz");
		fontFullHeight_ = boundingBox.Upper().Y() - boundingBox.Lower().Y();
		boundingBox = font_->BBox("..");
		dotWidth_ = boundingBox.Upper().X() - boundingBox.Lower().X();
	}

	return (font_ != NULL);
}

// Return whether font exists and is ready for use
bool FontInstance::fontOK()
{
	return (font_ != NULL);
}

// Return current font
FTFont* FontInstance::font()
{
	return font_;
}

// Return base height of font
double FontInstance::fontBaseHeight()
{
	return fontBaseHeight_;
}

// Return full height of font
double FontInstance::fontFullHeight()
{
	return fontFullHeight_;
}

// Return bounding box for specified string
FTBBox FontInstance::boundingBox(QString text)
{
	if (!font_) return FTBBox();

	// Need to be a little careful here - we will put a '.' either side of the text so we get the full width of strings with trailing spaces..
	FTBBox box = font_->BBox(qPrintable("." + text.toUtf8() + "."));
// 	double newWidth = box.Upper().X() - dotWidth_;
// 	box.Upper().X(newWidth);
	return FTBBox(box.Lower(), FTPoint(box.Upper().X()-dotWidth_, box.Upper().Y()));
}

// Calculate bounding box for specified string
void FontInstance::boundingBox(QString text, Vec3<double>& lowerLeft, Vec3<double>& upperRight)
{
	FTBBox box = boundingBox(text);
	lowerLeft.set(box.Lower().X(), box.Lower().Y(), box.Lower().Z());
	upperRight.set(box.Upper().X(), box.Upper().Y(), box.Upper().Z());
}

// Calculate bounding box width for specified string
double FontInstance::boundingBoxWidth(QString text)
{
	FTBBox box = boundingBox(text);
	return (box.Upper().X() - box.Lower().X());
}

// Calculate bounding box height for specified string
double FontInstance::boundingBoxHeight(QString text)
{
	FTBBox box = boundingBox(text);
	return (box.Upper().Y() - box.Lower().Y());
}
