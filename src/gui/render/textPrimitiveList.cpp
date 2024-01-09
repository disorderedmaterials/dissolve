// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/textPrimitiveList.h"
#include "gui/render/fontInstance.h"
#include "gui/render/view.h"

// Clear list
void TextPrimitiveList::clear() { textPrimitives_.clear(); }

// Set data from literal coordinates and text
void TextPrimitiveList::add(FontInstance *fontInstance, QString text, Vec3<double> anchorPoint,
                            TextPrimitive::TextAnchor anchorPosition, Vec3<double> adjustmentVector, Matrix4 &localRotation,
                            double textSize, bool flat)
{
    auto &primitive = textPrimitives_.emplace_back();
    primitive.set(fontInstance, text, anchorPoint, anchorPosition, adjustmentVector, localRotation, textSize, flat);
}

// Update global bounding cuboid for all text primitives in the list
Cuboid TextPrimitiveList::boundingCuboid(FontInstance &fontInstance, const Matrix4 &viewMatrixInverse, double baseFontSize,
                                         Cuboid startingCuboid)
{
    Cuboid result = startingCuboid;
    Matrix4 textMatrix;
    Vec3<double> corners[4], local;
    for (const auto &primitive : textPrimitives_)
    {
        // Get transformation matrix and bounding box for text
        textMatrix = primitive.transformationMatrix(fontInstance, viewMatrixInverse, baseFontSize);
        primitive.boundingBox(fontInstance, corners[0], corners[1]);
        corners[2].set(corners[0].x, corners[1].y, 0.0);
        corners[3].set(corners[1].x, corners[0].y, 0.0);

        // Transform the four corners of the bounding box with the text primitive's transformation matrix
        // and determine the extreme x, y, and z coordinates of the primitives in the local frame
        for (auto m = 0; m < 4; ++m)
        {
            local = textMatrix * corners[m];
            result.updateExtremes(local);
        }
    }

    return result;
}

// Render all primitives in list
void TextPrimitiveList::renderAll(FontInstance &fontInstance, const Matrix4 &viewMatrix, const Matrix4 &viewMatrixInverse,
                                  double baseFontSize)
{
    if (!fontInstance.fontOK())
        return;

    for (const auto &primitive : textPrimitives_)
        primitive.render(fontInstance, viewMatrix, viewMatrixInverse, baseFontSize);
}
