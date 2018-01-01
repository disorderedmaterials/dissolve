/*
	*** UChroma Base - Image
	*** src/gui/uchroma/uchromabase_image.cpp
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
#include "base/sysfunc.h"

// Available image export formats
const char* imageFormatFilters[UChromaBase::nImageFormats] = { "Windows Image (*.bmp)", "Joint Photographic Experts Group (*.jpg)", "Portable Network Graphics (*.png)", "Portable Pixmap (*.ppm)", "X11 Bitmap (*.xbm)", "X11 Pixmap (*.xpm)" };
const char* imageFormatExtensions[UChromaBase::nImageFormats] = { "bmp", "jpg", "png", "ppm", "xbm", "xpm" };
UChromaBase::ImageFormat UChromaBase::imageFormat(const char* s)
{
	for (int n=0; n<UChromaBase::nImageFormats; ++n) if (DUQSys::sameString(s, imageFormatExtensions[n])) return (UChromaBase::ImageFormat) n;
	return UChromaBase::nImageFormats;
}
UChromaBase::ImageFormat UChromaBase::imageFormatFromFilter(const char* s)
{
	for (int n=0; n<UChromaBase::nImageFormats; ++n) if (DUQSys::sameString(s, imageFormatFilters[n])) return (UChromaBase::ImageFormat) n;
	return UChromaBase::nImageFormats;
}
const char* UChromaBase::imageFormatFilter(UChromaBase::ImageFormat bf)
{
        return imageFormatFilters[bf];
}
const char* UChromaBase::imageFormatExtension(UChromaBase::ImageFormat bf)
{
        return imageFormatExtensions[bf];
}

// Set current image export filename
void UChromaBase::setImageExportFileName(const char* fileName)
{
	imageExportFileName_ = fileName;
}

// Return current image export filename
const char* UChromaBase::imageExportFileName()
{
	return imageExportFileName_;
}

// Set format for exported image
void UChromaBase::setImageExportFormat(UChromaBase::ImageFormat fmt)
{
	imageExportFormat_ = fmt;
}

// Return format for exported image
UChromaBase::ImageFormat UChromaBase::imageExportFormat()
{
	return imageExportFormat_;
}

// Set width of exported image
void UChromaBase::setImageExportWidth(int width)
{
	imageExportWidth_ = width;
}

// Return width of exported image
int UChromaBase::imageExportWidth()
{
	return imageExportWidth_;
}

// Set height of exported image
void UChromaBase::setImageExportHeight(int height)
{
	imageExportHeight_ = height;
}

// Return height of exported image
int UChromaBase::imageExportHeight()
{
	return imageExportHeight_;
}

// Set whether to maintain current aspect ratio on image export
void UChromaBase::setImageExportMaintainAspect(bool b)
{
	imageExportMaintainAspect_ = b;
}

// Return whether to maintain current aspect ratio on image export
bool UChromaBase::imageExportMaintainAspect()
{
	return imageExportMaintainAspect_;
}

// Set font filename to use for viewer
void UChromaBase::setViewerFontFileName(const char* fileName)
{
	viewerFontFileName_ = fileName;
}

// Return font file to use for viewer
const char* UChromaBase::viewerFontFileName()
{
	return viewerFontFileName_;
}
