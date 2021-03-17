// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/render/primitiveassembly.h"

PrimitiveAssembly::PrimitiveAssembly() : ListItem<PrimitiveAssembly>() {}

PrimitiveAssembly::~PrimitiveAssembly() {}

// Clear all existing primitives
void PrimitiveAssembly::clear()
{
    // Return all objects to their
    assembly_.clear();
}

// Add Primitive to the assembly
void PrimitiveAssembly::add(Primitive *primitive, const Matrix4 &matrix)
{
    UncolouredPrimitiveInfo *pi = uncolouredPrimitiveFactory_.produce();
    (*pi) = UncolouredPrimitiveInfo(primitive, matrix);
    assembly_.add(pi);
}

// Add Primitive with colour to the assembly
void PrimitiveAssembly::add(Primitive *primitive, const Matrix4 &matrix, const GLfloat *rgba)
{
    ColouredPrimitiveInfo *pi = colouredPrimitiveFactory_.produce();
    (*pi) = ColouredPrimitiveInfo(primitive, matrix, rgba[0], rgba[1], rgba[2], rgba[3]);
    assembly_.add(pi);
}

// Add Primitive with colour to the assembly
void PrimitiveAssembly::add(Primitive *primitive, const Matrix4 &matrix, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    ColouredPrimitiveInfo *pi = colouredPrimitiveFactory_.produce();
    (*pi) = ColouredPrimitiveInfo(primitive, matrix, r, g, b, a);
    assembly_.add(pi);
}

// Add styling information
void PrimitiveAssembly::add(bool lighting, GLenum polygonFillMode)
{
    StylePrimitiveInfo *pi = stylePrimitiveFactory_.produce();
    (*pi) = StylePrimitiveInfo(lighting, polygonFillMode);
    assembly_.add(pi);
}

// Add line styling information
void PrimitiveAssembly::add(LineStyle lineStyle)
{
    LineStylePrimitiveInfo *pi = lineStylePrimitiveFactory_.produce();
    (*pi) = LineStylePrimitiveInfo(lineStyle);
    assembly_.add(pi);
}

/*
 * GL
 */

// Send to OpenGL (i.e. render)
void PrimitiveAssembly::sendToGL(double pixelScaling)
{
    for (auto n = 0; n < assembly_.nItems(); ++n)
        assembly_[n]->sendToGL(pixelScaling);
}
