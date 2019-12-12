/*
	*** Rendering Primitive List
	*** src/gui/viewer/render/primitivelist.h
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

#ifndef DISSOLVE_RENDER_PRIMITIVELIST_H
#define DISSOLVE_RENDER_PRIMITIVELIST_H

#include "gui/viewer/render/primitive.h"
#include "templates/list.h"

// Forward Declarations
class QOpenGLContext;

// Rendering Primitive List
class PrimitiveList
{
	public:
	// Constructor / Destructor
	PrimitiveList();
	~PrimitiveList();


	/*
	 * Data
	 */
	private:
	// List of Primitives owned and managed by this list
	List<Primitive> primitives_;

	public:
	// Clear all existing Primitives
	void clear();
	// Forget all data, leaving arrays intact
	void forgetAll();
	// Reinitialise list so it is large enough to accomodate specified number of Primitives
	void reinitialise(int newSize, GLenum type, bool colourData);
	// Add a new Primitive to the end of the list
	Primitive* add(GLenum type = GL_LINES, bool colourData = false);
	// Register an existing Primitive with the list
	void add(Primitive* primitive);
	// Remove specified Primitive
	void remove(Primitive* primitive);
	// Return total number of defined vertices
	int nDefinedVertices();
	// Return total number of defined indices
	int nDefinedIndices();
	// Push instance layer
	void pushInstance(const QOpenGLContext* context);
	// Pop topmost instance layer
	void popInstance(const QOpenGLContext* context);
	// Return number of instances of topmost primitive
	int nInstances();
	// Send to OpenGL (i.e. render)
	void sendToGL();


	/*
	 * Operators
	 */
	public:
	// Element access operator
	Primitive* operator[](int index);
};

#endif
