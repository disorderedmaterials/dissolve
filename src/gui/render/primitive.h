// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/render/primitiveInstance.h"
#include "math/matrix4.h"
#include <QOpenGLFunctions>

// Rendering Primitive
class Primitive
{
    public:
    Primitive();
    ~Primitive();

    /*
     * Data
     */
    private:
    // Vertex data array
    std::vector<GLfloat> vertexData_;
    // Number of vertices defined in vertexData_
    int nDefinedVertices_;
    // Index data array
    std::vector<GLuint> indexData_;
    // GL primitive type (GL_TRIANGLES, GL_LINES etc.)
    GLenum type_;
    // Number of vertices per primitive type
    int verticesPerType_;
    // Number of data points per vertex
    int dataPerVertex_;
    // Whether vertex data array also contains colour information
    bool colouredVertexData_;

    public:
    // Initialise primitive storage
    void initialise(GLenum type, bool colourData, int arrayChunkSize = -1);
    // Forget all data, leaving arrays intact
    void forgetAll();
    // Return number of vertices currently defined in primitive
    int nDefinedVertices() const;
    // Return number of indices currently defined in primitive
    int nDefinedIndices() const;
    // Return whether vertex data contains colour information
    bool colouredVertexData() const;
    // Update mesh (recreate instance / VBO / display list) of primitive
    void updateMesh();

    /*
     * Instances
     */
    private:
    // Stack of OpenGL VBO or display list IDs and the contexts in which they were created
    std::vector<std::unique_ptr<PrimitiveInstance>> instances_;
    // Flag stating whether or not instances should be used for this primitive
    bool useInstances_;

    public:
    // Flag that this primitive should not use instances (rendering will use vertex arrays)
    void setNoInstances();
    // Push instance layer from current vertex chunk list
    void pushInstance(const QOpenGLContext *context);
    // Pop topmost instance layer
    void popInstance(const QOpenGLContext *context);
    // Return number of instances available
    int nInstances() const;
    // Send to OpenGL (i.e. render)
    void sendToGL() const;

    /*
     * Vertex / Index Generation
     */
    public:
    // Define next vertex and normal
    GLuint defineVertex(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, const GLfloat *rgba = nullptr);
    // Define next vertex and normal
    GLuint defineVertex(GLfloat x, GLfloat y, GLfloat z, Vec3<double> &normal, const GLfloat *rgba = nullptr);
    // Define next vertex and normal
    GLuint defineVertex(Vec3<double> &vertex, Vec3<double> &normal, const GLfloat *rgba = nullptr);
    // Define next index double
    void defineIndices(GLuint a, GLuint b);
    // Define next index triple
    void defineIndices(GLuint a, GLuint b, GLuint c);

    /*
     * Geometric Primitive Generation
     */
    public:
    // Draw line
    void line(double x1, double y1, double z1, double x2, double y2, double z2, const GLfloat *rgba = nullptr);
    // Draw line
    void line(Vec3<double> v1, Vec3<double> v2, const GLfloat *rgba = nullptr);
    // Create vertices of sphere with specified radius and quality
    void sphere(double radius, int nstacks, int nslices);
    // Plot cylinder vertices from origin {ox,oy,oz}, following vector {vx,vy,vz}, with radii and quality specified
    void cylinder(GLfloat ox, GLfloat oy, GLfloat oz, GLfloat vx, GLfloat vy, GLfloat vz, double startRadius, double endRadius,
                  int nStacks, int nSlices, bool capStart = false, bool capEnd = false);
    // Plot tube ring of specified radius and tube width
    void ring(double radius, double width, int nStacks, int nSlices, int nSegments, bool segmented);
    // Plot circle of specified radius
    void circle(double radius, int nStacks, int nSegments, bool segmented);
    // Create vertices of cross with specified width
    void cross(double halfWidth);
    // Plot solid orthorhomboid of specified size at specified origin, and with sides subdivided into triangles ( ntriangles
    // = 2*nSubs )
    void orthorhomboid(double sizex = 1.0, double sizey = 1.0, double sizez = 1.0, int nSubs = 4, double ox = 0.0,
                       double oy = 0.0, double oz = 0.0);
    // Plot wireframe orthorhomboid of specified size at specified origin, and with sides subdivided into triangles (
    // ntriangles = 2*nSubs )
    void wireOrthorhomboid(double sizex = 1.0, double sizey = 1.0, double sizez = 1.0, double ox = 0.0, double oy = 0.0,
                           double oz = 0.0);
    // Plot basic axes object
    void axes(double axisLength);
    // Plot basic coloured axes object
    void colouredAxes(double axisLength);
};
