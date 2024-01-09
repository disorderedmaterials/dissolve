// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/render/textPrimitive.h"
#include "math/cuboid.h"
#include "math/matrix4.h"
#include "templates/vector3.h"
#include <QString>

// Forward Declarations
class FontInstance;

// Text Primitive List
class TextPrimitiveList
{
    public:
    TextPrimitiveList() = default;
    ~TextPrimitiveList() = default;

    private:
    // List of text primitives
    std::vector<TextPrimitive> textPrimitives_;

    public:
    // Clear list
    void clear();
    // Add primitive to list
    void add(FontInstance *fontInstance, QString text, Vec3<double> anchorPoint, TextPrimitive::TextAnchor anchorPosition,
             Vec3<double> adjustmentVector, Matrix4 &rotation, double textSize, bool flat);
    // Update global bounding cuboid for all text primitives in the list
    Cuboid boundingCuboid(FontInstance &fontInstance, const Matrix4 &viewMatrixInverse, double baseFontSize,
                          Cuboid startingCuboid = Cuboid());
    // Render all primitives in list
    void renderAll(FontInstance &fontInstance, const Matrix4 &viewMatrix, const Matrix4 &viewMatrixInverse,
                   double baseFontSize);
};
