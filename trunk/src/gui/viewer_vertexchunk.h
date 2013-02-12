/*
	*** Vertex Chunk Storage
	*** src/gui/viewer_vertexchunk.h
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

#ifndef DUQ_VERTEXCHUNK_H
#define DUQ_VERTEXCHUNK_H

#ifdef _WIN32
#include <windows.h>
#endif
#ifdef _MAC
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include "math/constants.h"

#define VERTEXCHUNKSIZE 1020

// Chunk of triangles
class VertexChunk
{
	public:
	// Constructor / Destructor
	VertexChunk();
	~VertexChunk();
	// List pointers
	VertexChunk* prev, *next;
	
	private:
	// Vertex data array (containing normal and possibly colour data)
	GLfloat* vertexData_;
	// Centroid array
	GLfloat* centroids_;
	// Number of data points per vertex (NR=6, CNR=10)
	int dataPerVertex_;
	// Number of defined vertices in current chunk
	int nDefinedVertices_;
	// NUmber of primitive types (nDefinedVertices/verticesPerType) currently defined
	int nDefinedTypes_;
	// Maximum number of allowable vertices
	int maxVertices_;
	// Primitive type (GL)
	GLenum type_;
	// Number of vertices per primitive type
	int verticesPerType_;
	
	private:
	// Update (and finalise) centroid for current primitive type
	void updateCentroid(GLfloat x, GLfloat y, GLfloat z, bool finalise);
	
	public:
	// Initialise structure
	void initialise(GLenum type, bool colourData);
	// Forget all vertex data currently stored in array (but retain array)
	void forgetAll();
	// Define next vertex and normal
	void defineVertex(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, bool calcCentroid = TRUE);
	// Define next vertex, normal, and colour (as array)
	void defineVertex(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat* colour, bool calcCentroid = TRUE);
	// Define next vertex, normal, and colour
	void defineVertex(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat r, GLfloat g, GLfloat b, GLfloat a, bool calcCentroid = TRUE);
	// Return whether current array is full
	bool full();
	// Return number of defined primitive (GL) types
	int nDefinedTypes();
	// Return vertex array
	GLfloat* vertexData();
	// Return centroid array
	GLfloat* centroids();
	// Return number of defined vertices in chunk
	int nDefinedVertices();
	// Send to OpenGL (i.e. render)
	void sendToGL();
};

#endif
