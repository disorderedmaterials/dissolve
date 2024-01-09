// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "templates/vector3.h"
#include <FTGL/ftgl.h>
#include <QColor>
#include <QResource>
#include <QString>

// Font Instance
class FontInstance
{
    public:
    FontInstance(QColor colour);

    /*
     * Font Data
     */
    private:
    // Text colour
    QColor colour_;
    // Font file last passed to setupFont()
    QString fontFile_;
    // Font data
    QResource *fontData_;
    // FTGL font for text
    FTFont *font_;
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
    bool setUp(QString fontFileName = "");
    // Return whether font exists and is ready for use
    bool fontOK() const;
    // Return full height of font
    double fontFullHeight() const;
    // Return base height of font
    double fontBaseHeight() const;
    // Set general scaling factor for primitives rendered with this font instance
    void setScaleFactor(double scaleFactor);
    // Return general scaling factor for primitives rendered with this font instance
    double scaleFactor() const;
    // Return the text colour
    QColor colour() const;

    /*
     * Rendering
     */
    public:
    // Set face size
    bool setFaceSize(double faceSize);
    // Render supplied text
    bool renderText(std::string text) const;

    /*
     * Bounding Box Calculation
     */
    private:
    // Return bounding box for specified string
    FTBBox boundingBox(std::string_view text) const;

    public:
    // Calculate bounding box for specified string
    void boundingBox(std::string_view text, Vec3<double> &lowerLeft, Vec3<double> &upperRight) const;
    // Calculate bounding box width for specified string
    double boundingBoxWidth(std::string_view text) const;
    // Calculate bounding box height for specified string
    double boundingBoxHeight(std::string_view text) const;
};
