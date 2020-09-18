/*
    *** Primitive Info
    *** src/gui/render/primitiveinfo.h
    Copyright T. Youngs 2019-2020

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

#pragma once

#include "gui/render/linestyle.h"
#include "math/matrix4.h"

// Forward Declarations
class Primitive;

// Primitive Info
class PrimitiveInfo
{
    public:
    // Constructor / Desctructor
    PrimitiveInfo();
    virtual ~PrimitiveInfo();

    /*
     * GL
     */
    public:
    // Expose contained info to GL
    virtual void sendToGL(double pixelScaling) = 0;
};

/*
 * Derived Classes
 */

// Primitive Only
class UncolouredPrimitiveInfo : public PrimitiveInfo
{
    public:
    UncolouredPrimitiveInfo(Primitive *prim = nullptr, Matrix4 transform = Matrix4());
    ~UncolouredPrimitiveInfo();

    private:
    // Target primitive
    Primitive *primitive_;
    // Transformation to apply before rendering Primitive
    Matrix4 transform_;

    public:
    // Expose contained info to GL
    void sendToGL(double pixelScaling);
};

// Primitive and Colour
class ColouredPrimitiveInfo : public PrimitiveInfo
{
    public:
    ColouredPrimitiveInfo(Primitive *prim = nullptr, Matrix4 transform = Matrix4(), GLfloat r = 0.0, GLfloat g = 0.0,
                          GLfloat b = 0.0, GLfloat a = 0.0);
    ~ColouredPrimitiveInfo();

    private:
    // Target primitive
    Primitive *primitive_;
    // Transformation to apply before rendering Primitive
    Matrix4 transform_;
    // Colour of Primitive
    GLfloat colour_[4];

    public:
    // Expose contained info to GL
    void sendToGL(double pixelScaling);
};

// Style Information
class StylePrimitiveInfo : public PrimitiveInfo
{
    public:
    StylePrimitiveInfo(bool lighting = true, GLenum polygonFillMode = GL_FILL);
    ~StylePrimitiveInfo();

    private:
    // Whether lighting is enabled
    bool lighting_;
    // Polygon fill mode
    GLenum fillMode_;

    public:
    // Expose contained info to GL
    void sendToGL(double pixelScaling);
};

// Line Style Information
class LineStylePrimitiveInfo : public PrimitiveInfo
{
    public:
    LineStylePrimitiveInfo(LineStyle style = LineStyle());
    ~LineStylePrimitiveInfo();

    private:
    // Line styling to apply
    LineStyle lineStyle_;

    public:
    // Expose contained info to GL
    void sendToGL(double pixelScaling);
};
