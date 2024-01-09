// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/primitiveInfo.h"
#include "gui/render/primitive.h"

/*
 * PrimitiveInfo
 */

PrimitiveInfo::PrimitiveInfo() {}

PrimitiveInfo::~PrimitiveInfo() {}

/*
 * UncolouredPrimitiveInfo
 */

UncolouredPrimitiveInfo::UncolouredPrimitiveInfo(Primitive *prim, Matrix4 transform) : primitive_(prim), transform_(transform)
{
}

UncolouredPrimitiveInfo::~UncolouredPrimitiveInfo() {}

// Expose contained info to GL
void UncolouredPrimitiveInfo::sendToGL(double pixelScaling)
{
    // Render Primitive if one is present
    if (primitive_)
    {
        glPushMatrix();
        glMultMatrixd(transform_.matrix());
        primitive_->sendToGL();
        glPopMatrix();
    }
}

/*
 * ColouredPrimitiveInfo
 */

ColouredPrimitiveInfo::ColouredPrimitiveInfo(Primitive *prim, Matrix4 transform, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
    : primitive_(prim), transform_(transform)
{
    colour_[0] = r;
    colour_[1] = g;
    colour_[2] = b;
    colour_[3] = a;
}

ColouredPrimitiveInfo::~ColouredPrimitiveInfo() {}

// Expose contained info to GL
void ColouredPrimitiveInfo::sendToGL(double pixelScaling)
{
    // Apply colour
    glColor4fv(colour_);

    // Render Primitive if one is present
    if (primitive_)
    {
        glPushMatrix();
        glMultMatrixd(transform_.matrix());
        primitive_->sendToGL();
        glPopMatrix();
    }
}

/*
 * StylePrimitiveInfo
 */

StylePrimitiveInfo::StylePrimitiveInfo(bool lighting, GLenum polygonFillMode) : lighting_(lighting), fillMode_(polygonFillMode)
{
}

StylePrimitiveInfo::~StylePrimitiveInfo() {}

// Expose contained info to GL
void StylePrimitiveInfo::sendToGL(double pixelScaling)
{
    // Enable / disable lighting
    if (lighting_)
        glEnable(GL_LIGHTING);
    else
        glDisable(GL_LIGHTING);

    // Set polygon rendering mode and smoothing
    glPolygonMode(GL_FRONT_AND_BACK, fillMode_);
    if (fillMode_ == GL_POINT)
        glEnable(GL_POINT_SMOOTH);
    else
        glDisable(GL_POINT_SMOOTH);
    if (fillMode_ == GL_LINE)
        glEnable(GL_LINE_SMOOTH);
    else
        glDisable(GL_LINE_SMOOTH);
}

/*
 * LineStylePrimitiveInfo
 */

LineStylePrimitiveInfo::LineStylePrimitiveInfo(LineStyle style) : lineStyle_(style) {}

LineStylePrimitiveInfo::~LineStylePrimitiveInfo() {}

// Expose contained info to GL
void LineStylePrimitiveInfo::sendToGL(double pixelScaling) { lineStyle_.sendToGL(pixelScaling); }
