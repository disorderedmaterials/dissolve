// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/primitiveAssembly.h"
#include <array>

PrimitiveAssembly::PrimitiveAssembly() {}

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
    UncolouredPrimitiveInfo &pi = uncolouredPrimitiveFactory_.emplace_back();
    pi = UncolouredPrimitiveInfo(primitive, matrix);
    assembly_.push_back(&pi);
}

// Add Primitive with colour to the assembly
void PrimitiveAssembly::add(Primitive *primitive, const Matrix4 &matrix, const std::array<float, 4> &rgba)
{
    ColouredPrimitiveInfo &pi = colouredPrimitiveFactory_.emplace_back();
    pi = ColouredPrimitiveInfo(primitive, matrix, rgba[0], rgba[1], rgba[2], rgba[3]);
    assembly_.push_back(&pi);
}

// Add Primitive with colour to the assembly
void PrimitiveAssembly::add(Primitive *primitive, const Matrix4 &matrix, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    ColouredPrimitiveInfo &pi = colouredPrimitiveFactory_.emplace_back();
    pi = ColouredPrimitiveInfo(primitive, matrix, r, g, b, a);
    assembly_.push_back(&pi);
}

// Add styling information
void PrimitiveAssembly::add(bool lighting, GLenum polygonFillMode)
{
    StylePrimitiveInfo &pi = stylePrimitiveFactory_.emplace_back();
    pi = StylePrimitiveInfo(lighting, polygonFillMode);
    assembly_.push_back(&pi);
}

// Add line styling information
void PrimitiveAssembly::add(LineStyle lineStyle)
{
    LineStylePrimitiveInfo &pi = lineStylePrimitiveFactory_.emplace_back();
    pi = LineStylePrimitiveInfo(lineStyle);
    assembly_.push_back(&pi);
}

/*
 * Object
 */

// Create cylinder bond between supplied atoms in specified assembly
void PrimitiveAssembly::createCylinderBond(Primitive *bondPrimitive, Vec3<double> rI, Vec3<double> rJ, Vec3<double> vij,
                                           const std::array<float, 4> &colI, const std::array<float, 4> &colJ,
                                           bool drawFromAtoms, double radialScaling)
{
    Matrix4 A;
    auto unit = vij;
    const auto mag = unit.magAndNormalise();

    // Create rotation matrix for Bond
    A.setColumn(2, unit.x, unit.y, unit.z, 0.0);
    A.setColumn(0, unit.orthogonal(), 0.0);
    A.setColumn(1, unit * A.columnAsVec3(0), 0.0);
    A.columnMultiply(2, 0.5 * mag);
    A.applyScaling(radialScaling, radialScaling, 1.0);

    // If drawing from individual Atoms, locate on each Atom and draw the bond halves from there. If not, locate to the bond
    // centre.
    if (drawFromAtoms)
    {
        // Render half of Bond in colour of Atom j
        A.setTranslation(rI);
        add(bondPrimitive, A, colJ);

        // Render half of Bond in colour of Atom i
        A.setTranslation(rJ);
        A.columnMultiply(2, -1.0);
        add(bondPrimitive, A, colI);
    }
    else
    {
        A.setTranslation(rI + vij * 0.5);

        // Render half of Bond in colour of Atom j
        add(bondPrimitive, A, colJ);

        // Render half of Bond in colour of Atom i
        A.columnMultiply(2, -1.0);
        add(bondPrimitive, A, colI);
    }
}

/*
 * GL
 */

// Send to OpenGL (i.e. render)
void PrimitiveAssembly::sendToGL(double pixelScaling)
{
    for (auto *prim : assembly_)
        prim->sendToGL(pixelScaling);
}
