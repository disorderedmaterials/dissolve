/*
	*** Rendering Primitive Assembly
	*** src/gui/viewer/render/primitiveassembly.cpp
	Copyright T. Youngs 2019

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

#include "gui/viewer/render/primitiveassembly.h"

// Constructor
PrimitiveAssembly::PrimitiveAssembly() : ListItem<PrimitiveAssembly>()
{
}

// Destructor
PrimitiveAssembly::~PrimitiveAssembly()
{
}

// Clear all existing primitives
void PrimitiveAssembly::clear()
{
	// Return all objects to their 
	assembly_.clear();
}

// Add Primitive to the assembly
void PrimitiveAssembly::add(Primitive* primitive, const Matrix4& matrix)
{
	UncolouredPrimitiveInfo* pi = uncolouredPrimitiveFactory_.produce();
	(*pi) = UncolouredPrimitiveInfo(primitive, matrix);
	assembly_.add(pi);
}

// Add Primitive with colour to the assembly
void PrimitiveAssembly::add(Primitive* primitive, const Matrix4& matrix, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	ColouredPrimitiveInfo* pi = colouredPrimitiveFactory_.produce();
	(*pi) = ColouredPrimitiveInfo(primitive, matrix, r, g, b, a);
	assembly_.add(pi);
}

// Add styling information
void PrimitiveAssembly::add(bool lighting, GLenum polygonFillMode)
{
	StylePrimitiveInfo* pi = stylePrimitiveFactory_.produce();
	(*pi) = StylePrimitiveInfo(lighting, polygonFillMode);
	assembly_.add(pi);
}

// Add line styling information
void PrimitiveAssembly::add(LineStyle lineStyle)
{
	LineStylePrimitiveInfo* pi = lineStylePrimitiveFactory_.produce();
	(*pi) = LineStylePrimitiveInfo(lineStyle);
	assembly_.add(pi);
}

/*
 * GL
 */

// Send to OpenGL (i.e. render)
void PrimitiveAssembly::sendToGL(double pixelScaling)
{
	for (int n=0; n<assembly_.nItems(); ++n) assembly_[n]->sendToGL(pixelScaling);
}
