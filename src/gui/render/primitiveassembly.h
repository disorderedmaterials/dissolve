/*
	*** Rendering Primitive Assembly
	*** src/gui/render/primitiveassembly.h
	Copyright T. Youngs 2013-2019

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

#ifndef DISSOLVE_RENDER_PRIMITIVEASSEMBLY_H
#define DISSOLVE_RENDER_PRIMITIVEASSEMBLY_H

#include "gui/render/primitiveinfo.h"
#include "templates/array.h"
#include "templates/factory.h"
#include "templates/listitem.h"

// Forward Declarations
/* none */

// Rendering Primitive List
class PrimitiveAssembly : public ListItem<PrimitiveAssembly>
{
	public:
	// Constructor / Destructor
	PrimitiveAssembly();
	~PrimitiveAssembly();


	/*
	 * Object Factories
	 */
	private:
	// Factories for all PrimitiveInfo-derived classes
	ObjectFactory<UncolouredPrimitiveInfo> uncolouredPrimitiveFactory_;
	ObjectFactory<ColouredPrimitiveInfo> colouredPrimitiveFactory_;
	ObjectFactory<StylePrimitiveInfo> stylePrimitiveFactory_;
	ObjectFactory<LineStylePrimitiveInfo> lineStylePrimitiveFactory_;


	/*
	 * Assembly
	 */
	private:
	// Array of PrimitiveInfo pointers that constitute our assembly
	Array<PrimitiveInfo*> assembly_;

	public:
	// Clear existing data
	void clear();
	// Add Primitive to the assembly
	void add(Primitive* primitive, const Matrix4& matrix);
	// Add Primitive with colour to the assembly
	void add(Primitive* primitive, const Matrix4& matrix, const GLfloat* rgba);
	// Add Primitive with colour to the assembly
	void add(Primitive* primitive, const Matrix4& matrix, GLfloat r, GLfloat g, GLfloat b, GLfloat a);
	// Add styling information
	void add(bool lighting, GLenum polygonFillMode);
	// Add line styling information
	void add(LineStyle lineStyle);


	/*
	 * GL
	 */
	public:
	// Send to OpenGL (i.e. render)
	void sendToGL(double pixelScaling);
};

#endif
