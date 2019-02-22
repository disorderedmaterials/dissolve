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

// Constructor
PrimitiveInfo::PrimitiveInfo()
{
	clear();
}

// Clear
void PrimitiveInfo::clear()
{
	// Private variables
	primitive_ = NULL;
	fillMode_ = GL_FILL;
	lineWidth_ = 1.0f;
	colour_[0] = 0.0;
	colour_[1] = 0.0;
	colour_[2] = 0.0;
	colour_[3] = 1.0;

	// Content flags
	for (int n=0; n<nPrimitiveInfoTypes; ++n) infoType_[n] = false;
}

/*
 * Object Information
 */

// Set Primitive, transform, and colour
void PrimitiveInfo::setPrimitive(Primitive* prim, Matrix4& transform, GLfloat* colour)
{
	primitive_ = prim;
	transform_ = transform;
	for (int n=0; n<4; ++n) colour_[n] = colour[n];

	infoType_[PrimitiveInfo::ObjectInfoType] = true;
	infoType_[PrimitiveInfo::ColourInfoType] = true;
}

// Set Primitive and transform
void PrimitiveInfo::setPrimitive(Primitive* prim, Matrix4& transform)
{
	primitive_ = prim;
	transform_ = transform;

	infoType_[PrimitiveInfo::ObjectInfoType] = true;
}

// Set polygon fill mode
void PrimitiveInfo::setFillMode(GLenum fillMode)
{
	fillMode_ = fillMode;

	infoType_[PrimitiveInfo::FillModeInfoType] = true;
}

// Set line width
void PrimitiveInfo::setLineWidth(GLfloat lineWidth)
{
	lineWidth_ = lineWidth;

	infoType_[PrimitiveInfo::LineWidthInfoType] = true;
}

/*
 * GL
 */

// Expose contained info to GL
void PrimitiveInfo::sendToGL()
{
	// Apply styles first
	if (infoType_[PrimitiveInfo::ColourInfoType]) glColor4fv(colour_);
	if (infoType_[PrimitiveInfo::FillModeInfoType]) glPolygonMode(GL_FRONT_AND_BACK, fillMode_);
	if (infoType_[PrimitiveInfo::LineWidthInfoType]) glLineWidth(lineWidth_);

	// Render Primitive if one is present
	if (primitive_)
	{
		glPushMatrix();
		glLoadMatrixd(transform_.matrix());
		primitive_->sendToGL();
		glPopMatrix();
	}
}
