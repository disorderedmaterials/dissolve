/*
	*** Rendering Primitive
	*** src/gui/viewer/render/primitive.cpp
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

#include "gui/viewer/render/primitive.h"
#include "base/messenger.h"
#include <string.h>

// Constructor
Primitive::Primitive() : ListItem<Primitive>()
{
	colouredVertexData_ = false;
	type_ = GL_TRIANGLES;
	dataPerVertex_ = 6;
	nDefinedVertices_ = 0;
	useInstances_ = true;
}

// Destructor
Primitive::~Primitive()
{
}

/*
 * Data
 */

// Initialise primitive
void Primitive::initialise(GLenum type, bool colourData)
{
	type_ = type;
	colouredVertexData_ = colourData;

	// Set data per vertex based on the primitive type, and whether we have individual colour data or not
	dataPerVertex_ = (colouredVertexData_ ? 10 : 6);
	if (type_ == GL_TRIANGLES) verticesPerType_ = 3;
	else if ((type_ == GL_LINES) || (type_ == GL_LINE_LOOP) || (type_ == GL_LINE_STRIP)) verticesPerType_ = 2;
	else if (type_ == GL_POINTS) verticesPerType_ = 1;
	else printf("Warning - Invalid GLenum type given to VertexChunk::initialise (%i)\n", type_);

	forgetAll();
}

// Forget all vertex and index data
void Primitive::forgetAll()
{
	vertexData_.clear();
	indexData_.clear();
	nDefinedVertices_ = 0;
}

// Return number of vertices currently defined in primitive
int Primitive::nDefinedVertices() const
{
	return vertexData_.nItems();
}

// Return number of indices currently defined in primitive
int Primitive::nDefinedIndices() const
{
	return indexData_.nItems();
}

// Return whether vertex data contains colour information
bool Primitive::colouredVertexData() const
{
	return colouredVertexData_;
}

/*
 * Instances
 */

// Flag that this primitive should not use instances (rendering will use vertex arrays)
void Primitive::setNoInstances()
{
	useInstances_ = false;
}


// Push instance of primitive
void Primitive::pushInstance(const QOpenGLContext* context)
{
	// Does this primitive use instances?
	if (!useInstances_) return;

	// Clear the error flag
	glGetError();

	// Grab the QOpenGLFunctions object pointer
	QOpenGLFunctions* glFunctions = context->functions();

	// Create new instance
	PrimitiveInstance* pi = instances_.add();

	// Vertex buffer object or plain old display list?
	if (PrimitiveInstance::globalInstanceType() == PrimitiveInstance::VBOInstance)
	{
		// Prepare local array of data to pass to VBO
		GLuint vertexVBO = 0, indexVBO = 0;
		if (nDefinedVertices_ <= 0)
		{
			// Store instance data
			pi->setVBO(context, 0, 0);
			return;
		}

		// Determine total size of array (in bytes) for VBO
		int vboSize = nDefinedVertices_ * (colouredVertexData_ ? 10 : 6) * sizeof(GLfloat);
		
		// Generate vertex array object
		glFunctions->glGenBuffers(1, &vertexVBO);

		// Bind VBO
		glFunctions->glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
		
		// Initialise vertex array data
		glFunctions->glBufferData(GL_ARRAY_BUFFER, vboSize, vertexData_.array(), GL_STATIC_DRAW);
		if (glGetError() != GL_NO_ERROR)
		{
			glFunctions->glBindBuffer(GL_ARRAY_BUFFER, 0);
			printf("Error occurred while generating vertex buffer object for Primitive.\n");
			glFunctions->glDeleteBuffers(1, &vertexVBO);
			vertexVBO = 0;
			return;
		}
		glFunctions->glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Generate index array object (if using indices)
		if (indexData_.nItems() != 0)
		{
			// Generate index array object
			glFunctions->glGenBuffers(1, &indexVBO);

			// Bind VBO
			glFunctions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
			
			// Initialise index array data
			glFunctions->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData_.nItems()*sizeof(GLuint), indexData_.array(), GL_STATIC_DRAW);
			if (glGetError() != GL_NO_ERROR)
			{
				glFunctions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				printf("Error occurred while generating index buffer object for Primitive.\n");
				glFunctions->glDeleteBuffers(1, &indexVBO);
				indexVBO = 0;
				return;
			}
			glFunctions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		// Store instance data
		pi->setVBO(context, vertexVBO, indexData_.nItems() != 0 ? indexVBO : 0);
	}
	else
	{
		// Generate display list
		int listId = glGenLists(1);
		if (listId == 0) printf("Internal Error: Primitive::pushInstance - glGenLists(1) returned 0!\n!");
		else
		{
			glNewList(listId, GL_COMPILE);
			
			// Does the vertex data contain colour-per-vertex information?
			glInterleavedArrays(colouredVertexData_ ? GL_C4F_N3F_V3F : GL_N3F_V3F, 0, vertexData_.array());

			// Check if we are using indices
			if (indexData_.nItems()) glDrawElements(type_, indexData_.nItems(), GL_UNSIGNED_INT, indexData_.array());
			else glDrawArrays(type_, 0, nDefinedVertices_);
			
			glEndList();
		}

		// Store data
		pi->setDisplayList(context, listId);
	}
}

// Pop topmost instance on primitive's stack
void Primitive::popInstance(const QOpenGLContext* context)
{
	// Does this primitive use instances?
	if (!useInstances_) return;

	// Grab the QOpenGLFunctions object pointer
	QOpenGLFunctions* glFunctions = context->functions();

	PrimitiveInstance* pi = instances_.last();
	if (pi != NULL)
	{
		if (pi->context() == context)
		{
			// Vertex buffer object or plain old display list?
			if (pi->type() == PrimitiveInstance::VBOInstance)
			{
				GLuint bufid  = pi->vboVertexObject();
				if (bufid != 0) glFunctions->glDeleteBuffers(1, &bufid);
				if (indexData_.nItems() != 0)
				{
					bufid = pi->vboIndexObject();
					if (bufid != 0) glFunctions->glDeleteBuffers(1, &bufid);
				}
			}
			else if (pi->listObject() != 0) glDeleteLists(pi->listObject(),1);
		}
		instances_.removeLast();
	}
}


// Return number of instances available
int Primitive::nInstances()
{
	return instances_.nItems();
}

// Send to OpenGL (i.e. render)
void Primitive::sendToGL()
{
	// If no vertices are defined, nothing to do...
	if (nDefinedVertices_ == 0) return;

	// Check if using instances...
	if (useInstances_)
	{
		// Grab topmost instance
		PrimitiveInstance* pi = instances_.last();
		if (pi == NULL) printf("Internal Error: No instance on stack in primitive %p.\n", this);
		else if (pi->type() == PrimitiveInstance::VBOInstance)
		{
			// Get QOpenGLFunctions object from supplied context
			QOpenGLFunctions* functions = pi->context()->functions();

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			if (indexData_.nItems() != 0) glEnableClientState(GL_INDEX_ARRAY);
			else glDisableClientState(GL_INDEX_ARRAY);

			// Bind VBO and index buffer (if using it)
			functions->glBindBuffer(GL_ARRAY_BUFFER, pi->vboVertexObject());
			if (indexData_.nItems() != 0) functions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pi->vboIndexObject());

			glInterleavedArrays(colouredVertexData_ ? GL_C4F_N3F_V3F : GL_N3F_V3F, 0, NULL);
			if (indexData_.nItems() != 0) glDrawElements(type_, indexData_.nItems(), GL_UNSIGNED_INT, 0);
			else glDrawArrays(type_, 0, nDefinedVertices_);

			// Revert to normal operation - pass 0 as VBO index
			functions->glBindBuffer(GL_ARRAY_BUFFER, 0);
			if (indexData_.nItems() != 0) functions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);
			if (indexData_.nItems() != 0) glDisableClientState(GL_INDEX_ARRAY);
		}
		else if (pi->listObject() != 0) glCallList(pi->listObject());
	}
	else
	{
		// Does the vertex data contain colour-per-vertex information?
		glInterleavedArrays(colouredVertexData_ ? GL_C4F_N3F_V3F : GL_N3F_V3F, 0, vertexData_.array());

		// Check if we are using indices
		if (indexData_.nItems() != 0) glDrawElements(type_, indexData_.nItems(), GL_UNSIGNED_INT, indexData_.array());
		else glDrawArrays(type_, 0, nDefinedVertices_);
	}
}

/*
 * Vertex / Index Generation
 */

GLuint Primitive::defineVertex(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz)
{
	if (colouredVertexData_)
	{
		printf("Internal Error: No colour given to defineVertex(), but the primitive requires one.\n");
		return -1;
	}

	// Store normal
	vertexData_.add(nx);
	vertexData_.add(ny);
	vertexData_.add(nz);

	// Store vertex
	vertexData_.add(x);
	vertexData_.add(y);
	vertexData_.add(z);

	// Increase vertex counter
	++nDefinedVertices_;

	// Return index of vertex
	return (nDefinedVertices_-1);
}

// Define next vertex and normal (as Vec3<double>)
GLuint Primitive::defineVertex(Vec3<double> vertex, Vec3<double> normal)
{
	return defineVertex(vertex.x, vertex.y, vertex.z, normal.x, normal.y, normal.z);
}

// Define next vertex and normal with colour
GLuint Primitive::defineVertex(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	// Store colour
	if (!colouredVertexData_)
	{
		printf("Internal Error: Colour specified in vertex creation, but it is not required for primitive.\n");
		return -1;
	}
	else
	{
		vertexData_.add(r);
		vertexData_.add(g);
		vertexData_.add(b);
		vertexData_.add(a);
	}

	// Store normal
	vertexData_.add(nx);
	vertexData_.add(ny);
	vertexData_.add(nz);

	// Store vertex
	vertexData_.add(x);
	vertexData_.add(y);
	vertexData_.add(z);

	// Increase vertex counter
	++nDefinedVertices_;

	// Return index of vertex
	return (nDefinedVertices_-1);
}

// Define next vertex and normal
GLuint Primitive::defineVertex(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, Vec4<GLfloat>& colour)
{
	return defineVertex(x, y, z, nx, ny, nz, colour.x, colour.y, colour.z, colour.w);
}

// Define next vertex and normal with colour (as array)
GLuint Primitive::defineVertex(GLfloat x, GLfloat y, GLfloat z, Vec3<double>& normal, Vec4<GLfloat>& colour)
{
	return defineVertex(x, y, z, normal.x, normal.y, normal.z, colour.x, colour.y, colour.z, colour.w);
}

// Define next vertex, normal, and colour (as Vec3<double>s and array)
GLuint Primitive::defineVertex(Vec3<double>& v, Vec3<double>& u, Vec4<GLfloat>& colour)
{
	return defineVertex(v.x, v.y, v.z, u.x, u.y, u.z, colour.x, colour.y, colour.z, colour.w);
}

// Define next index double
void Primitive::defineIndices(GLuint a, GLuint b)
{
	indexData_.add(a);
	indexData_.add(b);
}

// Define next index triple
void Primitive::defineIndices(GLuint a, GLuint b, GLuint c)
{
	indexData_.add(a);
	indexData_.add(b);
	indexData_.add(c);
}

/*
 * Geometric Primitive Generation
 */

// Draw line
void Primitive::line(double x1, double y1, double z1, double x2, double y2, double z2)
{
	defineVertex(x1, y1, z1, 1.0, 0.0, 0.0);
	defineVertex(x2, y2, z2, 1.0, 0.0, 0.0);
}

// Add line to axis primitive
void Primitive::line(Vec3<double> v1, Vec3<double> v2)
{
	defineVertex(v1.x, v1.y, v1.z, 1.0, 0.0, 0.0);
	defineVertex(v2.x, v2.y, v2.z, 1.0, 0.0, 0.0);
}
