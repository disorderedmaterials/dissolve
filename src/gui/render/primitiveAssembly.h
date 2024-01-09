// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/render/primitiveInfo.h"
#include <deque>
#include <vector>

// Rendering Primitive List
class PrimitiveAssembly
{
    public:
    PrimitiveAssembly();
    ~PrimitiveAssembly();

    /*
     * Object Factories
     */
    private:
    // Factories for all PrimitiveInfo-derived classes
    std::deque<UncolouredPrimitiveInfo> uncolouredPrimitiveFactory_;
    std::deque<ColouredPrimitiveInfo> colouredPrimitiveFactory_;
    std::deque<StylePrimitiveInfo> stylePrimitiveFactory_;
    std::deque<LineStylePrimitiveInfo> lineStylePrimitiveFactory_;

    /*
     * Assembly
     */
    private:
    // Array of PrimitiveInfo pointers that constitute our assembly
    std::vector<PrimitiveInfo *> assembly_;

    public:
    // Clear existing data
    void clear();
    // Add Primitive to the assembly
    void add(Primitive *primitive, const Matrix4 &matrix);
    // Add Primitive with colour to the assembly
    void add(Primitive *primitive, const Matrix4 &matrix, const std::array<float, 4> &rgba);
    // Add Primitive with colour to the assembly
    void add(Primitive *primitive, const Matrix4 &matrix, GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    // Add styling information
    void add(bool lighting, GLenum polygonFillMode);
    // Add line styling information
    void add(LineStyle lineStyle);

    /*
     * Objects
     */
    public:
    // Create cylinder bond between supplied atoms in specified assembly
    void createCylinderBond(Primitive *bondPrimitive, Vec3<double> rI, Vec3<double> rJ, Vec3<double> vij,
                            const std::array<float, 4> &colI, const std::array<float, 4> &colJ, bool drawFromAtoms,
                            double radialScaling);

    /*
     * GL
     */
    public:
    // Send to OpenGL (i.e. render)
    void sendToGL(double pixelScaling);
};
