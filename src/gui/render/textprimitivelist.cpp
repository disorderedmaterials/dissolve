/*
    *** Text Primitive List
    *** src/gui/render/textprimitivelist.cpp
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

#include "gui/render/textprimitivelist.h"
#include "gui/render/fontinstance.h"
#include "gui/render/view.h"

TextPrimitiveList::TextPrimitiveList() {}

// Clear list
void TextPrimitiveList::clear() { textPrimitives_.clear(); }

// Set data from literal coordinates and text
void TextPrimitiveList::add(FontInstance &fontInstance, QString text, Vec3<double> anchorPoint,
                            TextPrimitive::TextAnchor anchorPosition, Vec3<double> adjustmentVector, Matrix4 &localRotation,
                            double textSize, bool flat)
{
    TextPrimitive *primitive = textPrimitives_.add();
    primitive->set(fontInstance, text, anchorPoint, anchorPosition, adjustmentVector, localRotation, textSize, flat);
}

// Update global bounding cuboid for all text primitives in the list
Cuboid TextPrimitiveList::boundingCuboid(FontInstance &fontInstance, const Matrix4 &viewMatrixInverse, double baseFontSize,
                                         Cuboid startingCuboid)
{
    Cuboid result = startingCuboid;
    Matrix4 textMatrix;
    Vec3<double> corners[4], local;
    for (auto *primitive = textPrimitives_.first(); primitive != nullptr; primitive = primitive->next())
    {
        // Get transformation matrix and bounding box for text
        textMatrix = primitive->transformationMatrix(fontInstance, viewMatrixInverse, baseFontSize);
        primitive->boundingBox(fontInstance, corners[0], corners[1]);
        corners[2].set(corners[0].x, corners[1].y, 0.0);
        corners[3].set(corners[1].x, corners[0].y, 0.0);

        // Transform the four corners of the bounding box with the text primitive's transformation matrix
        // and determine the extreme x, y, and z coordinates of the primitives in the local frame
        for (int m = 0; m < 4; ++m)
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

    for (auto *primitive = textPrimitives_.first(); primitive != nullptr; primitive = primitive->next())
        primitive->render(fontInstance, viewMatrix, viewMatrixInverse, baseFontSize);
}
