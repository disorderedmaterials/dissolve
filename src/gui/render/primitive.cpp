// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/primitive.h"
#include "base/messenger.h"

Primitive::Primitive()
{
    colouredVertexData_ = false;
    type_ = GL_TRIANGLES;
    dataPerVertex_ = 6;
    nDefinedVertices_ = 0;
    useInstances_ = true;
}

Primitive::~Primitive() {}

/*
 * Data
 */

// Initialise primitive
void Primitive::initialise(GLenum type, bool colourData, int arrayChunkSize)
{
    type_ = type;
    colouredVertexData_ = colourData;

    // Set data per vertex based on the primitive type, and whether we have individual colour data or not
    dataPerVertex_ = (colouredVertexData_ ? 10 : 6);
    if (type_ == GL_TRIANGLES)
        verticesPerType_ = 3;
    else if ((type_ == GL_LINES) || (type_ == GL_LINE_LOOP) || (type_ == GL_LINE_STRIP))
        verticesPerType_ = 2;
    else if (type_ == GL_POINTS)
        verticesPerType_ = 1;
    else
        Messenger::error("Invalid GLenum type given to VertexChunk::initialise ({})\n", type_);

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
int Primitive::nDefinedVertices() const { return vertexData_.size(); }

// Return number of indices currently defined in primitive
int Primitive::nDefinedIndices() const { return indexData_.size(); }

// Return whether vertex data contains colour information
bool Primitive::colouredVertexData() const { return colouredVertexData_; }

// Update mesh (VBO / display list) of dynamic primitive
void Primitive::updateMesh()
{
    // Check instances - if there is no current instance, create one
    if (!instances_.empty())
        popInstance(QOpenGLContext::currentContext());
    pushInstance(QOpenGLContext::currentContext());
}
/*
 * Instances
 */

// Flag that this primitive should not use instances (rendering will use vertex arrays)
void Primitive::setNoInstances() { useInstances_ = false; }

// Push instance of primitive
void Primitive::pushInstance(const QOpenGLContext *context)
{
    // Does this primitive use instances?
    if (!useInstances_)
        return;

    // Clear the error flag
    glGetError();

    // Grab the QOpenGLFunctions object pointer
    QOpenGLFunctions *glFunctions = context->functions();

    // Create new instance
    PrimitiveInstance *pi = instances_.emplace_back(std::make_unique<PrimitiveInstance>()).get();

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
        auto vboSize = nDefinedVertices_ * (colouredVertexData_ ? 10 : 6) * sizeof(GLfloat);

        // Generate vertex array object
        glFunctions->glGenBuffers(1, &vertexVBO);

        // Bind VBO
        glFunctions->glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);

        // Initialise vertex array data
        glFunctions->glBufferData(GL_ARRAY_BUFFER, vboSize, vertexData_.data(), GL_STATIC_DRAW);
        if (glGetError() != GL_NO_ERROR)
        {
            glFunctions->glBindBuffer(GL_ARRAY_BUFFER, 0);
            Messenger::error("Error occurred while generating vertex buffer object for Primitive.\n");
            glFunctions->glDeleteBuffers(1, &vertexVBO);
            vertexVBO = 0;
            return;
        }
        glFunctions->glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Generate index array object (if using indices)
        if (!indexData_.empty())
        {
            // Generate index array object
            glFunctions->glGenBuffers(1, &indexVBO);

            // Bind VBO
            glFunctions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);

            // Initialise index array data
            glFunctions->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData_.size() * sizeof(GLuint), indexData_.data(),
                                      GL_STATIC_DRAW);
            if (glGetError() != GL_NO_ERROR)
            {
                glFunctions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                Messenger::error("Error occurred while generating index buffer object for Primitive.\n");
                glFunctions->glDeleteBuffers(1, &indexVBO);
                indexVBO = 0;
                return;
            }
            glFunctions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        // Store instance data
        pi->setVBO(context, vertexVBO, indexData_.empty() ? 0 : indexVBO);
    }
    else
    {
        // Generate display list
        auto listId = glGenLists(1);
        if (listId == 0)
            Messenger::error("Internal Error: Primitive::pushInstance - glGenLists(1) returned 0!\n!");
        else
        {
            glNewList(listId, GL_COMPILE);

            // Does the vertex data contain colour-per-vertex information?
            glInterleavedArrays(colouredVertexData_ ? GL_C4F_N3F_V3F : GL_N3F_V3F, 0, vertexData_.data());

            // Check if we are using indices
            if (!indexData_.empty())
                glDrawElements(type_, indexData_.size(), GL_UNSIGNED_INT, indexData_.data());
            else
                glDrawArrays(type_, 0, nDefinedVertices_);

            glEndList();
        }

        // Store data
        pi->setDisplayList(context, listId);
    }
}

// Pop topmost instance on primitive's stack
void Primitive::popInstance(const QOpenGLContext *context)
{
    // Does this primitive use instances?
    if (!useInstances_)
        return;

    // Grab the QOpenGLFunctions object pointer
    QOpenGLFunctions *glFunctions = context->functions();

    auto &pi = instances_.back();
    if (pi != nullptr)
    {
        if (pi->context() == context)
        {
            // Vertex buffer object or plain old display list?
            if (pi->type() == PrimitiveInstance::VBOInstance)
            {
                GLuint bufid = pi->vboVertexObject();
                if (bufid != 0)
                    glFunctions->glDeleteBuffers(1, &bufid);
                if (!indexData_.empty())
                {
                    bufid = pi->vboIndexObject();
                    if (bufid != 0)
                        glFunctions->glDeleteBuffers(1, &bufid);
                }
            }
            else if (pi->listObject() != 0)
                glDeleteLists(pi->listObject(), 1);
        }
        instances_.pop_back();
    }
}

// Return number of instances available
int Primitive::nInstances() const { return instances_.size(); }

// Send to OpenGL (i.e. render)
void Primitive::sendToGL() const
{
    // If no vertices are defined, nothing to do...
    if (nDefinedVertices_ == 0)
        return;

    // Check if using instances...
    if (useInstances_)
    {
        // Grab topmost instance
        auto &pi = instances_.back();
        if (pi->type() == PrimitiveInstance::VBOInstance)
        {
            // Get QOpenGLFunctions object from supplied context
            QOpenGLFunctions *functions = pi->context()->functions();

            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_NORMAL_ARRAY);
            glEnableClientState(GL_COLOR_ARRAY);
            if (!indexData_.empty())
                glEnableClientState(GL_INDEX_ARRAY);
            else
                glDisableClientState(GL_INDEX_ARRAY);

            // Bind VBO and index buffer (if using it)
            functions->glBindBuffer(GL_ARRAY_BUFFER, pi->vboVertexObject());
            if (!indexData_.empty())
                functions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pi->vboIndexObject());

            glInterleavedArrays(colouredVertexData_ ? GL_C4F_N3F_V3F : GL_N3F_V3F, 0, nullptr);
            if (!indexData_.empty())
                glDrawElements(type_, indexData_.size(), GL_UNSIGNED_INT, 0);
            else
                glDrawArrays(type_, 0, nDefinedVertices_);

            // Revert to normal operation - pass 0 as VBO index
            functions->glBindBuffer(GL_ARRAY_BUFFER, 0);
            if (!indexData_.empty())
                functions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);
            glDisableClientState(GL_COLOR_ARRAY);
            if (!indexData_.empty())
                glDisableClientState(GL_INDEX_ARRAY);
        }
        else if (pi->listObject() != 0)
            glCallList(pi->listObject());
    }
    else
    {
        // Does the vertex data contain colour-per-vertex information?
        glInterleavedArrays(colouredVertexData_ ? GL_C4F_N3F_V3F : GL_N3F_V3F, 0, vertexData_.data());

        // Check if we are using indices
        if (!indexData_.empty())
            glDrawElements(type_, indexData_.size(), GL_UNSIGNED_INT, indexData_.data());
        else
            glDrawArrays(type_, 0, nDefinedVertices_);
    }
}

/*
 * Vertex / Index Generation
 */

// Define next vertex and normal
GLuint Primitive::defineVertex(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, const GLfloat *rgba)
{
    if (colouredVertexData_)
    {
        if (rgba == nullptr)
        {
            Messenger::error("Internal Error: No colour given to defineVertex(), but the Primitive requires one.\n");
            return -1;
        }

        // Store colour
        vertexData_.push_back(rgba[0]);
        vertexData_.push_back(rgba[1]);
        vertexData_.push_back(rgba[2]);
        vertexData_.push_back(rgba[3]);
    }
    else if (rgba != nullptr)
    {
        Messenger::error("Internal Error: Colour given to defineVertex(), but the Primitive does not require one.\n");
        return -1;
    }

    // Store normal
    vertexData_.push_back(nx);
    vertexData_.push_back(ny);
    vertexData_.push_back(nz);

    // Store vertex
    vertexData_.push_back(x);
    vertexData_.push_back(y);
    vertexData_.push_back(z);

    // Increase vertex counter
    ++nDefinedVertices_;

    // Return index of vertex
    return (nDefinedVertices_ - 1);
}

// Define next vertex and normal
GLuint Primitive::defineVertex(GLfloat x, GLfloat y, GLfloat z, Vec3<double> &normal, const GLfloat *rgba)
{
    return defineVertex(x, y, z, normal.x, normal.y, normal.z, rgba);
}

// Define next vertex and normal
GLuint Primitive::defineVertex(Vec3<double> &vertex, Vec3<double> &normal, const GLfloat *rgba)
{
    return defineVertex(vertex.x, vertex.y, vertex.z, normal.x, normal.y, normal.z, rgba);
}

// Define next index double
void Primitive::defineIndices(GLuint a, GLuint b)
{
    indexData_.push_back(a);
    indexData_.push_back(b);
}

// Define next index triple
void Primitive::defineIndices(GLuint a, GLuint b, GLuint c)
{
    indexData_.push_back(a);
    indexData_.push_back(b);
    indexData_.push_back(c);
}

/*
 * Geometric Primitive Generation
 */

// Draw line
void Primitive::line(double x1, double y1, double z1, double x2, double y2, double z2, const GLfloat *rgba)
{
    defineVertex(x1, y1, z1, 1.0, 0.0, 0.0, rgba);
    defineVertex(x2, y2, z2, 1.0, 0.0, 0.0, rgba);
}

// Add line to axis primitive
void Primitive::line(Vec3<double> v1, Vec3<double> v2, const GLfloat *rgba)
{
    defineVertex(v1.x, v1.y, v1.z, 1.0, 0.0, 0.0, rgba);
    defineVertex(v2.x, v2.y, v2.z, 1.0, 0.0, 0.0, rgba);
}

// Create vertices of sphere with specified radius and quality
void Primitive::sphere(double radius, int nStacks, int nSlices)
{
    int i, j;
    double stack0, stack1, z0, zr0, z1, zr1, slice0, slice1, x0, y0, x1, y1;

    for (i = 1; i <= nStacks; ++i)
    {
        stack0 = PI * (-0.5 + (double)(i - 1) / nStacks);
        z0 = sin(stack0);
        zr0 = cos(stack0);

        stack1 = PI * (-0.5 + (double)i / nStacks);
        z1 = sin(stack1);
        zr1 = cos(stack1);

        for (j = 1; j <= nSlices; ++j)
        {
            slice0 = 2 * PI * (double)(j - 1) / nSlices;
            x0 = cos(slice0);
            y0 = sin(slice0);

            slice1 = 2 * PI * (double)j / nSlices;
            x1 = cos(slice1);
            y1 = sin(slice1);

            // First triangle - {x0,y0,z0},{x0,y0,z1},{x1,y1,z0}
            // N.B Don't plot if i == 1, to avoid overlapping with subsequent vertices in this pass
            if (i > 1)
            {
                defineVertex(x0 * zr0 * radius, y0 * zr0 * radius, z0 * radius, x0 * zr0, y0 * zr0, z0);
                defineVertex(x0 * zr1 * radius, y0 * zr1 * radius, z1 * radius, x0 * zr1, y0 * zr1, z1);
                defineVertex(x1 * zr0 * radius, y1 * zr0 * radius, z0 * radius, x1 * zr0, y1 * zr0, z0);
            }

            // Second triangle - {x0,y0,z0},{x0,y0,z1},{x1,y1,z0}
            // N.B. Don't plot if i == nStacks, to avoid overlapping with previous vertices in this pass
            if (i < nStacks)
            {
                defineVertex(x0 * zr1 * radius, y0 * zr1 * radius, z1 * radius, x0 * zr1, y0 * zr1, z1);
                defineVertex(x1 * zr0 * radius, y1 * zr0 * radius, z0 * radius, x1 * zr0, y1 * zr0, z0);
                defineVertex(x1 * zr1 * radius, y1 * zr1 * radius, z1 * radius, x1 * zr1, y1 * zr1, z1);
            }
        }
    }
}

// Plot cylinder vertices from origin {ox,oy,oz}, following vector {vx,vy,vz}, with radii and quality specified
void Primitive::cylinder(GLfloat ox, GLfloat oy, GLfloat oz, GLfloat vx, GLfloat vy, GLfloat vz, double startRadius,
                         double endRadius, int nStacks, int nSlices, bool capStart, bool capEnd)
{
    int i, j;
    Vec3<GLfloat> u, v, w, vert[4], normal[2], deltarj, rj;
    double d, dtheta, dradius;

    // Setup some variables
    rj.set(vx, vy, vz);
    dtheta = TWOPI / nSlices;
    dradius = (startRadius - endRadius) / nStacks;
    deltarj = rj / nStacks;

    // Calculate orthogonal vectors
    u = rj.orthogonal();
    v = rj * u;
    v.normalise();
    w = rj;
    w.normalise();

    // TODO Normal calculation for cones will be incorrect
    for (i = 1; i <= nStacks; ++i)
    {
        for (j = 1; j <= nSlices; ++j)
        {
            d = (j - 1) * dtheta;
            normal[0] = u * cos(d) + v * sin(d);
            vert[0] = normal[0] * (startRadius - (i - 1) * dradius) + deltarj * (i - 1);
            vert[1] = normal[0] * (startRadius - i * dradius) + deltarj * i;
            d = j * dtheta;
            normal[1] = u * cos(d) + v * sin(d);
            vert[2] = normal[1] * (startRadius - (i - 1) * dradius) + deltarj * (i - 1);
            vert[3] = normal[1] * (startRadius - i * dradius) + deltarj * i;

            // Triangle 1
            if ((i > 1) || (startRadius > 1.0e-5))
            {
                defineVertex(ox + vert[0].x, oy + vert[0].y, oz + vert[0].z, normal[0].x, normal[0].y, normal[0].z);
                defineVertex(ox + vert[1].x, oy + vert[1].y, oz + vert[1].z, normal[0].x, normal[0].y, normal[0].z);
                defineVertex(ox + vert[2].x, oy + vert[2].y, oz + vert[2].z, normal[1].x, normal[1].y, normal[1].z);
            }

            // Triangle 2
            if ((i < nStacks) || (endRadius > 1.0e-5))
            {
                defineVertex(ox + vert[1].x, oy + vert[1].y, oz + vert[1].z, normal[0].x, normal[0].y, normal[0].z);
                defineVertex(ox + vert[2].x, oy + vert[2].y, oz + vert[2].z, normal[1].x, normal[1].y, normal[1].z);
                defineVertex(ox + vert[3].x, oy + vert[3].y, oz + vert[3].z, normal[1].x, normal[1].y, normal[1].z);
            }

            // Start cap
            if ((i == 1) && (startRadius > 1.0e-5) && capStart)
            {
                defineVertex(ox, oy, oz, -w.x, -w.y, -w.z);
                defineVertex(ox + vert[0].x, oy + vert[0].y, oz + vert[0].z, -w.x, -w.y, -w.z);
                defineVertex(ox + vert[2].x, oy + vert[2].y, oz + vert[2].z, -w.x, -w.y, -w.z);
            }

            // End cap
            if ((i == nStacks) && (endRadius > 1.0e-5) && capEnd)
            {
                defineVertex(ox + rj.x, oy + rj.y, oz + rj.z, w.x, w.y, w.z);
                defineVertex(ox + vert[1].x, oy + vert[1].y, oz + vert[1].z, w.x, w.y, w.z);
                defineVertex(ox + vert[3].x, oy + vert[3].y, oz + vert[3].z, w.x, w.y, w.z);
            }
        }
    }
}

// Plot tube ring of specified radius and tube width
void Primitive::ring(double radius, double width, int nStacks, int nSlices, int nSegments, bool segmented)
{
    int n, m, o;
    Vec3<GLfloat> x1, x2, y(0.0, 0.0, 1.0), normal[4], vert[4], r1, r2;
    double d1, d2, dtheta, dphi, dpsi, cosphi1, sinphi1, cosphi2, sinphi2;

    // Setup some variables
    dphi = TWOPI / nStacks;
    dpsi = dphi / nSegments;
    dtheta = TWOPI / nSlices;

    for (n = 0; n < nStacks; ++n)
    {
        // Calculate position around circle and orthogonal vectors (for cylinder plotting)
        if (segmented && (n + 1) % 2)
            continue;

        for (o = 0; o < nSegments; ++o)
        {
            cosphi1 = cos(n * dphi + o * dpsi);
            sinphi1 = sin(n * dphi + o * dpsi);
            cosphi2 = cos(n * dphi + (o + 1) * dpsi);
            sinphi2 = sin(n * dphi + (o + 1) * dpsi);
            r1.set(cosphi1 * radius, sinphi1 * radius, 0.0);
            r2.set(cosphi2 * radius, sinphi2 * radius, 0.0);
            x1.set(cosphi1, sinphi1, 0.0);
            x2.set(cosphi2, sinphi2, 0.0);

            for (m = 0; m < nSlices; ++m)
            {
                // Plot along specified direction, and then map vertices from straight cylinder onto circle in
                // XY plane
                d1 = m * dtheta;
                d2 = d1 + dtheta;

                normal[0] = x1 * cos(d1) + y * sin(d1);
                normal[1] = x1 * cos(d2) + y * sin(d2);
                normal[2] = x2 * cos(d1) + y * sin(d1);
                normal[3] = x2 * cos(d2) + y * sin(d2);

                vert[0] = normal[0] * width + r1;
                vert[1] = normal[1] * width + r1;
                vert[2] = normal[2] * width + r2;
                vert[3] = normal[3] * width + r2;

                // Triangle 1
                defineVertex(vert[0].x, vert[0].y, vert[0].z, normal[0].x, normal[0].y, normal[0].z);
                defineVertex(vert[1].x, vert[1].y, vert[1].z, normal[1].x, normal[1].y, normal[1].z);
                defineVertex(vert[2].x, vert[2].y, vert[2].z, normal[2].x, normal[2].y, normal[2].z);

                // Triangle 2
                defineVertex(vert[1].x, vert[1].y, vert[1].z, normal[1].x, normal[1].y, normal[1].z);
                defineVertex(vert[2].x, vert[2].y, vert[2].z, normal[2].x, normal[2].y, normal[2].z);
                defineVertex(vert[3].x, vert[3].y, vert[3].z, normal[3].x, normal[3].y, normal[3].z);
            }
        }
    }
}

// Plot circle of specified radius
void Primitive::circle(double radius, int nStacks, int nSegments, bool segmented)
{
    int n, o;
    Vec3<GLfloat> r1, r2;
    double dphi, dpsi, cosphi1, sinphi1, cosphi2, sinphi2;

    type_ = GL_LINES;

    // Setup some variables
    dphi = TWOPI / nStacks;
    dpsi = dphi / nSegments;

    for (n = 0; n < nStacks; ++n)
    {
        // Calculate position around circle
        if (segmented && (n + 1) % 2)
            continue;

        for (o = 0; o < nSegments; ++o)
        {
            cosphi1 = cos(n * dphi + o * dpsi);
            sinphi1 = sin(n * dphi + o * dpsi);
            cosphi2 = cos(n * dphi + (o + 1) * dpsi);
            sinphi2 = sin(n * dphi + (o + 1) * dpsi);
            r1.set(cosphi1 * radius, sinphi1 * radius, 0.0);
            r2.set(cosphi2 * radius, sinphi2 * radius, 0.0);

            defineVertex(r1.x, r1.y, r1.z, 0.0, 0.0, 1.0);
            defineVertex(r2.x, r2.y, r2.z, 0.0, 0.0, 1.0);
        }
    }
}

// Create vertices of cross with specified width
void Primitive::cross(double width)
{
    Vec3<double> v;
    const auto halfWidth = 0.5 * width;
    for (auto i = 0; i < 3; ++i)
    {
        v.zero();
        v.set(i, halfWidth);
        defineVertex(v.x, v.y, v.z, 1.0, 1.0, 1.0);
        defineVertex(-v.x, -v.y, -v.z, 1.0, 1.0, 1.0);
        defineIndices(i * 2, i * 2 + 1);
    }
}

// Plot solid orthorhomboid of specified size at specified origin, and with sides subdivided into triangles ( ntriangles =
// 2*nSubs )
void Primitive::orthorhomboid(double sizex, double sizey, double sizez, int nSubs, double ox, double oy, double oz)
{
    // Create each face individually - 'offset' calculated so centre of orthorhombus is at <ox,oy,oz>
    GLfloat delta[3], veca[3], vecb[3], vertex[3], sizes[3], offset[3];
    int i, j, plane;
    sizes[0] = (GLfloat)sizex;
    sizes[1] = (GLfloat)sizey;
    sizes[2] = (GLfloat)sizez;
    delta[0] = (GLfloat)sizex / nSubs;
    delta[1] = (GLfloat)sizey / nSubs;
    delta[2] = (GLfloat)sizez / nSubs;
    offset[0] = (-sizex / 2.0f) + ox;
    offset[1] = (-sizey / 2.0f) + oy;
    offset[2] = (-sizez / 2.0f) + oz;

    // Set general origin coordinate
    // Loop over planes
    for (plane = 0; plane < 3; ++plane)
    {
        // Define deltas for this plane
        for (j = 0; j < 3; ++j)
        {
            veca[j] = 0.0;
            vecb[j] = 0.0;
        }
        veca[(plane + 1) % 3] = delta[(plane + 1) % 3];
        vecb[(plane + 2) % 3] = delta[(plane + 2) % 3];
        // Loop over subdivisions in plane
        for (i = 0; i < nSubs; ++i)
        {
            for (j = 0; j < nSubs; ++j)
            {
                vertex[0] = offset[0] + i * veca[0] + j * vecb[0];
                vertex[1] = offset[1] + i * veca[1] + j * vecb[1];
                vertex[2] = offset[2] + i * veca[2] + j * vecb[2];
                // Define triangle vertices for 'lower' plane
                defineVertex(vertex[0], vertex[1], vertex[2], plane == 0, -1 * (plane == 1), -1 * (plane == 2));
                defineVertex(vertex[0] + veca[0], vertex[1] + veca[1], vertex[2] + veca[2], plane == 0, -1 * (plane == 1),
                             -1 * (plane == 2));
                defineVertex(vertex[0] + veca[0] + vecb[0], vertex[1] + veca[1] + vecb[1], vertex[2] + veca[2] + vecb[2],
                             plane == 0, -1 * (plane == 1), -1 * (plane == 2));
                defineVertex(vertex[0], vertex[1], vertex[2], plane == 0, -1 * (plane == 1), -1 * (plane == 2));
                defineVertex(vertex[0] + vecb[0], vertex[1] + vecb[1], vertex[2] + vecb[2], plane == 0, -1 * (plane == 1),
                             -1 * (plane == 2));
                defineVertex(vertex[0] + veca[0] + vecb[0], vertex[1] + veca[1] + vecb[1], vertex[2] + veca[2] + vecb[2],
                             plane == 0, -1 * (plane == 1), -1 * (plane == 2));

                // Define trangle vertices for 'upper' plane
                vertex[plane] += sizes[plane];
                defineVertex(vertex[0], vertex[1], vertex[2], plane == 0, plane == 1, plane == 2);
                defineVertex(vertex[0] + veca[0], vertex[1] + veca[1], vertex[2] + veca[2], plane == 0, plane == 1, plane == 2);
                defineVertex(vertex[0] + veca[0] + vecb[0], vertex[1] + veca[1] + vecb[1], vertex[2] + veca[2] + vecb[2],
                             plane == 0, plane == 1, plane == 2);
                defineVertex(vertex[0], vertex[1], vertex[2], plane == 0, plane == 1, plane == 2);
                defineVertex(vertex[0] + vecb[0], vertex[1] + vecb[1], vertex[2] + vecb[2], plane == 0, plane == 1, plane == 2);
                defineVertex(vertex[0] + veca[0] + vecb[0], vertex[1] + veca[1] + vecb[1], vertex[2] + veca[2] + vecb[2],
                             plane == 0, plane == 1, plane == 2);
            }
        }
    }
}

// Plot wireframe orthorhomboid of specified size at specified origin, and with sides subdivided into triangles ( ntriangles =
// 2*nSubs )
void Primitive::wireOrthorhomboid(double sizex, double sizey, double sizez, double ox, double oy, double oz)
{
    GLfloat offset[3];
    offset[0] = ox - sizex * 0.5;
    offset[1] = oy - sizey * 0.5;
    offset[2] = oz - sizez * 0.5;

    // Define the eight vertices of the orthorhomboid
    defineVertex(offset[0], offset[1], offset[2], 1.0, 0.0, 0.0);                         // 0
    defineVertex(offset[0] + sizex, offset[1], offset[2], 1.0, 0.0, 0.0);                 // 1
    defineVertex(offset[0], offset[1] + sizey, offset[2], 1.0, 0.0, 0.0);                 // 2
    defineVertex(offset[0] + sizex, offset[1] + sizey, offset[2], 1.0, 0.0, 0.0);         // 3
    defineVertex(offset[0], offset[1], offset[2] + sizez, 1.0, 0.0, 0.0);                 // 4
    defineVertex(offset[0] + sizex, offset[1], offset[2] + sizez, 1.0, 0.0, 0.0);         // 5
    defineVertex(offset[0], offset[1] + sizey, offset[2] + sizez, 1.0, 0.0, 0.0);         // 6
    defineVertex(offset[0] + sizex, offset[1] + sizey, offset[2] + sizez, 1.0, 0.0, 0.0); // 7

    // Define edges
    // -- Along X
    defineIndices(0, 1);
    defineIndices(2, 3);
    defineIndices(4, 5);
    defineIndices(6, 7);
    // -- Along Y
    defineIndices(0, 2);
    defineIndices(1, 3);
    defineIndices(4, 6);
    defineIndices(5, 7);
    // -- Along Z
    defineIndices(0, 4);
    defineIndices(1, 5);
    defineIndices(2, 6);
    defineIndices(3, 7);
}

// Plot basic axes object
void Primitive::axes(double axisLength)
{
    const auto arrowSize = axisLength * 0.05;

    // X (first vertex = 0)
    defineVertex(0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    defineVertex(axisLength, 0.0, 0.0, 0.0, 0.0, 1.0);
    defineVertex(axisLength - arrowSize, arrowSize, 0.0, 0.0, 0.0, 1.0);
    defineVertex(axisLength - arrowSize, -arrowSize, 0.0, 0.0, 0.0, 1.0);
    defineIndices(0, 1);
    defineIndices(1, 2);
    defineIndices(1, 3);

    // X label (first vertex = 4)
    defineVertex(axisLength + arrowSize, arrowSize, 0.0, 0.0, 0.0, 1.0);
    defineVertex(axisLength + arrowSize, -arrowSize, 0.0, 0.0, 0.0, 1.0);
    defineVertex(axisLength + 2 * arrowSize, arrowSize, 0.0, 0.0, 0.0, 1.0);
    defineVertex(axisLength + 2 * arrowSize, -arrowSize, 0.0, 0.0, 0.0, 1.0);
    defineIndices(4, 7);
    defineIndices(5, 6);

    // Y (first vertex = 8)
    defineVertex(0.0, axisLength, 0.0, 0.0, 0.0, 1.0);
    defineVertex(arrowSize, axisLength - arrowSize, 0.0, 0.0, 0.0, 1.0);
    defineVertex(-arrowSize, axisLength - arrowSize, 0.0, 0.0, 0.0, 1.0);
    defineIndices(0, 8);
    defineIndices(8, 9);
    defineIndices(8, 10);

    // Y label (first vertex = 11)
    defineVertex(-arrowSize, axisLength + arrowSize, 0.0, 0.0, 0.0, 1.0);
    defineVertex(-arrowSize, axisLength + 2 * arrowSize, 0.0, 0.0, 0.0, 1.0);
    defineVertex(0.0, axisLength + 1.5 * arrowSize, 0.0, 0.0, 0.0, 1.0);
    defineVertex(arrowSize, axisLength + 1.5 * arrowSize, 0.0, 0.0, 0.0, 1.0);
    defineIndices(11, 13);
    defineIndices(12, 13);
    defineIndices(13, 14);

    // Z (first vertex = 15)
    defineVertex(0.0, 0.0, axisLength, 0.0, 0.0, 1.0);
    defineVertex(0.0, arrowSize, axisLength - arrowSize, 0.0, 0.0, 1.0);
    defineVertex(0.0, -arrowSize, axisLength - arrowSize, 0.0, 0.0, 1.0);
    defineIndices(0, 15);
    defineIndices(15, 16);
    defineIndices(15, 17);

    // Z label (first vertex = 18)
    defineVertex(0.0, arrowSize, axisLength + arrowSize, 0.0, 0.0, 1.0);
    defineVertex(0.0, arrowSize, axisLength + 2 * arrowSize, 0.0, 0.0, 1.0);
    defineVertex(0.0, -arrowSize, axisLength + arrowSize, 0.0, 0.0, 1.0);
    defineVertex(0.0, -arrowSize, axisLength + 2 * arrowSize, 0.0, 0.0, 1.0);
    defineIndices(18, 19);
    defineIndices(19, 20);
    defineIndices(20, 21);
}

// Plot basic coloured axes object
void Primitive::colouredAxes(double axisLength)
{
    const auto arrowSize = axisLength * 0.05;
    GLfloat rgba[4], black[4];
    black[0] = 0.0;
    black[1] = 0.0;
    black[2] = 0.0;
    black[3] = 1.0;

    // X (first vertex = 0)
    rgba[0] = 1.0;
    rgba[1] = 0.0;
    rgba[2] = 0.0;
    rgba[3] = 1.0;
    defineVertex(0.0, 0.0, 0.0, 0.0, 0.0, 1.0, rgba);
    defineVertex(axisLength, 0.0, 0.0, 0.0, 0.0, 1.0, rgba);
    defineVertex(axisLength - arrowSize, arrowSize, 0.0, 0.0, 0.0, 1.0, rgba);
    defineVertex(axisLength - arrowSize, -arrowSize, 0.0, 0.0, 0.0, 1.0, rgba);
    defineIndices(0, 1);
    defineIndices(1, 2);
    defineIndices(1, 3);

    // X label (first vertex = 4)
    defineVertex(axisLength + arrowSize, arrowSize, 0.0, 0.0, 0.0, 1.0, black);
    defineVertex(axisLength + arrowSize, -arrowSize, 0.0, 0.0, 0.0, 1.0, black);
    defineVertex(axisLength + 2 * arrowSize, arrowSize, 0.0, 0.0, 0.0, 1.0, black);
    defineVertex(axisLength + 2 * arrowSize, -arrowSize, 0.0, 0.0, 0.0, 1.0, black);
    defineIndices(4, 7);
    defineIndices(5, 6);

    // Y (first vertex = 8)
    rgba[0] = 0.0;
    rgba[1] = 1.0;
    defineVertex(0.0, axisLength, 0.0, 0.0, 0.0, 1.0, rgba);
    defineVertex(arrowSize, axisLength - arrowSize, 0.0, 0.0, 0.0, 1.0, rgba);
    defineVertex(-arrowSize, axisLength - arrowSize, 0.0, 0.0, 0.0, 1.0, rgba);
    defineIndices(0, 8);
    defineIndices(8, 9);
    defineIndices(8, 10);

    // Y label (first vertex = 11)
    defineVertex(-arrowSize, axisLength + arrowSize, 0.0, 0.0, 0.0, 1.0, black);
    defineVertex(-arrowSize, axisLength + 2 * arrowSize, 0.0, 0.0, 0.0, 1.0, black);
    defineVertex(0.0, axisLength + 1.5 * arrowSize, 0.0, 0.0, 0.0, 1.0, black);
    defineVertex(arrowSize, axisLength + 1.5 * arrowSize, 0.0, 0.0, 0.0, 1.0, black);
    defineIndices(11, 13);
    defineIndices(12, 13);
    defineIndices(13, 14);

    // Z (first vertex = 15)
    rgba[1] = 0.0;
    rgba[2] = 1.0;
    defineVertex(0.0, 0.0, axisLength, 0.0, 0.0, 1.0, rgba);
    defineVertex(0.0, arrowSize, axisLength - arrowSize, 0.0, 0.0, 1.0, rgba);
    defineVertex(0.0, -arrowSize, axisLength - arrowSize, 0.0, 0.0, 1.0, rgba);
    defineIndices(0, 15);
    defineIndices(15, 16);
    defineIndices(15, 17);

    // Z label (first vertex = 18)
    defineVertex(0.0, arrowSize, axisLength + arrowSize, 0.0, 0.0, 1.0, black);
    defineVertex(0.0, arrowSize, axisLength + 2 * arrowSize, 0.0, 0.0, 1.0, black);
    defineVertex(0.0, -arrowSize, axisLength + arrowSize, 0.0, 0.0, 1.0, black);
    defineVertex(0.0, -arrowSize, axisLength + 2 * arrowSize, 0.0, 0.0, 1.0, black);
    defineIndices(18, 19);
    defineIndices(19, 20);
    defineIndices(20, 21);
}
