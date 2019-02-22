/*
	*** Primitive Info
	*** src/gui/viewer/render/primitiveinfo.h
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

#ifndef DISSOLVE_PRIMITIVEINFO_H
#define DISSOLVE_PRIMITIVEINFO_H

#include "math/matrix4.h"

// Forward Declarations
class Primitive;

// Primitive Info
class PrimitiveInfo
{
	public:
	// Constructors
	PrimitiveInfo();
	PrimitiveInfo(GLfloat lineWidth);
	PrimitiveInfo(GLenum fillMode);
	PrimitiveInfo(Primitive* prim, Matrix4& transform);
	PrimitiveInfo(Primitive* prim, Matrix4& transform, GLfloat* colour);
	// Clear
	void clear();

	/*
	 * Info Type
	 */
	public:
	// PrimitiveInfo Type
	enum PrimitiveInfoType { ObjectInfoType, ColourInfoType, FillModeInfoType, LineWidthInfoType, nPrimitiveInfoTypes };

	private:
	// Flags indicating which types of info are contained
	bool infoType_[nPrimitiveInfoTypes];


	/*
	 * Object Information
	 */
	private:
	// Target primitive
	Primitive* primitive_;
	// Transformation to apply before rendering Primitive
	Matrix4 transform_;
	// Whether to draw the Primitive as filled or wireframe polygons
	GLenum fillMode_;
	// Colour of Primitive (if relevant)
	GLfloat colour_[4];
	// GL object line width (if type_ == GL_LINE or chunk primitive type == GL_LINES)
	GLfloat lineWidth_;
	
	public:
	// Set Primitive, transform, and colour
	void setPrimitive(Primitive* prim, Matrix4& transform, GLfloat* colour);
	// Set Primitive and transform
	void setPrimitive(Primitive* prim, Matrix4& transform);
	// Set polygon fill mode
	void setFillMode(GLenum fillMode);
	// Set line width
	void setLineWidth(GLfloat lineWidth);


	/*
	 * GL
	 */
	public:
	// Expose contained info to GL
	void sendToGL();
};

#endif
