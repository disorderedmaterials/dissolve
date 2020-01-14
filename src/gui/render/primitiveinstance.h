/*
	*** Rendering Primitive Instance
	*** src/gui/render/primitiveinstance.h
	Copyright T. Youngs 2013-2020

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

#ifndef DISSOLVE_RENDER_PRIMITIVEINSTANCE_H
#define DISSOLVE_RENDER_PRIMITIVEINSTANCE_H

#include <QtGui/qopengl.h>
#include "templates/listitem.h"

// Forward Declarations
class QOpenGLContext;
class GLExtensions;

// Primitive Instance
class PrimitiveInstance : public ListItem<PrimitiveInstance>
{
	public:
	// Constructor
	PrimitiveInstance();
	// Instance Type
	enum InstanceType { NoInstances, ListInstance, VBOInstance };
	
	private:
	// Global instance type to use
	static PrimitiveInstance::InstanceType globalInstanceType_;
	// Context to which primitive instance is associated
	const QOpenGLContext* context_;
	// GL extension function pointers for this context
	GLExtensions* extensions_;
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
	const QOpenGLContext* context();
	// Return GL extensions
	const GLExtensions* extensions() const;
	// Set display list data
	void setDisplayList(const QOpenGLContext* context, GLuint listObject);
	// Set vbo object data
	void setVBO(const QOpenGLContext* context, GLuint vertexObject, GLuint indexObject);
	// Return type of instance
	InstanceType type() const;
	// Return display list object for instance
	GLuint listObject() const;
	// Return VBO ID of vertex array for instance
	GLuint vboVertexObject() const;
	// Return VBO ID of index array for instance
	GLuint vboIndexObject() const;
};

#endif
