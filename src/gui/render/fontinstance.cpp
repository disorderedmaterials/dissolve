/*
    *** Font Instance
    *** src/gui/render/fontinstance.cpp
    Copyright T. Youngs 2013-2020

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

#include "gui/render/fontinstance.h"
#include "base/messenger.h"

FontInstance::FontInstance()
{
    fontData_ = nullptr;
    font_ = nullptr;
    fontBaseHeight_ = 0.0;
    fontFullHeight_ = 0.0;
    dotWidth_ = 0.0;
    scaleFactor_ = 1.0;
}

/*
 * Font Data
 */

// Set up font with font filename specified
bool FontInstance::setUp(QString fontFileName)
{
    // Delete any previous font
    if (font_)
        delete font_;
    font_ = nullptr;
    if (fontData_)
        delete fontData_;
    fontData_ = nullptr;

    // Check the fontFileName - if it's empty then we try to load the default font from our resource
    fontFile_ = fontFileName;
    if (fontFile_.isEmpty())
        fontData_ = new QResource(":/fonts/fonts/FreeSans.ttf");
    else
        fontData_ = new QResource(fontFile_);
    if (fontData_->size() <= 0)
    {
        Messenger::error("Font data is empty - correct resource path specified?\n");
        return false;
    }

    // Construct font
    auto *newFont = new FTPolygonFont(fontData_->data(), fontData_->size());
    if (newFont->Error())
    {
        Messenger::error("Error generating font.\n");
        delete newFont;
        fontBaseHeight_ = 1.0;
        return false;
    }
    else
    {
        font_ = newFont;

        // Request unicode character mapping...
        if (!font_->CharMap(ft_encoding_unicode))
            Messenger::print("Failed to set unicode character mapping for font - special characters may not render "
                             "correctly.\n");

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

    return (font_ != nullptr);
}

// Return whether font exists and is ready for use
bool FontInstance::fontOK() const { return (font_ != nullptr); }

// Return base height of font
double FontInstance::fontBaseHeight() const { return fontBaseHeight_; }

// Return full height of font
double FontInstance::fontFullHeight() const { return fontFullHeight_; }

// Return bounding box for specified string
FTBBox FontInstance::boundingBox(std::string_view text) const
{
    if (!font_)
        return FTBBox();

    // Need to be a little careful here - we will put a '.' either side of the text so we get the full width of strings with
    // trailing spaces..
    FTBBox box = font_->BBox(fmt::format(".{}.", text).c_str());
    // 	double newWidth = box.Upper().X() - dotWidth_;
    // 	box.Upper().X(newWidth);
    return FTBBox(box.Lower(), FTPoint(box.Upper().X() - dotWidth_, box.Upper().Y()));
}

// Set general scaling factor for primitives rendered with this font instance
void FontInstance::setScaleFactor(double scaleFactor) { scaleFactor_ = scaleFactor; }

// Return general scaling factor for primitives rendered with this font instance
double FontInstance::scaleFactor() const { return scaleFactor_; }

/*
 * Rendering
 */

// Set face size
bool FontInstance::setFaceSize(double faceSize)
{
    if (!font_)
        return false;

    font_->FaceSize(faceSize);

    return true;
}

// Render supplied text
bool FontInstance::renderText(std::string text) const
{
    if (!font_)
        return false;

    font_->Render(text.c_str());

    return true;
}

/*
 * Bounding Box Calculation
 */

// Calculate bounding box for specified string
void FontInstance::boundingBox(std::string_view text, Vec3<double> &lowerLeft, Vec3<double> &upperRight) const
{
    FTBBox box = boundingBox(text);
    lowerLeft.set(box.Lower().X(), box.Lower().Y(), box.Lower().Z());
    upperRight.set(box.Upper().X(), box.Upper().Y(), box.Upper().Z());
    lowerLeft *= scaleFactor_;
    upperRight *= scaleFactor_;
}

// Calculate bounding box width for specified string
double FontInstance::boundingBoxWidth(std::string_view text) const
{
    FTBBox box = boundingBox(text);
    return scaleFactor_ * (box.Upper().X() - box.Lower().X());
}

// Calculate bounding box height for specified string
double FontInstance::boundingBoxHeight(std::string_view text) const
{
    FTBBox box = boundingBox(text);
    return scaleFactor_ * (box.Upper().Y() - box.Lower().Y());
}
