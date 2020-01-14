/*
	*** Primitive Info
	*** src/gui/render/primitiveinfo.cpp
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

#include "gui/render/primitiveinfo.h"
#include "gui/render/primitive.h"

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
void UncolouredPrimitiveInfo::sendToGL(double pixelScaling)
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
void ColouredPrimitiveInfo::sendToGL(double pixelScaling)
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
StylePrimitiveInfo::StylePrimitiveInfo(bool lighting, GLenum polygonFillMode) : lighting_(lighting), fillMode_(polygonFillMode)
{
}

// Destructor
StylePrimitiveInfo::~StylePrimitiveInfo()
{
}

// Expose contained info to GL
void StylePrimitiveInfo::sendToGL(double pixelScaling)
{
	// Enable / disable lighting
	if (lighting_) glEnable(GL_LIGHTING);
	else glDisable(GL_LIGHTING);

	// Set polygon rendering mode and smoothing
	glPolygonMode(GL_FRONT_AND_BACK, fillMode_);
	if (fillMode_ == GL_POINT) glEnable(GL_POINT_SMOOTH);
	else glDisable(GL_POINT_SMOOTH);
	if (fillMode_ == GL_LINE) glEnable(GL_LINE_SMOOTH);
	else glDisable(GL_LINE_SMOOTH);
}

/*
 * LineStylePrimitiveInfo
 */

// Constructor
LineStylePrimitiveInfo::LineStylePrimitiveInfo(LineStyle style) : lineStyle_(style)
{
}

// Destructor
LineStylePrimitiveInfo::~LineStylePrimitiveInfo()
{
}

// Expose contained info to GL
void LineStylePrimitiveInfo::sendToGL(double pixelScaling)
{
	lineStyle_.sendToGL(pixelScaling);
}
