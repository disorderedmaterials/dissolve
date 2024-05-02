// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/primitiveInstance.h"

// Static members
PrimitiveInstance::InstanceType PrimitiveInstance::globalInstanceType_ = PrimitiveInstance::VBOInstance;

PrimitiveInstance::PrimitiveInstance()
{
    // Private variables
    context_ = nullptr;
    type_ = PrimitiveInstance::ListInstance;
    listObject_ = 0;
    vboVertexObject_ = 0;
    vboIndexObject_ = 0;
}

// Return global instance type to use
PrimitiveInstance::InstanceType PrimitiveInstance::globalInstanceType() { return globalInstanceType_; }

// Set global instance type to use
void PrimitiveInstance::setGlobalInstanceType(PrimitiveInstance::InstanceType instanceType)
{
    globalInstanceType_ = instanceType;
}

// Return context to which primitive instance is associated
const QOpenGLContext *PrimitiveInstance::context() { return context_; }

// Return type of instance
PrimitiveInstance::InstanceType PrimitiveInstance::type() const { return type_; }

// Set display list data
void PrimitiveInstance::setDisplayList(const QOpenGLContext *context, GLuint listObject)
{
    context_ = context;
    type_ = PrimitiveInstance::ListInstance;
    listObject_ = listObject;
}

// Set vbo object data
void PrimitiveInstance::setVBO(const QOpenGLContext *context, GLuint vertexObject, GLuint indexObject)
{
    context_ = context;
    type_ = PrimitiveInstance::VBOInstance;
    vboVertexObject_ = vertexObject;
    vboIndexObject_ = indexObject;
}

// Return display list object for instance
GLuint PrimitiveInstance::listObject() const { return listObject_; }

// Return VBO ID of vertex array for instance
GLuint PrimitiveInstance::vboVertexObject() const { return vboVertexObject_; }

// Return VBO ID of index array for instance
GLuint PrimitiveInstance::vboIndexObject() const { return vboIndexObject_; }
