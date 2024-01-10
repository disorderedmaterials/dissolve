// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/vertexChunk.h"
#include "base/messenger.h"

VertexChunk::VertexChunk()
{
    // Public variables
    next = nullptr;
    prev = nullptr;

    // Private variables
    vertexData_ = nullptr;
    centroids_ = nullptr;
    verticesPerType_ = 0;
    dataPerVertex_ = 0;
    nDefinedVertices_ = 0;
    maxVertices_ = -1;
    nDefinedTypes_ = 0;
    type_ = GL_TRIANGLES;
}

VertexChunk::~VertexChunk()
{
    if (vertexData_ != nullptr)
        delete[] vertexData_;
    if (centroids_ != nullptr)
        delete[] centroids_;
}

// Update (or finalise) centroid for current primitive type
void VertexChunk::updateCentroid(GLfloat x, GLfloat y, GLfloat z, bool finalise)
{
    // Accumulate centroid
    auto coff = nDefinedTypes_ * 3;
    centroids_[coff] += x;
    centroids_[coff + 1] += y;
    centroids_[coff + 2] += z;
    // Finalise centroid?
    if (finalise)
    {
        centroids_[coff] /= verticesPerType_;
        centroids_[coff + 1] /= verticesPerType_;
        centroids_[coff + 2] /= verticesPerType_;
    }
}

// Initialise structure
void VertexChunk::initialise(GLenum type, bool colourData)
{
    type_ = type;
    dataPerVertex_ = (colourData ? 10 : 6);
    if (type_ == GL_TRIANGLES)
        verticesPerType_ = 3;
    else if (type_ == GL_LINES)
        verticesPerType_ = 2;
    else if (type_ == GL_POINTS)
        verticesPerType_ = 1;
    else
        Messenger::error("Invalid GLenum type given to VertexChunk::initialise ({})\n", type_);
    maxVertices_ = VERTEXCHUNKSIZE * verticesPerType_;
    nDefinedVertices_ = 0;
    nDefinedTypes_ = 0;
    vertexData_ = new GLfloat[maxVertices_ * dataPerVertex_];
    centroids_ = new GLfloat[VERTEXCHUNKSIZE * 3];
    for (auto n = 0; n < VERTEXCHUNKSIZE * 3; ++n)
        centroids_[n] = 0.0f;
}

// Define next vertex and normal
void VertexChunk::defineVertex(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, bool calcCentroid)
{
    if (nDefinedVertices_ == maxVertices_)
        Messenger::error("Vertex limit for VertexChunk reached.\n");
    auto index = nDefinedVertices_ * dataPerVertex_;
    if (dataPerVertex_ == 10)
    {
        Messenger::error("No colour specified in vertex creation, but the primitive requires one.\n");
        index += 4;
    }
    // Store normal
    vertexData_[index++] = nx;
    vertexData_[index++] = ny;
    vertexData_[index++] = nz;
    // Store vertex
    vertexData_[index++] = x;
    vertexData_[index++] = y;
    vertexData_[index++] = z;
    // Increase vertex counter
    ++nDefinedVertices_;
    // Update centroid
    auto finalise = (nDefinedVertices_ % verticesPerType_) == 0;
    if (calcCentroid)
        updateCentroid(x, y, z, finalise);
    if (finalise)
        ++nDefinedTypes_;
}

// Define next vertex and normal with specific colour (as array)
void VertexChunk::defineVertex(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat *colour,
                               bool calcCentroid)
{
    if (nDefinedVertices_ == maxVertices_)
        Messenger::error("Vertex limit for VertexChunk reached.\n");
    auto index = nDefinedVertices_ * dataPerVertex_;
    // Store colour
    if (dataPerVertex_ != 10)
        Messenger::error("Colour specified in vertex creation, but it is not required for primitive.\n");
    else
    {
        vertexData_[index++] = colour[0];
        vertexData_[index++] = colour[1];
        vertexData_[index++] = colour[2];
        vertexData_[index++] = colour[3];
    }
    // Store normal
    vertexData_[index++] = nx;
    vertexData_[index++] = ny;
    vertexData_[index++] = nz;
    // Store vertex
    vertexData_[index++] = x;
    vertexData_[index++] = y;
    vertexData_[index++] = z;
    // Increase vertex counter
    ++nDefinedVertices_;
    // Update centroid
    auto finalise = (nDefinedVertices_ % verticesPerType_) == 0;
    if (calcCentroid)
        updateCentroid(x, y, z, finalise);
    if (finalise)
        ++nDefinedTypes_;
}

// Define next vertex and normal with specific colour
void VertexChunk::defineVertex(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat r, GLfloat g,
                               GLfloat b, GLfloat a, bool calcCentroid)
{
    if (nDefinedVertices_ == maxVertices_)
        Messenger::error("Vertex limit for VertexChunk reached.\n");
    auto index = nDefinedVertices_ * dataPerVertex_;
    // Store colour
    if (dataPerVertex_ != 10)
        Messenger::error("Colour specified in vertex creation, but it is not required for primitive.\n");
    else
    {
        vertexData_[index++] = r;
        vertexData_[index++] = g;
        vertexData_[index++] = b;
        vertexData_[index++] = a;
    }
    // Store normal
    vertexData_[index++] = nx;
    vertexData_[index++] = ny;
    vertexData_[index++] = nz;
    // Store vertex
    vertexData_[index++] = x;
    vertexData_[index++] = y;
    vertexData_[index++] = z;
    // Increase vertex counter
    ++nDefinedVertices_;
    // Update centroid
    auto finalise = (nDefinedVertices_ % verticesPerType_) == 0;
    if (calcCentroid)
        updateCentroid(x, y, z, finalise);
    if (finalise)
        ++nDefinedTypes_;
}

// Return whether current array is full
bool VertexChunk::full() { return (nDefinedVertices_ == maxVertices_); }

// Forget all vertex data currently stored in array (but retain array)
void VertexChunk::forgetAll()
{
    nDefinedTypes_ = 0;
    nDefinedVertices_ = 0;
    for (auto n = 0; n < VERTEXCHUNKSIZE * 3; ++n)
        centroids_[n] = 0.0f;
}

// Return number of defined primitive (GL) types
int VertexChunk::nDefinedTypes() { return nDefinedTypes_; }

// Return vertex array
GLfloat *VertexChunk::vertexData() { return vertexData_; }

// Return centroid array
GLfloat *VertexChunk::centroids() { return centroids_; }

// Return number of defined vertices in chunk
int VertexChunk::nDefinedVertices() { return nDefinedVertices_; }

// Send to OpenGL (i.e. render)
void VertexChunk::sendToGL()
{
    if (nDefinedVertices_ == 0)
        return;
    // Does the vertex data contain colour-per-vertex information?
    glInterleavedArrays(dataPerVertex_ == 10 ? GL_C4F_N3F_V3F : GL_N3F_V3F, 0, vertexData_);
    glDrawArrays(type_, 0, nDefinedVertices_);
}
