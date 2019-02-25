/*
	*** Primitive Info
	*** src/gui/viewer/render/primitiveinfo.cpp
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

#include "gui/viewer/render/primitiveinfo.h"
#include "gui/viewer/render/primitive.h"

/*
 * PrimitiveInfo
 */

// Constructor
PrimitiveInfo::PrimitiveInfo()
{
}

// Destructor
PrimitiveInfo::~PrimitiveInfo()
{
}

/*
 * UncolouredPrimitiveInfo
 */

// Constructor
UncolouredPrimitiveInfo::UncolouredPrimitiveInfo(Primitive* prim, Matrix4 transform) : primitive_(prim), transform_(transform)
{
}

// Destructor
UncolouredPrimitiveInfo::~UncolouredPrimitiveInfo()
{
}

// Expose contained info to GL
void UncolouredPrimitiveInfo::sendToGL()
{
	// Render Primitive if one is present
	if (primitive_)
	{
		glPushMatrix();
		glMultMatrixd(transform_.matrix());
		primitive_->sendToGL();
		glPopMatrix();
	}
}

/*
 * ColouredPrimitiveInfo
 */

// Constructor
ColouredPrimitiveInfo::ColouredPrimitiveInfo(Primitive* prim, Matrix4 transform, GLfloat r, GLfloat g, GLfloat b, GLfloat a) : primitive_(prim), transform_(transform)
{
	colour_[0] = r;
	colour_[1] = g;
	colour_[2] = b;
	colour_[3] = a;
}

// Destructor
ColouredPrimitiveInfo::~ColouredPrimitiveInfo()
{
}

// Expose contained info to GL
void ColouredPrimitiveInfo::sendToGL()
{
	// Apply colour
	glColor4fv(colour_);

	// Render Primitive if one is present
	if (primitive_)
	{
		glPushMatrix();
		glMultMatrixd(transform_.matrix());
		primitive_->sendToGL();
		glPopMatrix();
	}
}

/*
 * StylePrimitiveInfo
 */

// Constructor
StylePrimitiveInfo::StylePrimitiveInfo(bool lighting, GLenum polygonFillMode, GLfloat lineWidth) : lighting_(lighting), fillMode_(polygonFillMode), lineWidth_(lineWidth)
{
}

// Destructor
StylePrimitiveInfo::~StylePrimitiveInfo()
{
}

// Expose contained info to GL
void StylePrimitiveInfo::sendToGL()
{
	// Apply style
	if (lighting_) glEnable(GL_LIGHTING);
	else glDisable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, fillMode_);
	glLineWidth(lineWidth_);
}
