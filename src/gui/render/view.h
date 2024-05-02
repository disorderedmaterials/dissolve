// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/render/axes.h"
#include "gui/render/renderable.h"
#include "gui/render/renderableGroup.h"
#include "math/matrix4.h"

// Forward Declarations
class FontInstance;

// Rendering View
class View
{
    private:
    // Associated FontInstance from parent viewer
    FontInstance &fontInstance_;
    // List of Renderables that we are to display
    const std::vector<std::shared_ptr<Renderable>> &renderables_;

    public:
    View(const std::vector<std::shared_ptr<Renderable>> &renderables, FontInstance &fontInstance);
    ~View();
    // Clear view, resetting to defaults
    void clear();

    /*
     * Geometry
     */
    private:
    // Pixel offsets for view
    int xOffset_, yOffset_;
    // Scale factors for view
    double xScale_, yScale_;
    // Aspect ratio of view
    double aspectRatio_;
    // Viewport matrix for GL
    GLuint viewportMatrix_[4];
    // Version of viewport matrix
    int viewportVersion_;

    public:
    // Set pixel offsets and scales to use
    void setOffsetAndScale(int xOffset, int yOffset, double xScale, double yScale);
    // Recalculate viewport matrix based on pixel dimensions provided
    void recalculateViewport(int width, int height);
    // Translate viewport by specified pixel amounts
    void translateViewport(int deltaX, int deltaY);
    // Return viewport matrix
    const GLuint *viewportMatrix() const;

    /*
     * Projection / View
     */
    public:
    // View type
    enum ViewType
    {
        NormalView,
        AutoStretchedView,
        FlatXYView,
        FlatXZView,
        FlatZYView,
        nViewTypes
    };
    // Return enum options for ViewType
    static EnumOptions<View::ViewType> viewTypes();
    // AutoFollow type
    enum AutoFollowType
    {
        NoAutoFollow,
        AllAutoFollow,
        XAutoFollow,
        nAutoFollowTypes
    };
    // Return enum options for AutoFollowType
    static EnumOptions<View::AutoFollowType> autoFollowTypes();

    private:
    // Type of view to use
    ViewType viewType_;
    // Linked View, if any
    View *linkedView_;
    // Projection matrix for GL
    Matrix4 projectionMatrix_;
    // Whether projection has perspective
    bool hasPerspective_;
    // Field of view angle used in projectionMatrix_ when using perspective
    double perspectiveFieldOfView_;
    // View rotation matrix for GL
    Matrix4 viewRotation_;
    // Logpoint for view rotation matrix
    int viewRotationPoint_;
    // View rotation matrix inverse
    Matrix4 viewRotationInverse_;
    // Logpoint for view rotation inverse matrix calculation
    int viewRotationInversePoint_;
    // Current translation of view
    Vec3<double> viewTranslation_;
    // Full view matrix (rotation + translation)
    Matrix4 viewMatrix_;
    // Inverse of view matrix
    Matrix4 viewMatrixInverse_;
    // Default z offset for translation matrix
    static const double defaultZTranslation_;
    // Axes version at which view matrix was last calculated (mostly for keeping 2D view correct)
    int viewAxesUsedAt_;
    // Viewport version at which view matrix was last calculated
    int viewViewportUsedAt_;
    // Auto-follow type in effect
    AutoFollowType autoFollowType_;
    // Transformed data versions at last auto-follow
    std::vector<std::pair<std::shared_ptr<Renderable>, int>> autoFollowTransformVersions_;
    // Length of X region to follow, if autoFollowType_ == XFollow
    double autoFollowXLength_;

    private:
    // Return calculated projection matrix
    Matrix4 calculateProjectionMatrix(bool hasPerspective, double orthoZoom = 0.0) const;

    public:
    // Set view type
    void setViewType(View::ViewType vt);
    // Return view type
    View::ViewType viewType() const;
    // Set linked View, if any
    void setLinkedView(View *linkedView);
    // Return linked View, if any
    View *linkedView() const;
    // Return whether view type is flat
    bool isFlatView() const;
    // Return projection matrix
    Matrix4 projectionMatrix() const;
    // Set whether the view uses perspective
    void setHasPerspective(bool perspective);
    // Return whether the view uses perspective
    bool hasPerspective() const;
    // Update view matrix
    void setViewRotation(Matrix4 &mat);
    // Update single column of view matrix
    void setViewRotationColumn(int column, double x, double y, double z);
    // Rotate view matrix about x and y by amounts specified
    void rotateView(double dx, double dy);
    // Return rotation matrix
    const Matrix4 &viewRotation() const;
    // Return view rotation inverse
    Matrix4 viewRotationInverse();
    // Set view translation
    void setViewTranslation(double x, double y, double z);
    // Translate view matrix by amounts specified
    void translateView(double dx, double dy, double dz);
    // Return current view translation
    Vec3<double> viewTranslation() const;
    // Update view matrix
    void updateViewMatrix();
    // Return view matrix
    const Matrix4 &viewMatrix() const;
    // Project given data coordinates into world coordinates
    Vec3<double> dataToWorld(Vec3<double> r) const;
    // Project given data coordinates into screen coordinates
    Vec3<double> dataToScreen(Vec3<double> r) const;
    // Project given data coordinates into screen coordinates, with corresponding distance 'delta' in data
    Vec3<double> dataToScreen(Vec3<double> r, double &delta) const;
    // Project given data coordinates into screen coordinates using supplied rotation matrix and translation vector
    Vec3<double> dataToScreen(Vec3<double> r, Matrix4 projectionMatrix, Matrix4 rotationMatrix,
                              Vec3<double> translation = Vec3<double>()) const;
    // Return z translation necessary to display coordinates supplied, assuming the identity view matrix
    double calculateRequiredZoom(double xMax, double yMax, double fraction) const;
    // Convert screen coordinates into data space coordinates
    Vec3<double> screenToData(int x, int y, double z) const;
    // Calculate selection axis coordinate from supplied screen coordinates
    double screenToAxis(int axis, int x, int y, bool clamp) const;
    // Recalculate current view parameters (e.g. for 2D, autostretched 3D etc.)
    void recalculateView(bool force = false);
    // Reset view matrix to face XY plane
    void resetViewMatrix();
    // Set display limits to show all available data
    void showAllData(double xFrac = 1.0, double yFrac = 1.0, double zFrac = 1.0);
    // Zoom to specified region
    void zoomTo(Vec3<double> limit1, Vec3<double> limit2);
    // Scale the currently displayed range
    void scaleRange(double factor);
    // Centre 2D view at specified coordinates, optionally moving only by a fraction of the distance required
    void centre2DAt(Vec3<double> centre, double fraction);
    // Set auto-follow type in effect
    void setAutoFollowType(AutoFollowType aft);
    // Cycle auto-follow type in effect
    void cycleAutoFollowType();
    // Return auto-follow type in effect
    AutoFollowType autoFollowType() const;
    // Set length of X region to follow, if autoFollowType_ == XFollow
    void setAutoFollowXLength(double length);
    // Return length of X region to follow, if autoFollowType_ == XFollow
    double autoFollowXLength() const;
    // Set axis limits based on current auto-follow type
    void autoFollowData();

    /*
     * Axes
     */
    private:
    // Axes for the view
    Axes axes_;
    // Pixel 'lengths' of axes in flat views
    Vec3<double> axisPixelLength_;

    public:
    // Return data minima over all displayed renderables
    Vec3<double> dataMinima();
    // Return data maxima over all displayed renderables
    Vec3<double> dataMaxima();
    // Return positive data minima over all displayed renderables
    Vec3<double> positiveDataMinima();
    // Return positive data maxima over all displayed renderables
    Vec3<double> positiveDataMaxima();
    // Update axis limits to represent data extent of renderables
    void updateAxisLimits(double xFrac = 1.0, double yFrac = 1.0, double zFrac = 1.0);
    // Shift flat view axis limits by specified amounts
    void shiftFlatAxisLimits(double deltaH, double deltaV);
    // Shift flat view axis limits by specified fractional amounts
    void shiftFlatAxisLimitsFractional(double fracH, double fracV);
    // Return axes for the view
    Axes &axes();
    const Axes &axes() const;

    /*
     * Style
     */
    private:
    // Font scaling for axis value labels
    double labelPointSize_;
    // Font scaling for titles
    double titlePointSize_;
    // Text z scaling factor
    double textZScale_;
    // Whether axis text labels are drawn flat in 3D views
    bool flatLabelsIn3D_;

    private:
    // Calculate font scaling factor
    void calculateFontScaling();

    public:
    // Set font point size for axis value labels
    void setLabelPointSize(double value);
    // Return font point size for axis value labels
    double labelPointSize() const;
    // Return font point size for titles
    void setTitlePointSize(double value);
    // Return font point size for titles
    double titlePointSize() const;
    // Return text z scaling factor
    double textZScale() const;
    // Set whether axis text labels are drawn flat in 3D views
    void setFlatLabelsIn3D(bool flat);
    // Whether axis text labels are drawn flat in 3D views
    bool flatLabelsIn3D() const;
};
