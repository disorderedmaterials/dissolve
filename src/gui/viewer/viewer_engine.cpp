/*
	*** Viewer Main Engine
	*** src/gui/viewer/viewer_engine.cpp
	Copyright T. Youngs 2012-2013

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

#include "base/messenger.h"
#include "gui/viewer/viewer.hui"

// Render primitive in specified colour
void Viewer::renderPrimitive(Primitive* primitive, const GLfloat* colour, Matrix4& transform, GLenum fillMode, GLfloat lineWidth)
{
	if ((fillMode != GL_FILL) || ((colour != NULL) && (colour[3] > 0.99f)))
	{
		// Add primitive info to solid objects list
		PrimitiveInfo* pi = solidPrimitives_.add();
		pi->set(primitive, colour, transform, fillMode, lineWidth);
	}
	else
	{
		// Add primitive info to transparent objects list
		PrimitiveInfo* pi = transparentPrimitives_.add();
		pi->set(primitive, colour, transform);
	}
}

// Add text primitive for rendering later
void Viewer::renderTextPrimitive(Vec3<double> pos, QString text, bool isIsotope, bool rightalign)
{
	if (isIsotope) isotopeTextPrimitives_.add(pos, text, rightalign);
	else textPrimitives_.add(pos, text, rightalign);
}

// Sort and render filtered polygons by depth
void Viewer::sortAndSendGL()
{
	Matrix4 A;
	Primitive* prim;

	// Transform and render each solid primitive in each list
	for (PrimitiveInfo* pi = solidPrimitives_.first(); pi != NULL; pi = pi->next)
	{
		// Retrieve pointer to primitive
		prim = pi->primitive();
		if (prim == NULL) continue;

		// If colour data is not present in the vertex data array, use the colour stored in the PrimitiveInfo object
		if (!prim->colouredVertexData()) glColor4fv(pi->colour());
		glPolygonMode(GL_FRONT_AND_BACK, pi->fillMode());
		if (pi->fillMode() == GL_LINE)
		{
			glLineWidth(pi->lineWidth());
			glDisable(GL_LIGHTING);
		}
		else if (pi->fillMode() == GL_POINT)
		{
			glPointSize(1.0);
			glDisable(GL_LIGHTING);
		}
		A = viewMatrix_ * pi->localTransform();
		glLoadMatrixd(A.matrix());
		prim->sendToGL();
		
		if (pi->fillMode() == GL_LINE)
		{
			glLineWidth(1.0);
			glEnable(GL_LIGHTING);
		}
		else if (pi->fillMode() == GL_POINT) glEnable(GL_LIGHTING);
	}

	// Draw line primitives
	glDisable(GL_LIGHTING);
	glLoadMatrixd(viewMatrix_.matrix());
	glLineWidth(1.0);
// 	printf("NLineVertices = %i\n", linePrimitives_.nDefinedVertices());
	linePrimitives_.sendToGL();
	glPointSize(1.0);
	pointPrimitives_.sendToGL();
// 	glLineWidth(prefs.stickLineSelectedWidth());
// 	stickSelectedLines_.sendToGL();
	glEnable(GL_LIGHTING);
	glLineWidth(1.0);
	
	// Render transparent primitives
	for (PrimitiveInfo* pi = transparentPrimitives_.first(); pi != NULL; pi = pi->next)
	{
		// Grab primitive pointer
		prim = pi->primitive();
		if (prim == NULL) continue;
		if (!prim->colouredVertexData()) glColor4fv(pi->colour());
		A = viewMatrix_ * pi->localTransform();
		glLoadMatrixd(A.matrix());
		prim->sendToGL();
	}
}

