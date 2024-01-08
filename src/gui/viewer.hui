// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/timer.h"
#include "gui/render/fontInstance.h"
#include "gui/render/renderable.h"
#include "gui/render/renderableGroupManager.h"
#include "gui/render/view.h"
#include "math/data1D.h"
#include <QMenu>
#include <QObject>
#include <QOffscreenSurface>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QTimer>
#include <set>
#include <vector>

// Forward Declarations
class DissolveWindow;
class QOpenGLFramebufferObject;

// Base rendering widget
class BaseViewer : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

    public:
    explicit BaseViewer(QWidget *parent);
    ~BaseViewer() override = default;

    /*
     * Main UI
     */
    protected:
    // Pointer to DissolveWindow (for optional integrations with main UI)
    std::optional<DissolveWindow *> dissolveWindow_;

    public:
    // Set pointer to DissolveWindow (for optional integrations with main UI)
    void setDissolveWindow(DissolveWindow *dissolveWindow);

    /*
     * View
     */
    protected:
    // Font instance for this viewer
    FontInstance fontInstance_;
    // View for renderable data
    View view_;
    // Viewer to which this view is linked (is dependent on for its view)
    BaseViewer *linkedViewer_;
    // List of other viewers that are dependent on (i.e. are linked to) this one
    std::set<BaseViewer *> dependentViewers_;

    private:
    // Register that the specified viewer depends on (links to) this one
    void addDependentViewer(BaseViewer *viewer);
    // Unregister the specified viewer
    void removeDependentViewer(BaseViewer *viewer);

    public:
    // Return the View definition
    View &view();
    const View &view() const;

    /*
     * Renderable Data
     */
    protected:
    // Vector of renderables to display
    std::vector<std::shared_ptr<Renderable>> renderables_;

    public:
    // Clear existing data
    void clear();
    // Create a new Renderable (by object name)
    template <class T>
    std::shared_ptr<T> createRenderable(std::string_view objectName, std::string_view renderableName,
                                        std::string_view groupName = "")
    {
        auto renderable = std::make_shared<T>(objectName);
        renderables_.emplace_back(renderable);

        renderable->setName(renderableName);

        if (!groupName.empty())
            groupManager_.addToGroup(renderable, groupName);

        Q_EMIT(renderableAdded());

        return renderable;
    }
    // Create a new Renderable (by object reference)
    template <class T, class D>
    std::shared_ptr<T> createRenderable(const D &objectData, std::string_view renderableName, std::string_view groupName = "")
    {
        auto renderable = std::make_shared<T>(objectData);
        renderables_.emplace_back(renderable);

        renderable->setName(renderableName);

        if (!groupName.empty())
            groupManager_.addToGroup(renderable, groupName);

        Q_EMIT(renderableAdded());

        return renderable;
    }
    // Create a new Renderable (by object data pointer)
    template <class T, class D>
    std::shared_ptr<T> createRenderable(const D *objectData, std::string_view renderableName, std::string_view groupName = "")
    {
        auto renderable = std::make_shared<T>(objectData, renderableName);
        renderables_.emplace_back(renderable);

        renderable->setName(renderableName);

        if (!groupName.empty())
            groupManager_.addToGroup(renderable, groupName);

        Q_EMIT(renderableAdded());

        return renderable;
    }
    // Add an existing Renderable
    void addRenderable(const std::shared_ptr<Renderable> &renderable);
    // Remove existing Renderable
    void removeRenderable(const std::shared_ptr<Renderable> &renderable);
    // Clear all Renderables
    void clearRenderables();
    // Attempt to validate all renderables
    void validateRenderables(const GenericList &source);
    // Return list of Renderables
    const std::vector<std::shared_ptr<Renderable>> &renderables() const;
    // Return Renderable with specified tag (if it exists)
    std::shared_ptr<Renderable> renderableWithTag(std::string_view tag) const;
    // Emit the signal to notify that the viewing style of data represented in the Viewer has changed
    void notifyStyleModified();
    // Emit the signal to notify that the data has changed
    void notifyDataModified();

    Q_SIGNALS:
    // A Renderable has been added for display
    void renderableAdded();
    // A Renderable has been removed from display
    void renderableRemoved();
    // A Renderable has been changed (re-grouped etc.)
    void renderableChanged();
    // Viewing style of data represented in the Viewer has changed
    void styleModified();
    // Data has changed
    void dataModified();

    /*
     * Renderable Groups
     */
    protected:
    // Group manager for Renderables
    RenderableGroupManager groupManager_;

    public:
    // Return the group manager for Renderables
    RenderableGroupManager &groupManager();
    // Add Renderable to specified group
    void addRenderableToGroup(std::shared_ptr<Renderable> &renderable, std::string_view group);

    /*
     * Options
     */
    private:
    // Whether Axes are visible
    bool axesVisible_;
    // Whether to apply clip planes about Axes volume for Renderables
    bool clipToAxesVolume_;

    public:
    // Set whether Axes are visible
    void setAxesVisible(bool visible);
    // Return whether Axes are visible
    bool axesVisible() const;
    // Set whether to apply clip planes about Axes volume for Renderables
    void setClipToAxesVolume(bool clip);
    // Return whether clip planes are applied about Axes volume for Renderables
    bool clipToAxesVolume() const;

    /*
     * Rendering
     */
    protected:
    // Width and height of the context
    GLsizei contextWidth_, contextHeight_;
    // Flag to indicate whether we may draw to the context
    bool valid_;
    // Flag indicating if we are currently drawing to this context
    bool drawing_;
    // Flag specifying whether we are currently rendering offscreen
    bool renderingOffScreen_;
    // Scaling to use for objects measured in pixel widths / point sizes
    double pixelScaling_;
    // Offscreen context
    QOpenGLContext offscreenContext_;
    // Offscreen surface
    QOffscreenSurface offscreenSurface_;
    // Current offscreen buffer
    QOpenGLFramebufferObject *offscreenBuffer_;
    // Device pixel ratio
    static double devicePixelRatio_;

    private:
    // Initialise context widget (when created by Qt)
    void initializeGL() override;
    // Widget repaint
    void paintGL() override;
    // Resize function
    void resizeGL(int width, int height) override;
    // Main rendering function
    void renderGL(int xOffset = 0, int yOffset = 0);
    // Create viewer update stack
    void createUpdateStack(std::vector<BaseViewer *> &updateStack);
    // Perform post-initialisation operations
    virtual void postInitialiseGL();
    // Perform post-resize operations
    virtual void postResizeGL();
    // Setup basic GL properties
    virtual void setupGL();
    // Render 2D overlay content
    virtual void render2DOverlay() = 0;
    // Set scaling to use for objects measured in pixel widths / point sizes
    void setPixelScaling(double scaling);

    protected:
    // Set and enable clip planes suitable for current axis limits and view
    void enableClipping();
    // Disable clip planes
    void disableClipping();

    public:
    // Return the current height of the drawing area
    GLsizei contextHeight() const;
    // Return the current width of the drawing area
    GLsizei contextWidth() const;
    // Check for GL error
    void checkGlError();
    // Refresh widget / scene
    void postRedisplay();
    // Grab current contents of framebuffer
    QPixmap frameBuffer();
    // Render current scene at supplied size
    QPixmap generateImage(int imageWidth, int imageHeight);
    // Copy current view to clipboard as an image
    void copyViewToClipboard(bool checked);
    // Set device pixel ratio
    static void setDevicePixelRatio(double ratio);

    /*
     * Mouse / Keyboard Input
     */
    protected:
    // Canvas coordinates of mouse down
    Vec3<int> rMouseDown_;
    // Mouse release timer for detection of context menu requests
    QTimer mouseReleaseTimer_;
    // Canvas coordinates of mouse cursor
    Vec3<int> rMouseLast_;
    // Current state of mouse buttons
    Qt::MouseButtons buttonState_;
    // Modifier state on mouse down
    Qt::KeyboardModifiers mouseDownModifiers_;

    private:
    // Mouse press event
    void mousePressEvent(QMouseEvent *event) override;
    // Mouse release event
    void mouseReleaseEvent(QMouseEvent *event) override;
    // Mouse move event
    void mouseMoveEvent(QMouseEvent *event) override;
    // Mouse wheel event
    void wheelEvent(QWheelEvent *event) override;
    // Mouse double click event
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    private Q_SLOTS:
    // Key press event
    void keyPressEvent(QKeyEvent *) override;
    // Key release event
    void keyReleaseEvent(QKeyEvent *) override;

    protected:
    // Mouse moved
    virtual void mouseMoved(int dx, int dy);
    // Mouse 'wheeled'
    virtual void mouseWheeled(int delta);
    // Mouse double clicked
    virtual void mouseDoubleClicked();
    // Context menu requested
    virtual void contextMenuRequested(QPoint pos);
    // Key pressed
    virtual bool keyPressed(int key);
    // Key released
    virtual bool keyReleased(int key);

    /*
     * Interaction
     */
    private:
    // Clicked coordinates (in Axes frame) in 2D view
    Vec3<double> clicked2DAxesCoordinates_;
    // Current coordinates (in Axes frame) under mouse in 2D view
    Vec3<double> current2DAxesCoordinates_;
    // Whether the viewer is currently being interacted with
    bool interacting_;

    protected:
    // Start interaction
    virtual void startInteraction();
    // End interaction
    virtual void endInteraction();
    // Cancel current interaction
    virtual void cancelInteraction();

    public:
    // Return whether the viewer is currently being interacted with
    bool interacting() const;
    // Return text describing current interaction mode
    virtual const QString interactionModeText() const;
    // Return clicked coordinates (in Axes frame) in 2D view
    Vec3<double> clicked2DAxesCoordinates() const;
    // Return current coordinates (in Axes frame) under mouse in 2D view
    Vec3<double> current2DAxesCoordinates() const;
    // Calculate 2D Axes coordinate from screen coordinates
    Vec3<double> screenTo2DAxes(int mouseX, int mouseY);

    Q_SIGNALS:
    // Notification that interaction mode has changed
    void interactionModeChanged();

    /*
     * Object Querying
     */
    public:
    // Drawing object type
    enum ViewerObject
    {
        NoObject,
        AxisLineObject,
        AxisTickLabelObject,
        AxisTitleLabelObject,
        GridLineMajorObject,
        GridLineMinorObject,
        RenderableObject,
        CustomObject
    };
    // Return description of viewer object
    static std::string_view viewerObject(ViewerObject vo);

    private:
    // Whether we are currently running an object query
    bool queryingObjects_;
    // Bottom-left coordinates of region to probe for objects at next redraw
    int queryRegionBottom_, queryRegionLeft_;
    // Size of area to sample, starting from bottom-left coordinates
    int queryRegionWidth_, queryRegionHeight_;
    // Colour contents of sample region
    std::vector<GLfloat> queryRegionR_, queryRegionG_, queryRegionB_;
    // Scaling of generated query image
    double queryImageScale_;
    // Width of query image
    int queryImageWidth_;
    // Height of query image
    int queryImageHeight_;
    // Type of object found at query coordinates
    ViewerObject queryObjectType_;
    // Information describing object at query coordinates
    std::string queryObjectInfo_;
    // Secondary information describing object at query coordinates
    std::string queryObjectSubInfo_;

    private:
    // Generate query image at specified scaling
    void generateQueryImage();

    protected:
    // Update object query, setting supplied information if the sample area has changed significantly
    void updateQuery(BaseViewer::ViewerObject objectType, std::string_view info, std::string_view subInfo = "");

    public:
    // Return the type of object at the specified coordinates
    ViewerObject queryAt(int x, int y);
    // Return info for object at query coordinates
    std::string_view queryObjectInfo() const;
    // Return sub-info for object at query coordinates
    std::string_view queryObjectSubInfo() const;
};
