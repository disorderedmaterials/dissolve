/*
	*** Text Primitive List
	*** src/gui/uchroma/render/textprimitivelist.h
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

#ifndef DISSOLVE_UCHROMATEXTPRIMITIVELIST_H
#define DISSOLVE_UCHROMATEXTPRIMITIVELIST_H

#include "gui/uchroma/render/textprimitive.h"
#include "math/matrix4.h"
#include "math/cuboid.h"
#include "templates/vector3.h"
#include "templates/list.h"
#include <QString>

// Forward Declarations
class FontInstance;
class ViewPane;

// Text Primitive List
class TextPrimitiveList
{
	public:
	// Constructor
	TextPrimitiveList();

	private:
	// List of text primitive chunks
	List<TextPrimitive> textPrimitives_;

	public:
	// Clear list
	void clear();
	// Add primitive to list
	void add(FontInstance& fontInstance, QString text, Vec3<double> anchorPoint, TextPrimitive::TextAnchor anchorPosition, Vec3<double> adjustmentVector, Matrix4& rotation, double textSize, bool flat);
	// Update global bounding cuboid for all text primitives in the list
	Cuboid boundingCuboid(FontInstance& fontInstance, const Matrix4& viewMatrixInverse, double baseFontSize, Cuboid startingCuboid = Cuboid());
	// Render all primitives in list
	void renderAll(FontInstance& fontInstance, const Matrix4& viewMatrix, const Matrix4& viewMatrixInverse, double baseFontSize);
};

#endif
