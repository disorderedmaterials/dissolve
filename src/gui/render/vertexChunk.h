// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "math/constants.h"
#include <QtGui/qopengl.h>

#define VERTEXCHUNKSIZE 1020

// Chunk of triangles
class VertexChunk
{
    public:
    VertexChunk();
    ~VertexChunk();
    // List pointers
    VertexChunk *prev, *next;

    private:
    // Vertex data array (containing normal and possibly colour data)
    GLfloat *vertexData_;
    // Centroid array
    GLfloat *centroids_;
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
    void defineVertex(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, bool calcCentroid = true);
    // Define next vertex, normal, and colour (as array)
    void defineVertex(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat *colour,
                      bool calcCentroid = true);
    // Define next vertex, normal, and colour
    void defineVertex(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat r, GLfloat g, GLfloat b,
                      GLfloat a, bool calcCentroid = true);
    // Return whether current array is full
    bool full();
    // Return number of defined primitive (GL) types
    int nDefinedTypes();
    // Return vertex array
    GLfloat *vertexData();
    // Return centroid array
    GLfloat *centroids();
    // Return number of defined vertices in chunk
    int nDefinedVertices();
    // Send to OpenGL (i.e. render)
    void sendToGL();
};
