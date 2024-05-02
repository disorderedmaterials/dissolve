// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/messenger.h"
#include "base/sysFunc.h"
#include "gui/render/fontInstance.h"
#include "gui/viewer.h"
#include <QApplication>
#include <QClipboard>
#include <QOpenGLFramebufferObjectFormat>
#include <QPainter>
#include <QProgressDialog>
#include <algorithm>

// Device pixel ratio
double BaseViewer::devicePixelRatio_{1.0};

// Initialise context widget (when created by Qt)
void BaseViewer::initializeGL()
{
    // Setup function pointers to OpenGL extension functions
    initializeOpenGLFunctions();

    // Set up the font instance
    fontInstance_.setUp();

    // Setup offscreen context
    Messenger::printVerbose("Setting up offscreen context and surface...");
    offscreenContext_.setShareContext(context());
    offscreenContext_.setFormat(context()->format());
    offscreenContext_.create();
    offscreenSurface_.setFormat(context()->format());
    offscreenSurface_.create();
    Messenger::printVerbose("Done.");

    // Check for vertex buffer extensions
    if ((!hasOpenGLFeature(QOpenGLFunctions::Buffers)) &&
        (PrimitiveInstance::globalInstanceType() == PrimitiveInstance::VBOInstance))
    {
        Messenger::warn("VBO extension is requested but not available, so reverting to display lists instead.\n");
        PrimitiveInstance::setGlobalInstanceType(PrimitiveInstance::ListInstance);
    }

    valid_ = true;

    // Recalculate viewport
    view_.recalculateViewport(contextWidth_, contextHeight_);
    view_.recalculateView();

    // Perform any custom post-initialisation operations
    postInitialiseGL();
}

// Widget repaint
void BaseViewer::paintGL()
{
    // Do nothing if the canvas is not valid, or we are still drawing from last time
    if ((!valid_) || drawing_)
        return;

    // Set the drawing flag so we don't have any rendering clashes
    drawing_ = true;

    // Setup basic GL stuff
    setupGL();

    // Render full scene
    renderGL();

    // Set the rendering flag to false
    drawing_ = false;
}

// Resize function
void BaseViewer::resizeGL(int newwidth, int newheight)
{
    // Store the new width and height of the widget
    contextWidth_ = (GLsizei)newwidth;
    contextHeight_ = (GLsizei)newheight;

    // Recalculate viewport
    view_.recalculateViewport(contextWidth_, contextHeight_);
    view_.recalculateView();

    // Perform any custom post-resize operations
    postResizeGL();
}

// Main rendering function
void BaseViewer::renderGL(int xOffset, int yOffset)
{
    GLfloat colourBlack[4] = {0.0, 0.0, 0.0, 1.0};

    // Set colour mode
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_2D);

    // Clear the colour and depth buffers
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Update the View
    if (view().autoFollowType() != View::NoAutoFollow)
        view().autoFollowData();
    view().recalculateView();

    // Set-up the GL viewport
    glViewport(view_.viewportMatrix()[0] + xOffset, view_.viewportMatrix()[1] + yOffset,
               view_.viewportMatrix()[2] * devicePixelRatio_, view_.viewportMatrix()[3] * devicePixelRatio_);

    // Apply our View's projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(view_.projectionMatrix().matrix());

    // Grab the View's standard and inverse matrices (from the linked view if available)
    Matrix4 viewMatrix = view().viewMatrix();
    Matrix4 viewRotationInverse = view().viewRotationInverse();

    // Apply the view matrix, and render the Axes associated to the View
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(viewMatrix.matrix());
    glColor4fv(colourBlack);

    if (axesVisible_)
    {
        auto skipAxis = -1;
        if (view_.viewType() == View::FlatXYView)
            skipAxis = 2;
        else if (view_.viewType() == View::FlatXZView)
            skipAxis = 1;
        else if (view_.viewType() == View::FlatZYView)
            skipAxis = 0;

        // -- Render axis text
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_BLEND);
        glDisable(GL_LIGHTING);
        LineStyle().sendToGL(pixelScaling_);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        if (fontInstance_.fontOK())
        {
            fontInstance_.setFaceSize(1);
            for (auto axis = 0; axis < 3; ++axis)
                if (view_.axes().visible(axis) && (axis != skipAxis))
                {
                    view_.axes().labelPrimitive(axis).renderAll(fontInstance_, viewMatrix, viewRotationInverse,
                                                                view_.textZScale());
                    updateQuery(BaseViewer::AxisTickLabelObject, fmt::format("{}", axis), fmt::format("{}", char(88 + axis)));
                    view_.axes().titlePrimitive(axis).renderAll(fontInstance_, viewMatrix, viewRotationInverse,
                                                                view_.textZScale());
                    updateQuery(BaseViewer::AxisTitleLabelObject, fmt::format("{}", axis), fmt::format("{}", char(88 + axis)));
                }
        }

        // -- Render axis (and grid) lines
        glLoadMatrixd(viewMatrix.matrix());
        glDisable(GL_LIGHTING);
        glEnable(GL_LINE_SMOOTH);
        for (auto axis = 0; axis < 3; ++axis)
            if (view_.axes().visible(axis) && (axis != skipAxis))
            {
                view_.axes().gridLineMinorStyle(axis).sendToGL(pixelScaling_);
                view_.axes().gridLineMinorPrimitive(axis).sendToGL();
                updateQuery(BaseViewer::GridLineMinorObject, fmt::format("{}", axis), fmt::format("{}", char(88 + axis)));
            }
        for (auto axis = 0; axis < 3; ++axis)
            if (view_.axes().visible(axis) && (axis != skipAxis))
            {
                view_.axes().gridLineMajorStyle(axis).sendToGL(pixelScaling_);
                view_.axes().gridLineMajorPrimitive(axis).sendToGL();
                updateQuery(BaseViewer::GridLineMajorObject, fmt::format("{}", axis), fmt::format("{}", char(88 + axis)));
            }

        // -- Reset line style, ensure polygons are now filled, and render the axis lines
        LineStyle().sendToGL(pixelScaling_);
        for (auto axis = 0; axis < 3; ++axis)
            if (view_.axes().visible(axis) && (axis != skipAxis))
            {
                view_.axes().axisPrimitive(axis).sendToGL();
                updateQuery(BaseViewer::AxisLineObject, fmt::format("{}", axis), fmt::format("{}", char(88 + axis)));
            }
        glEnable(GL_LIGHTING);
        glDisable(GL_LINE_SMOOTH);
    }

    // Enable clip planes to enforce limits in Axes volume
    if (clipToAxesVolume_)
        enableClipping();

    // Draw all Renderables
    for (auto &rend : renderables_)
    {
        // If the Renderable is hidden, don't draw it!
        if (!rend->isVisible())
            continue;

        // Make sure the Renderable is up to date, and draw it
        rend->updateAndSendPrimitives(view(), renderingOffScreen_, renderingOffScreen_, context(), pixelScaling_);

        // Update query
        updateQuery(BaseViewer::RenderableObject, rend->tag(), Renderable::renderableTypes().keyword(rend->type()));

        glEnable(GL_COLOR_MATERIAL);
    }

    // Disable clip planes
    if (clipToAxesVolume_)
        disableClipping();

    // Set up an orthographic matrix for any 2D overlays
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, view_.viewportMatrix()[2], 0, view_.viewportMatrix()[3], -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glDisable(GL_LIGHTING);

    // Call the derived-class overlay function
    render2DOverlay();
}

// Create viewer update stack
void BaseViewer::createUpdateStack(std::vector<BaseViewer *> &updateStack)
{
    // If we are already in the list, return immediately...
    if (std::find(updateStack.begin(), updateStack.end(), this) != updateStack.end())
        return;

    // Add ourselves to the stack
    updateStack.push_back(this);

    // If we are a linked view, append viewer through the linked view
    if (linkedViewer_)
    {
        linkedViewer_->createUpdateStack(updateStack);
        return;
    }

    // Add any viewers that are dependent on us (i.e. link to us)
    for (BaseViewer *viewer : dependentViewers_)
        viewer->createUpdateStack(updateStack);
}

// Perform post-initialisation operations
void BaseViewer::postInitialiseGL() {}

// Perform post-resize operations
void BaseViewer::postResizeGL() {}

// Setup basic GL properties (called each time before renderScene())
void BaseViewer::setupGL()
{
    // Define colours etc.
    GLfloat spotlightAmbient[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat spotlightDiffuse[4] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat spotlightSpecular[4] = {0.7f, 0.7f, 0.7f, 1.0f};
    GLfloat spotlightPosition[4] = {1.0f, 1.0f, 0.0f, 0.0f};
    GLfloat specularColour[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    auto background = palette().base().color();
    GLfloat backgroundColour[4] = {background.redF(), background.greenF(), background.blueF(), background.alphaF()};

    // Clear (background) colour
    glClearColor(backgroundColour[0], backgroundColour[1], backgroundColour[2], backgroundColour[3]);

    // Perspective hint
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Smooth shading
    glShadeModel(GL_SMOOTH);

    // Auto-normalise surface normals
    glEnable(GL_NORMALIZE);

    // Set up the light model
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_AMBIENT, spotlightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, spotlightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spotlightSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, spotlightPosition);
    glEnable(GL_LIGHT0);

    // Set specular reflection colour
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColour);
    glMateriali(GL_FRONT, GL_SHININESS, 127);

    // Configure antialiasing
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}

/*
 * Public Functions
 */

// Return the current height of the drawing area
GLsizei BaseViewer::contextHeight() const { return contextHeight_; }

// Return the current width of the drawing area
GLsizei BaseViewer::contextWidth() const { return contextWidth_; }

// Check for GL error
void BaseViewer::checkGlError()
{
    GLenum glerr = GL_NO_ERROR;
    do
    {
        switch (glGetError())
        {
            case (GL_INVALID_ENUM):
                Messenger::printVerbose("GLenum argument out of range\n");
                break;
            case (GL_INVALID_VALUE):
                Messenger::printVerbose("Numeric argument out of range\n");
                break;
            case (GL_INVALID_OPERATION):
                Messenger::printVerbose("Operation illegal in current state\n");
                break;
            case (GL_STACK_OVERFLOW):
                Messenger::printVerbose("Command would cause a stack overflow\n");
                break;
            case (GL_STACK_UNDERFLOW):
                Messenger::printVerbose("Command would cause a stack underflow\n");
                break;
            case (GL_OUT_OF_MEMORY):
                Messenger::printVerbose("Not enough memory left to execute command\n");
                break;
            case (GL_NO_ERROR):
                Messenger::printVerbose("No GL error\n");
                return;
                // 			case (GL_CONTEXT_LOST): return;		/* Not recognised on OSX? */
            default:
                Messenger::printVerbose("Unknown GL error?\n");
                break;
        }
    } while (glerr != GL_NO_ERROR);
}

// Refresh widget / scene
void BaseViewer::postRedisplay()
{
    if ((!valid_) || drawing_)
        return;

    // This is the entry-point for our assmebling of a unique list of viewer to update from this call, including any linked
    // viewers
    std::vector<BaseViewer *> updateStack;
    createUpdateStack(updateStack);

    for (auto *viewer : updateStack)
    {
        if ((!viewer->valid_) || (viewer->drawing_))
            continue;
        viewer->update();
    }
}

// Set scaling to use for objects measured in pixel widths / point sizes
void BaseViewer::setPixelScaling(double scaling)
{
    pixelScaling_ = scaling;

    fontInstance_.setScaleFactor(scaling);
}

// Set and enable clip planes suitable for current axis limits and view
void BaseViewer::enableClipping()
{
    // Load identity matrix
    glLoadMatrixd(view_.viewMatrix().matrix());

    // Get clip plane coordinates and indices
    auto clipMin = view_.axes().clipMin();
    auto clipMax = view_.axes().clipMax();
    static GLenum planes[6] = {GL_CLIP_PLANE0, GL_CLIP_PLANE1, GL_CLIP_PLANE2, GL_CLIP_PLANE3, GL_CLIP_PLANE4, GL_CLIP_PLANE5};

    // Loop over axes
    Vec3<double> translation;
    for (auto axis = 0; axis < 3; ++axis)
    {
        translation.zero();

        // Positive
        translation[axis] = clipMin[axis];
        GLdouble plane[4] = {0.0, 0.0, 0.0, 0.0};
        plane[axis] = 1.0;

        glPushMatrix();
        glTranslated(translation.x, translation.y, translation.z);
        glClipPlane(planes[axis], plane);
        glEnable(planes[axis]);
        glPopMatrix();

        // Negative
        translation[axis] = clipMax[axis];
        plane[axis] = -1.0;

        glPushMatrix();
        glTranslated(translation.x, translation.y, translation.z);
        glClipPlane(planes[axis + 3], plane);
        glEnable(planes[axis + 3]);
        glPopMatrix();
    }
}

// Disable clip planes
void BaseViewer::disableClipping()
{
    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);
    glDisable(GL_CLIP_PLANE2);
    glDisable(GL_CLIP_PLANE3);
    glDisable(GL_CLIP_PLANE4);
    glDisable(GL_CLIP_PLANE5);
}

// Grab current contents of framebuffer
QPixmap BaseViewer::frameBuffer() { return QPixmap::fromImage(grabFramebuffer()); }

// Render or grab image
QPixmap BaseViewer::generateImage(int imageWidth, int imageHeight)
{
    // Flag that we are rendering offscreen
    renderingOffScreen_ = true;

    // Scale pixel-based measures to reflect size of exported image
    setPixelScaling(double(imageHeight) / double(contextHeight()));

    // Make the offscreen surface the current context
    offscreenContext_.makeCurrent(&offscreenSurface_);

    // Set tile size
    auto tileWidth = 512;
    auto tileHeight = 512;

    // Initialise framebuffer format and object
    QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setMipmap(true);
    fboFormat.setSamples(4);
    fboFormat.setAttachment(QOpenGLFramebufferObject::Depth);
    offscreenBuffer_ = new QOpenGLFramebufferObject(tileWidth, tileHeight, fboFormat);

    if (!offscreenBuffer_->bind())
    {
        Messenger::print("Failed to bind framebuffer object when generating image.");
        return QPixmap();
    }

    // Create a QPixmap of the desired full size and a QPainter for it
    QPixmap pixmap = QPixmap(imageWidth, imageHeight);
    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawRect(0, 0, imageWidth, imageHeight);

    // Calculate the number of tiles required to cover the requested image size
    auto nX = imageWidth / tileWidth + ((imageWidth % tileWidth) ? 1 : 0);
    auto nY = imageHeight / tileHeight + ((imageHeight % tileHeight) ? 1 : 0);

    // Set the viewport of our View to be the new image size
    view_.recalculateViewport(imageWidth, imageHeight);

    // Loop over tiles in x and y
    QProgressDialog progress("Generating tiled image", "Cancel", 0, nX * nY);
    progress.setWindowTitle("Dissolve");
    for (auto x = 0; x < nX; ++x)
    {
        for (auto y = 0; y < nY; ++y)
        {
            // Set progress value and check for cancellation
            if (progress.wasCanceled())
                break;
            progress.setValue(x * nY + y);

            // Generate this tile
            if (!offscreenBuffer_->bind())
                Messenger::error("Failed to bind framebuffer object.\n");
            setupGL();
            renderGL(-x * tileWidth, -y * tileHeight);
            QImage fboImage(offscreenBuffer_->toImage());
            QImage tile(fboImage.constBits(), fboImage.width(), fboImage.height(), QImage::Format_ARGB32);

            // Paste this tile into the main image
            painter.drawImage(x * tileWidth, imageHeight - (y + 1) * tileHeight, tile);
        }
        if (progress.wasCanceled())
            break;
    }

    // Finalise and save
    painter.end();

    // Reset pixel scaling
    setPixelScaling(1.0);

    // Turn off offscreen rendering
    renderingOffScreen_ = false;

    // Reset the viewport of the View
    view_.recalculateViewport(contextWidth_, contextHeight_);

    // Reset context back to main view
    makeCurrent();

    // Delete the temporary buffer
    delete offscreenBuffer_;

    return pixmap;
}

// Copy current view to clipboard as an image
void BaseViewer::copyViewToClipboard(bool checked)
{
    // Generate image of current view
    QPixmap pixmap = frameBuffer();

    // Copy image to clipboard
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setImage(pixmap.toImage());
}

// Set device pixel ratio
void BaseViewer::setDevicePixelRatio(double ratio) { devicePixelRatio_ = ratio; }
