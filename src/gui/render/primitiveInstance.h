// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QtGui/qopengl.h>

// Forward Declarations
class QOpenGLContext;
class GLExtensions;

// Primitive Instance
class PrimitiveInstance
{
    public:
    PrimitiveInstance();
    // Instance Type
    enum InstanceType
    {
        NoInstances,
        ListInstance,
        VBOInstance
    };

    private:
    // Global instance type to use
    static PrimitiveInstance::InstanceType globalInstanceType_;
    // Context to which primitive instance is associated
    const QOpenGLContext *context_;
    // GL extension function pointers for this context
    GLExtensions *extensions_;
    // Type of instance
    InstanceType type_;
    // List ID of instance (if using display lists)
    GLuint listObject_;
    // VBO ID of vertex array (if using VBOs)
    GLuint vboVertexObject_;
    // VBO ID of index array (if using indexed VBOs)
    GLuint vboIndexObject_;

    public:
    // Return global instance type to use
    static PrimitiveInstance::InstanceType globalInstanceType();
    // Set global instance type to use
    static void setGlobalInstanceType(PrimitiveInstance::InstanceType instanceType);
    // Return context to which primitive instance is associated
    const QOpenGLContext *context();
    // Return GL extensions
    const GLExtensions *extensions() const;
    // Set display list data
    void setDisplayList(const QOpenGLContext *context, GLuint listObject);
    // Set vbo object data
    void setVBO(const QOpenGLContext *context, GLuint vertexObject, GLuint indexObject);
    // Return type of instance
    InstanceType type() const;
    // Return display list object for instance
    GLuint listObject() const;
    // Return VBO ID of vertex array for instance
    GLuint vboVertexObject() const;
    // Return VBO ID of index array for instance
    GLuint vboIndexObject() const;
};
