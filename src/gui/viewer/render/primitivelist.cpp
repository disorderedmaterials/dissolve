/*
	*** Rendering Primitive List
	*** src/gui/uchroma/render/primitivelist.cpp
	Copyright T. Youngs 2013-2019

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

#include "gui/uchroma/render/primitivelist.h"

// Constructor
PrimitiveList::PrimitiveList()
{
}

// Destructor
PrimitiveList::~PrimitiveList()
{
}

// Clear all existing primitives
void PrimitiveList::clear()
{
// 	for (Primitive* prim = primitives_.first(); prim != NULL; prim = prim->next) prim->clear();
}

// Forget all data, leaving arrays intact
void PrimitiveList::forgetAll()
{
	for (Primitive* prim = primitives_.first(); prim != NULL; prim = prim->next) prim->forgetAll();
}

// Resize list so it is large enough to accommodate specified number of Primitives
void PrimitiveList::reinitialise(int newSize, bool allowShrink, GLenum type, bool colourData)
{
	// Add enough primitives to match the new size
	while (primitives_.nItems() < newSize) primitives_.add();

	// Shrink list to new size (if allowed)
	if (allowShrink)
	{
		// Manage primitive list, reusing where possible...
		while (primitives_.nItems() > newSize) primitives_.removeLast();
	}

	// Loop over all current primitives and set information
	for (Primitive* prim = primitives_.first(); prim != NULL; prim = prim->next)
	{
		prim->initialise(type, colourData);
	}
}

// Add a new primitive to the end of the list
Primitive* PrimitiveList::addPrimitive(GLenum type, bool colourData)
{
	Primitive* newPrim = primitives_.add();
	newPrim->initialise(type, colourData);

	return newPrim;
}

// Return total number of defined vertices
int PrimitiveList::nDefinedVertices()
{
	int totalVertices = 0;
	for (Primitive* prim = primitives_.first(); prim != NULL; prim = prim->next) totalVertices += prim->nDefinedVertices();
	return totalVertices;
}

// Return total number of defined indices
int PrimitiveList::nDefinedIndices()
{
	int totalIndices = 0;
	for (Primitive* prim = primitives_.first(); prim != NULL; prim = prim->next) totalIndices += prim->nDefinedIndices();
	return totalIndices;
}

// Push instance layer
void PrimitiveList::pushInstance(const QOpenGLContext* context)
{
	for (Primitive* prim = primitives_.first(); prim != NULL; prim = prim->next) prim->pushInstance(context);
}

// Pop topmost instance layer
void PrimitiveList::popInstance(const QOpenGLContext* context)
{
	for (Primitive* prim = primitives_.first(); prim != NULL; prim = prim->next) prim->popInstance(context);
}

// Return number of instances of topmost primitive
int PrimitiveList::nInstances()
{
	if (primitives_.nItems() == 0) return 0;
	else return primitives_.first()->nInstances();
}

// Send to OpenGL (i.e. render)
void PrimitiveList::sendToGL()
{
	for (Primitive* prim = primitives_.first(); prim != NULL; prim = prim->next) prim->sendToGL();
}

/*
 * Operators
 */

// Element access operator
Primitive* PrimitiveList::operator[](int index)
{
	return primitives_[index];
}
