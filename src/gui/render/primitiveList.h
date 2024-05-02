// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/render/primitive.h"
#include <vector>

// Forward Declarations
class QOpenGLContext;

// Rendering Primitive List
class PrimitiveList
{
    public:
    PrimitiveList();
    ~PrimitiveList();

    /*
     * Data
     */
    private:
    // List of Primitives owned and managed by this list
    std::vector<std::unique_ptr<Primitive>> primitives_;

    public:
    // Clear all existing Primitives
    void clear();
    // Forget all data, leaving arrays intact
    void forgetAll();
    // Reinitialise list so it is large enough to accomodate specified number of Primitives
    void reinitialise(int newSize, GLenum type, bool colourData);
    // Add a new Primitive to the end of the list
    Primitive *add(GLenum type = GL_LINES, bool colourData = false);
    // Register an existing Primitive with the list
    void add(Primitive *primitive);
    // Return number of primitives in the list
    int nPrimitives() const;
    // Remove specified Primitive
    void remove(Primitive *primitive);
    // Return total number of defined vertices
    int nDefinedVertices();
    // Return total number of defined indices
    int nDefinedIndices();
    // Push instance layer
    void pushInstance(const QOpenGLContext *context);
    // Pop topmost instance layer
    void popInstance(const QOpenGLContext *context);
    // Return number of instances of topmost primitive
    int nInstances();
    // Send to OpenGL (i.e. render)
    void sendToGL();

    /*
     * Operators
     */
    public:
    Primitive *operator[](int index);
};
