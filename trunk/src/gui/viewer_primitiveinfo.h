/*
	*** Primitive Info
	*** src/gui/viewer_primitiveinfo.h
	Copyright T. Youngs 2012-2013

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_PRIMITIVEINFO_H
#define DUQ_PRIMITIVEINFO_H

#include "math/matrix4.h"

// Forward Declarations
class Primitive;

// Primitive Info
class PrimitiveInfo
{
	public:
	// Constructor
	PrimitiveInfo();
	// List pointer
	PrimitiveInfo* prev, *next;

	private:
	// Target primitive
	Primitive* primitive_;
	// Local transformation of primitive
	Matrix4 localTransform_;
	// Colour of primitive (if vertexData_ doesn't contain colour information)
	GLfloat colour_[4];
	// Whether to draw the primitive as filled or wireframe polygons
	GLenum fillMode_;
	// GL object line width (if type_ == GL_LINE or chunk primitive type == GL_LINES)
	GLfloat lineWidth_;
	
	public:
	// Set primitive info data
	void set(Primitive* prim, const GLfloat* colour, Matrix4& transform, GLenum fillMode = GL_FILL, GLfloat lineWidth = 1.0f);
	// Return pointer to stored primitive
	Primitive* primitive();
	// Return local transformation of primitive
	Matrix4& localTransform();
	// Return colour array
	GLfloat* colour();
	// Return polygon fill mode
	GLenum fillMode();
	// Return line width
	GLfloat lineWidth();
};

#endif
