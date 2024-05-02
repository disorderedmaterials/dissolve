// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/view.h"
#include "gui/render/fontInstance.h"
#include "gui/render/renderable.h"
#include "math/cuboid.h"
#include <algorithm>
#include <cmath>

// Static Members
const double View::defaultZTranslation_ = -10.0;

View::View(const std::vector<std::shared_ptr<Renderable>> &renderables, FontInstance &fontInstance)
    : fontInstance_(fontInstance), renderables_(renderables), axes_(*this, fontInstance)
{
    clear();
}

View::~View() {}

// Clear view, resetting to defaults
void View::clear()
{
    // Geometry / position
    xOffset_ = 0;
    yOffset_ = 0;
    xScale_ = 1.0;
    yScale_ = 1.0;
    aspectRatio_ = 1.0;

    // Role
    viewType_ = View::AutoStretchedView;
    linkedView_ = nullptr;
    autoFollowType_ = View::NoAutoFollow;
    autoFollowXLength_ = 20.0;

    // Projection / view
    hasPerspective_ = false;
    perspectiveFieldOfView_ = 20.0;
    viewportMatrix_[0] = 0;
    viewportMatrix_[1] = 0;
    viewportMatrix_[2] = 0;
    viewportMatrix_[3] = 0;
    viewportVersion_ = 0;
    viewTranslation_.set(0.0, 0.0, defaultZTranslation_);
    viewViewportUsedAt_ = -1;
    viewAxesUsedAt_ = -1;
    viewRotationPoint_ = 0;
    viewRotation_.setIdentity();
    viewRotationInversePoint_ = -1;
    updateViewMatrix();

    // Style
    labelPointSize_ = 16.0;
    titlePointSize_ = 18.0;
    textZScale_ = -1.0;
    flatLabelsIn3D_ = false;
}

/*
 * Geometry
 */

// Set pixel offsets and scales for view
void View::setOffsetAndScale(int xOffset, int yOffset, double xScale, double yScale)
{
    xOffset_ = xOffset;
    yOffset_ = yOffset;
    xScale_ = xScale;
    yScale_ = yScale;
}

// Recalculate viewport matrix based on pixel dimensions provided
void View::recalculateViewport(int width, int height)
{
    viewportMatrix_[0] = xOffset_;
    viewportMatrix_[1] = yOffset_;
    viewportMatrix_[2] = width * xScale_;
    viewportMatrix_[3] = height * yScale_;
    aspectRatio_ = double(viewportMatrix_[2]) / double(viewportMatrix_[3]);
    ++viewportVersion_;

    // Recalculate projection matrix
    if ((viewType_ == View::NormalView) || (viewType_ == View::AutoStretchedView))
    {
        projectionMatrix_ = calculateProjectionMatrix(hasPerspective_, defaultZTranslation_);
    }
    else
        projectionMatrix_ = calculateProjectionMatrix(false, defaultZTranslation_);

    calculateFontScaling();

    // Recalculate view matrix
    updateViewMatrix();

    recalculateView(true);
}

// Translate viewport by specified pixel amounts
void View::translateViewport(int deltaX, int deltaY)
{
    viewportMatrix_[0] += deltaX;
    viewportMatrix_[1] += deltaY;
}

// Return viewport matrix
const GLuint *View::viewportMatrix() const { return viewportMatrix_; }

/*
 * Projection / View
 */

// Return enum options for FormatType
EnumOptions<View::ViewType> View::viewTypes()
{
    return EnumOptions<View::ViewType>("ViewType", {{View::NormalView, "Normal"},
                                                    {View::AutoStretchedView, "AutoStretched"},
                                                    {View::FlatXYView, "FlatXY"},
                                                    {View::FlatXZView, "FlatXZ"},
                                                    {View::FlatZYView, "FlatZY"}});
}

// Return enum options for FormatType
EnumOptions<View::AutoFollowType> View::autoFollowTypes()
{
    return EnumOptions<View::AutoFollowType>(
        "AutoFollowType", {{View::NoAutoFollow, "None"}, {View::AllAutoFollow, "All"}, {View::XAutoFollow, "X"}});
}

// Return calculated projection matrix
Matrix4 View::calculateProjectionMatrix(bool hasPerspective, double orthoZoom) const
{
    Matrix4 result;

    GLdouble top, bottom, right, left;
    GLdouble nearClip = 0.5, farClip = 2000.0;

    if (hasPerspective)
    {
        // Use reversed top and bottom values so we get y-axis (0,1,0) pointing up
        top = tan(perspectiveFieldOfView_ / DEGRAD) * 0.5;
        bottom = -top;
        left = -aspectRatio_ * top;
        right = aspectRatio_ * top;
        result.setColumn(0, (nearClip * 2.0) / (right - left), 0.0, 0.0, 0.0);
        result.setColumn(1, 0.0, (nearClip * 2.0) / (top - bottom), 0.0, 0.0);
        result.setColumn(2, (right + left) / (right - left), (top + bottom) / (top - bottom),
                         -(farClip + nearClip) / (farClip - nearClip), -1.0);
        result.setColumn(3, 0.0, 0.0, -(2.0 * nearClip * farClip) / (farClip - nearClip), 0.0);
    }
    else
    {
        top = -tan(perspectiveFieldOfView_ / DEGRAD) * orthoZoom;
        bottom = -top;
        left = -aspectRatio_ * top;
        right = aspectRatio_ * top;

        result.setColumn(0, 2.0 / (right - left), 0.0, 0.0, (right + left) / (right - left));
        result.setColumn(1, 0.0, 2.0 / (top - bottom), 0.0, (top + bottom) / (top - bottom));
        result.setColumn(2, 0.0, 0.0, -1.0 / farClip, 0.0);
        result.setColumn(3, 0.0, 0.0, 0.0, 1.0);
    }

    return result;
}

// Update view matrix
void View::updateViewMatrix()
{
    // Create translation to centre of Axes coordinates
    viewMatrix_.createTranslation(-axes_.coordCentre());

    // Apply rotation matrix about this local centre
    viewMatrix_.preMultiply(viewRotation_);

    // Apply translation to apply view shift and zoom (the latter only if using perspective)
    viewMatrix_.applyPreTranslation(viewTranslation_.x, viewTranslation_.y, hasPerspective_ ? viewTranslation_.z : 0.0);

    // Calculate inverse
    viewMatrixInverse_ = viewMatrix_;
    viewMatrixInverse_.invert();
}

// Set view type
void View::setViewType(View::ViewType vt)
{
    viewType_ = vt;

    // Forcibly turn off perspective if this is a flat view
    if ((viewType_ >= View::FlatXYView) && (viewType_ <= View::FlatZYView))
        setHasPerspective(false);

    showAllData();

    resetViewMatrix();

    updateViewMatrix();

    ++viewRotationPoint_;
}

// Return view type
View::ViewType View::viewType() const { return viewType_; }

// Set linked View, if any
void View::setLinkedView(View *linkedView) { linkedView_ = linkedView; }

// Return linked View, if any
View *View::linkedView() const { return linkedView_; }

// Return whether view type is flat
bool View::isFlatView() const { return ((viewType_ >= View::FlatXYView) && (viewType_ <= View::FlatZYView)); }

// Return projection matrix
Matrix4 View::projectionMatrix() const { return projectionMatrix_; }

// Set whether the view uses perspective
void View::setHasPerspective(bool perspective)
{
    hasPerspective_ = perspective;

    projectionMatrix_ = calculateProjectionMatrix(hasPerspective_, viewTranslation_.z);

    calculateFontScaling();
}

// Return whether the view uses perspective
bool View::hasPerspective() const { return hasPerspective_; }

// Update transformation (view) matrix
void View::setViewRotation(Matrix4 &mat)
{
    // If this is a two-dimensional or linked view, ignore the request
    if ((viewType_ != View::NormalView) && (viewType_ != View::AutoStretchedView))
        return;

    viewRotation_ = mat;

    updateViewMatrix();

    ++viewRotationPoint_;
}

// Update single column of view matrix
void View::setViewRotationColumn(int column, double x, double y, double z)
{
    // If this is a two-dimensional or linked view, ignore the request
    if ((viewType_ != View::NormalView) && (viewType_ != View::AutoStretchedView))
        return;

    viewRotation_.setColumn(column, x, y, z, 0.0);

    updateViewMatrix();

    ++viewRotationPoint_;
}

// Rotate view matrix about x and y by amounts specified
void View::rotateView(double dx, double dy)
{
    // If this is a two-dimensional or linked view, ignore the request
    if ((viewType_ != View::NormalView) && (viewType_ != View::AutoStretchedView))
        return;

    Matrix4 A;
    A.createRotationXY(dx, dy); // TODO Create Matrix4::applyPreRotationXY() function.
    viewRotation_.preMultiply(A);

    updateViewMatrix();

    ++viewRotationPoint_;
}

// Return rotation matrix
const Matrix4 &View::viewRotation() const { return viewRotation_; }

// Return view rotation inverse
Matrix4 View::viewRotationInverse()
{
    if (viewRotationPoint_ != viewRotationInversePoint_)
    {
        viewRotationInverse_ = viewRotation_;
        viewRotationInverse_.invert();

        viewRotationInversePoint_ = viewRotationPoint_;
    }

    return viewRotationInverse_;
}

// Set view translation
void View::setViewTranslation(double x, double y, double z)
{
    viewTranslation_.set(x, y, z);

    if (!hasPerspective_)
        projectionMatrix_ = calculateProjectionMatrix(false, viewTranslation_.z);

    updateViewMatrix();

    calculateFontScaling();
}

// Translate view matrix by amounts specified
void View::translateView(double dx, double dy, double dz)
{
    // If this is a two-dimensional view, ignore the request
    if ((viewType_ != View::NormalView) && (viewType_ != View::AutoStretchedView))
        return;

    viewTranslation_.add(dx, dy, dz);
    if ((!hasPerspective_) && (fabs(dz) > 1.0e-4))
        projectionMatrix_ = calculateProjectionMatrix(false, viewTranslation_.z);

    updateViewMatrix();

    calculateFontScaling();
}

// Return current view translation
Vec3<double> View::viewTranslation() const { return viewTranslation_; }

// Return view matrix
const Matrix4 &View::viewMatrix() const { return viewMatrix_; }

// Project given data coordinates into world coordinates
Vec3<double> View::dataToWorld(Vec3<double> r) const
{
    Matrix4 vmat;
    Vec4<double> pos, temp;

    // Get the world coordinates of r - multiply by view matrix
    pos.set(r, 1.0);
    temp = viewMatrix_ * pos;

    return Vec3<double>(temp.x, temp.y, temp.z);
}

// Project given data coordinates into screen coordinates
Vec3<double> View::dataToScreen(Vec3<double> r) const
{
    Vec4<double> screenr, tempscreen;
    Vec4<double> worldr;
    Matrix4 vmat;
    Vec4<double> pos;

    // Projection formula is : worldr = P x M x r
    pos.set(r, 1.0);

    // Get the world coordinates of the point - Multiply by view matrix
    worldr = viewMatrix_ * pos;
    screenr = projectionMatrix_ * worldr;
    screenr.x /= screenr.w;
    screenr.y /= screenr.w;
    screenr.x = viewportMatrix_[0] + viewportMatrix_[2] * (screenr.x + 1) * 0.5;
    screenr.y = viewportMatrix_[1] + viewportMatrix_[3] * (screenr.y + 1) * 0.5;
    screenr.z = screenr.z / screenr.w;

    return Vec3<double>(screenr.x, screenr.y, screenr.z);
}

// Project given data coordinates into screen coordinates, with corresponding distance 'delta' in data
Vec3<double> View::dataToScreen(Vec3<double> r, double &lengthScale) const
{
    Vec4<double> screenr;
    Vec4<double> worldr;
    Matrix4 vmat;
    Vec4<double> pos;

    // Projection formula is : worldr = P x M x r
    pos.set(r, 1.0);

    // Get the world coordinates of the point - Multiply by view matrix
    worldr = viewMatrix_ * pos;
    screenr = projectionMatrix_ * worldr;
    screenr.x /= screenr.w;
    screenr.y /= screenr.w;
    screenr.x = viewportMatrix_[0] + viewportMatrix_[2] * (screenr.x + 1) * 0.5;
    screenr.y = viewportMatrix_[1] + viewportMatrix_[3] * (screenr.y + 1) * 0.5;
    screenr.z = screenr.z / screenr.w;

    // Calculate 2D lengthscale around the point - multiply world[x+lengthScale] coordinates by P
    worldr.x += lengthScale;
    Vec4<double> tempScreen = projectionMatrix_ * worldr;
    tempScreen.x /= tempScreen.w;
    lengthScale = fabs((viewportMatrix_[0] + viewportMatrix_[2] * (tempScreen.x + 1) * 0.5) - screenr.x);

    return Vec3<double>(screenr.x, screenr.y, screenr.z);
}

// Project given data coordinates into screen coordinates using supplied projection matrix, rotation matrix and translation
// vector
Vec3<double> View::dataToScreen(Vec3<double> r, Matrix4 projectionMatrix, Matrix4 rotationMatrix,
                                Vec3<double> translation) const
{
    Vec4<double> screenr, tempscreen;
    Vec4<double> worldr;
    Matrix4 vmat;
    Vec4<double> pos;

    // Projection formula is : worldr = P x M x r
    pos.set(r, 1.0);

    // Get the screen coordinates of the point
    vmat = rotationMatrix;
    vmat.applyPreTranslation(translation);
    worldr = vmat * pos;
    screenr = projectionMatrix * worldr;
    screenr.x /= screenr.w;
    screenr.y /= screenr.w;
    screenr.x = viewportMatrix_[0] + viewportMatrix_[2] * (screenr.x + 1) * 0.5;
    screenr.y = viewportMatrix_[1] + viewportMatrix_[3] * (screenr.y + 1) * 0.5;
    screenr.z = screenr.z / screenr.w;

    return Vec3<double>(screenr.x, screenr.y, screenr.z);
}

// Return z translation necessary to display coordinates supplied, assuming the identity view matrix
double View::calculateRequiredZoom(double xMax, double yMax, double fraction) const
{
    // The supplied x and y extents should indicate the number of units in those directions
    // from the origin that are to be displaye on-screen. The 'fraction' indicates how much of the
    // available range on-screen to use, allowing a margin to be added. A value of '1.0' would
    // put the extent with the highest units on the very edge of the display.

    Matrix4 viewMatrix, projectionMatrix = calculateProjectionMatrix(hasPerspective_, viewTranslation_.z);
    Vec4<double> rScreen, rWorld, rModel(xMax, yMax, 0.0, 1.0);
    Vec3<double> translation(0.0, 0.0, -1.0);

    // Sanity check
    if (viewportMatrix_[2] == 0)
        return 1.0;
    if (viewportMatrix_[3] == 0)
        return 1.0;

    // Calculate target screen coordinate
    auto targetX = viewportMatrix_[0] + (1.0 + fraction) * viewportMatrix_[2] * 0.5;
    auto targetY = viewportMatrix_[1] + (1.0 + fraction) * viewportMatrix_[3] * 0.5;

    auto count = 0;
    do
    {
        // If not using perspective, must recalculate the projection matrix
        if (!hasPerspective_)
            projectionMatrix = calculateProjectionMatrix(false, translation.z);

        // Project the point : worldr = P x M x modelr
        viewMatrix.setIdentity();
        viewMatrix.applyPreTranslation(translation);
        rWorld = viewMatrix * rModel;
        rScreen = projectionMatrix * rWorld;
        rScreen.x /= rScreen.w;
        rScreen.y /= rScreen.w;
        rScreen.x = viewportMatrix_[0] + viewportMatrix_[2] * (rScreen.x + 1) * 0.5;
        rScreen.y = viewportMatrix_[1] + viewportMatrix_[3] * (rScreen.y + 1) * 0.5;
        rScreen.z = rScreen.z / rScreen.w;

        // Increase zoom distance
        translation.z -= std::max(std::max(rScreen.x / targetX, rScreen.y / targetY), 1.0);

        // Limit the number of iterations so we can never get into an infinite loop
        if (++count == 1000)
            break;

    } while ((rScreen.x > targetX) || (rScreen.y > targetY));

    return translation.z;
}

// Convert screen coordinates into data space coordinates
Vec3<double> View::screenToData(int x, int y, double z) const
{
    Vec4<double> temp, worldr;
    int newx, newy;
    double dx, dy;

    // Project points at guide z-position and two other points along literal x and y to get scaling factors for screen
    // coordinates
    worldr.set(0.0, 0.0, z, 1.0);
    temp = projectionMatrix_ * worldr;
    newx = viewportMatrix_[0] + viewportMatrix_[2] * (temp.x / temp.w + 1.0) * 0.5;
    newy = viewportMatrix_[1] + viewportMatrix_[3] * (temp.y / temp.w + 1.0) * 0.5;

    for (auto n = 0; n < 10; ++n)
    {
        // Determine new (better) coordinate from a yardstick centred at current world coordinates
        temp = projectionMatrix_ * Vec4<double>(worldr.x + 1.0, worldr.y + 1.0, worldr.z, worldr.w);
        dx = viewportMatrix_[0] + viewportMatrix_[2] * (temp.x / temp.w + 1.0) * 0.5 - newx;
        dy = viewportMatrix_[1] + viewportMatrix_[3] * (temp.y / temp.w + 1.0) * 0.5 - newy;

        worldr.add((x - newx) / dx, (y - newy) / dy, 0.0, 0.0);
        temp = projectionMatrix_ * worldr;
        newx = viewportMatrix_[0] + viewportMatrix_[2] * (temp.x / temp.w + 1.0) * 0.5;
        newy = viewportMatrix_[1] + viewportMatrix_[3] * (temp.y / temp.w + 1.0) * 0.5;
        if ((x == newx) && (y == newy))
            break;
    }

    // Finally, invert to model coordinates
    return viewMatrixInverse_ * Vec3<double>(worldr.x, worldr.y, worldr.z);
}

// Calculate selection axis coordinate from supplied screen coordinates
double View::screenToAxis(int axis, int x, int y, bool clamp) const
{
    // Check for a valid axis
    if (axis == -1)
        return 0.0;

    // Project axis coordinates to get a screen-based yardstick
    auto axmin = dataToScreen(axes_.coordMin(axis));
    auto axmax = dataToScreen(axes_.coordMax(axis));

    // Calculate vectors between axis minimum and mouse position (AM) and axis maximum (AB)
    Vec3<double> ab(axmax.x - axmin.x, axmax.y - axmin.y, 0.0);
    Vec3<double> am(x - axmin.x, y - axmin.y, 0.0);
    auto amNorm = am, abNorm = ab;
    auto ratio = am.magnitude() / ab.magnitude();
    abNorm.normalise();
    amNorm.normalise();

    // Calculate slice axis value - no need to account for inverted axes here, since this is accounted for in the vectors
    // axmin and axmax
    double axisValue;
    if (axes_.logarithmic(axis))
        axisValue =
            pow(10, abNorm.dp(amNorm) * ratio * (log10(axes_.max(axis)) - log10(axes_.min(axis))) + log10(axes_.min(axis)));
    else
        axisValue = abNorm.dp(amNorm) * ratio * (axes_.max(axis) - axes_.min(axis)) + axes_.min(axis);

    // Clamp value to data range
    if (clamp)
    {
        if (axisValue < axes_.min(axis))
            axisValue = axes_.min(axis);
        else if (axisValue > axes_.max(axis))
            axisValue = axes_.max(axis);
    }

    return axisValue;
}

// Recalculate current view parameters (e.g. for 2D, autoStretched 3D etc.)
void View::recalculateView(bool force)
{
    // Check viewport size to try and avoid nan errors
    if ((viewportMatrix_[2] == 0) || (viewportMatrix_[3] == 0))
        return;

    // If the view is neither flat nor autoStretched, there is nothing to do here...
    if (viewType_ == View::NormalView)
        return;

    // If we are already up-to-date (w.r.t. the associated axes) then we can also return now
    auto upToDate = true;
    if (force)
        upToDate = false;
    else if (viewAxesUsedAt_ != axes().version())
        upToDate = false;
    else if (viewViewportUsedAt_ != viewportVersion_)
        upToDate = false;

    if (upToDate)
        return;

    int axis;

    // Calculate ourselves a 'standard' projection matrix
    if (viewType_ == View::AutoStretchedView)
        projectionMatrix_ = calculateProjectionMatrix(hasPerspective_, viewTranslation_.z);
    else
        projectionMatrix_ = calculateProjectionMatrix(false, defaultZTranslation_);

    // Create a temporary, orthographic projection matrix
    Matrix4 tempProjection = calculateProjectionMatrix(false, defaultZTranslation_);

    /*
     * To begin, set the stretch factors to our best first estimate, dividing our width by the range of the axes
     * Doing this first will allow us to get much better values for the pixel overlaps we need later on
     */

    // -- Project a point one unit each along X and Y and subtract off the viewport centre coordinate in order to get
    // literal 'pixels per unit' for (screen) X and Y
    auto unit = dataToScreen(Vec3<double>(1.0, 1.0, 0.0), tempProjection, Matrix4());
    unit.x -= viewportMatrix_[0] + viewportMatrix_[2] / 2.0;
    unit.y -= viewportMatrix_[1] + viewportMatrix_[3] / 2.0;
    unit.z = unit.y;

    // Get axis min/max, accounting for logarithmic axes
    Vec3<double> axisMin, axisMax;
    for (axis = 0; axis < 3; ++axis)
    {
        axisMin[axis] = axes_.logarithmic(axis) ? log10(axes_.min(axis)) : axes_.min(axis);
        axisMax[axis] = axes_.logarithmic(axis) ? log10(axes_.max(axis)) : axes_.max(axis);
    }

    // Decide how we will set stretch factors for each axis (initially set to standard xyy)
    auto axisX = 0, axisY = 1;
    Vec3<int> axisDir(0, 1, 1);
    if (viewType_ == View::FlatXZView)
        axisY = 2;
    else if (viewType_ == View::FlatZYView)
    {
        axisDir.set(1, 1, 0);
        axisX = 2;
    }

    // Set axis stretch factors to fill available pixel width/height
    for (axis = 0; axis < 3; ++axis)
    {
        axes_.setStretch(axis, viewportMatrix_[axisDir[axis] + 2] / (unit[axisDir[axis]] * (axes_.realRange(axis))));
        if (!std::isnormal(axes_.stretch(axis)))
            axes_.setStretch(axis, 1.0);
    }

    const auto margin = 10.0 * fontInstance_.scaleFactor();
    Matrix4 viewMat, B, viewMatrixInverse;
    double tempMin, tempMax;
    Vec3<double> coordMin[3], coordMax[3], labelMin, labelMax, a, b, globalMin, globalMax;

    // Iterate for a few cycles
    for (auto cycle = 0; cycle < 5; ++cycle)
    {
        // We will now calculate more accurate stretch factors to apply to the X and Y axes.
        // Project the axis limits on to the screen using the relevant viewmatrix + coordinate centre translation
        viewMat.createTranslation(-axes().coordCentre());
        if (viewType_ == View::FlatXZView)
            viewMat.applyPreRotationX(90.0);
        else if (viewType_ == View::FlatZYView)
            viewMat.applyPreRotationY(-90.0);

        // Calculate view rotation matrix inverse
        viewMatrixInverse = viewMat;
        viewMatrixInverse.removeTranslationAndScaling();
        viewMatrixInverse.invert();

        // Calculate coordinates and global extremes over axes and labels
        globalMin.set(1e9, 1e9, 1e9);
        globalMax = -globalMin;
        labelMin = globalMin;
        labelMax = -labelMin;
        for (axis = 0; axis < 3; ++axis)
        {
            // Skip third (i.e. 'z') axis
            if ((axis != axisX) && (axis != axisY))
                continue;

            // Project axis min/max coordinates onto screen
            a = dataToScreen(axes_.coordMin(axis), tempProjection, viewMat);
            b = dataToScreen(axes_.coordMax(axis), tempProjection, viewMat);
            coordMin[axis].set(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
            coordMax[axis].set(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));

            // Update global min/max
            for (auto n = 0; n < 3; ++n)
            {
                if (coordMin[axis][n] < globalMin[n])
                    globalMin[n] = coordMin[axis][n];
                if (coordMax[axis][n] > globalMax[n])
                    globalMax[n] = coordMax[axis][n];
            }

            // Get bounding cuboid for axis text
            Cuboid cuboid;
            cuboid = axes_.labelPrimitive(axis).boundingCuboid(fontInstance_, viewMatrixInverse, textZScale_);
            cuboid = axes_.titlePrimitive(axis).boundingCuboid(fontInstance_, viewMatrixInverse, textZScale_, cuboid);

            // Project cuboid extremes and store projected coordinates
            a = dataToScreen(cuboid.minima(), tempProjection, viewMat);
            b = dataToScreen(cuboid.maxima(), tempProjection, viewMat);

            // Update global and label min/max
            for (auto n = 0; n < 3; ++n)
            {
                tempMin = std::min(a[n], b[n]);
                tempMax = std::max(a[n], b[n]);
                if (tempMin < globalMin[n])
                    globalMin[n] = tempMin;
                if (tempMax > globalMax[n])
                    globalMax[n] = tempMax;
                if (tempMin < labelMin[n])
                    labelMin[n] = tempMin;
                if (tempMax > labelMax[n])
                    labelMax[n] = tempMax;
            }
        }

        // Now have screen coordinates of all necessary objects (axes and labels)
        // Calculate total width and height of objects as they are arranged
        double globalWidth = globalMax.x - globalMin.x;
        double globalHeight = globalMax.y - globalMin.y;
        axisPixelLength_[axisX] = coordMax[axisX].x - coordMin[axisX].x;
        axisPixelLength_[axisY] = coordMax[axisY].y - coordMin[axisY].y;

        // Now, we know the width and height of the axis on its own, and the extra 'added' by the labels, so work out
        // how much we need to shrink the axis by
        double deltaWidth = (viewportMatrix_[2] - 2 * margin) - globalWidth;
        double deltaHeight = (viewportMatrix_[3] - 2 * margin) - globalHeight;

        // So, need to lose deltaWidth and deltaHeight pixels from the axis exents - we'll do this by scaling the
        // stretchfactor
        double factor = axisPixelLength_[axisX] / (axisPixelLength_[axisX] - deltaWidth);
        axes_.setStretch(axisX, axes_.stretch(axisX) * factor);
        factor = axisPixelLength_[axisY] / (axisPixelLength_[axisY] - deltaHeight);
        axes_.setStretch(axisY, axes_.stretch(axisY) * factor);
    }

    // Set new rotation matrix and translation vector (if not AutoStretchedView)
    if (viewType_ > View::AutoStretchedView)
    {
        viewRotation_.setIdentity();
        if (viewType_ == View::FlatXZView)
            viewRotation_.applyPreRotationX(90.0);
        else if (viewType_ == View::FlatZYView)
            viewRotation_.applyPreRotationY(-90.0);

        // Set a translation in order to set the margins as requested
        // The viewTranslation_ is applied in 'normal' coordinate axes, so viewTranslation_.x is along screen x etc.
        viewTranslation_.zero();
        viewTranslation_[0] = (margin - (globalMin.x - viewportMatrix_[0])) / unit.x;
        viewTranslation_[1] = (margin - (globalMin.y - viewportMatrix_[1])) / unit.y;
    }

    // Recalculate font scaling
    calculateFontScaling();

    // Recalculate view matrix (translation may have changed)
    updateViewMatrix();

    // Store new versions of view
    viewAxesUsedAt_ = axes().version();
    viewViewportUsedAt_ = viewportVersion_;
}

// Reset view
void View::resetViewMatrix()
{
    // Reset view matrix only if a normal or autostretched view
    if (viewType_ <= View::AutoStretchedView)
    {
        viewRotation_.setIdentity();
        viewTranslation_.set(0.0, 0.0, 0.0);

        // Calculate zoom to show all data
        viewTranslation_.z = calculateRequiredZoom(axes_.realRange(0) * 0.5 * axes_.stretch(0),
                                                   axes_.realRange(1) * 0.5 * axes_.stretch(1), 0.9);

        // Recalculate projection matrix
        projectionMatrix_ = calculateProjectionMatrix(hasPerspective_, viewTranslation_.z);
    }
    else
    {
        // Recalculate projection matrix
        projectionMatrix_ = calculateProjectionMatrix(false, viewTranslation_.z);
    }

    updateViewMatrix();

    calculateFontScaling();

    ++viewRotationPoint_;
}

// Set display limits to show all available data
void View::showAllData(double xFrac, double yFrac, double zFrac)
{
    updateAxisLimits(xFrac, yFrac, zFrac);

    // Set axes limits to the extreme data values, making sure we have a sensible (i.e. non-zero range)
    for (auto axis = 0; axis < 3; ++axis)
    {
        // Grab axis limits and make sure the limits are sensible, expanding only if the range is zero
        double limitMin = axes_.limitMin(axis);
        double limitMax = axes_.limitMax(axis);
        Axes::ensureSensibleRange(limitMin, limitMax);

        axes_.setRange(axis, limitMin, limitMax);
    }
}

// Zoom to specified region
void View::zoomTo(Vec3<double> limit1, Vec3<double> limit2)
{
    // The provided limits should be in local axis coordinates, and may not be in min/max order
    Vec3<double> newMin(std::min(limit1.x, limit2.x), std::min(limit1.y, limit2.y), std::min(limit1.z, limit2.z));
    Vec3<double> newMax(std::max(limit1.x, limit2.x), std::max(limit1.y, limit2.y), std::max(limit1.z, limit2.z));

    // Check the view type and set relevant coordinates
    if (isFlatView())
    {
        auto axisX = 0, axisY = 1;
        if (viewType_ == View::FlatXZView)
            axisY = 2;
        else if (viewType_ == View::FlatZYView)
            axisX = 2;
        axes_.setRange(axisX, newMin.get(axisX), newMax.get(axisX));
        axes_.setRange(axisY, newMin.get(axisY), newMax.get(axisY));
    }
    else
    {
        // 3D view, so set all three axes
        for (auto axis = 0; axis < 3; ++axis)
        {
            axes_.setMin(axis, newMin.get(axis));
            axes_.setMax(axis, newMax.get(axis));
        }
    }
}

// Scale the currently displayed range
void View::scaleRange(double factor)
{
    // Set the axis to skip (if any)
    auto skipAxis = -1;
    if (viewType_ == View::FlatXYView)
        skipAxis = 2;
    else if (viewType_ == View::FlatXZView)
        skipAxis = 1;
    else if (viewType_ == View::FlatZYView)
        skipAxis = 0;

    // Loop over axes
    for (auto axis = 0; axis < 3; ++axis)
    {
        if (axis == skipAxis)
            continue;

        // Get current range of axis (either the real or logged values stored)
        double halfRange = 0.5 * (axes_.max(axis) - axes_.min(axis));
        double mid = axes_.min(axis) + halfRange;
        halfRange *= factor;
        axes_.setMin(axis, mid - halfRange);
        axes_.setMax(axis, mid + halfRange);
    }
}

// Centre 2D view at specified coordinates, optionally moving only by a fraction of the distance required
void View::centre2DAt(Vec3<double> centre, double fraction)
{
    // Get delta distance
    const auto delta = (centre - axes_.centre()) * fraction;

    // Add to current axis limits
    axes_.setRange(0, axes_.min(0) + delta.x, axes_.max(0) + delta.x);
    axes_.setRange(1, axes_.min(1) + delta.y, axes_.max(1) + delta.y);
    axes_.setRange(2, axes_.min(2) + delta.z, axes_.max(2) + delta.z);
}

// Set auto-follow type in effect
void View::setAutoFollowType(AutoFollowType aft)
{
    autoFollowType_ = aft;

    autoFollowTransformVersions_.clear();
}

// Cycle auto-follow type in effect
void View::cycleAutoFollowType() { autoFollowType_ = (View::AutoFollowType)((autoFollowType_ + 1) % nAutoFollowTypes); }

// Auto-follow type in effect
View::AutoFollowType View::autoFollowType() const { return autoFollowType_; }

// Set length of X region to follow, if autoFollowType_ == XFollow
void View::setAutoFollowXLength(double length) { autoFollowXLength_ = length; }

// Return length of X region to follow, if autoFollowType_ == XFollow
double View::autoFollowXLength() const { return autoFollowXLength_; }

// Set axis limits based on current auto-follow type
void View::autoFollowData()
{
    // Is autofollowing enabled?
    if (autoFollowType_ == View::NoAutoFollow)
        return;

    // If renderable data access is disabled, there's nothing to do
    if (!Renderable::sourceDataAccessEnabled())
        return;

    // Only update the axes if one of the renderables transformed data has changed, to prevent needless primitive
    // regeneration further down the line
    auto updateRequired = false;
    for (const auto &rend : renderables_)
    {
        // If the renderable isn't in the list, or our stored version is different, we need to update
        auto it = std::find_if(autoFollowTransformVersions_.begin(), autoFollowTransformVersions_.end(),
                               [rend](auto data) { return data.first == rend; });
        if (it != autoFollowTransformVersions_.end() && (it->second == rend->dataVersion()))
            continue;

        // Update stored transformedData versions
        if (it == autoFollowTransformVersions_.end())
            autoFollowTransformVersions_.emplace_back(rend, rend->valuesTransformDataVersion());
        else
            it->second = rend->valuesTransformDataVersion();

        updateRequired = true;
    }

    if (!updateRequired)
        return;

    if (autoFollowType_ == View::AllAutoFollow)
        showAllData();
    else if (autoFollowType_ == View::XAutoFollow)
    {
        // Establish min / max limits on x axis
        double xMin = dataMinima().x;
        double xMax = dataMaxima().x;
        if ((xMax - xMin) > autoFollowXLength_)
            xMin = xMax - autoFollowXLength_;

        // Get y range over the horizontal range we've established
        auto first = true;
        double yMin = 0.0, yMax = 0.0, yMinTest = 0.0, yMaxTest = 0.0;
        for (const auto &rend : renderables_)
        {
            // Skip this Renderable if it is not currently visible
            if (!rend->isVisible())
                continue;

            // Get y limits for the this data
            if (!rend->yRangeOverX(xMin, xMax, yMinTest, yMaxTest))
                continue;
            if (first)
            {
                yMin = yMinTest;
                yMax = yMaxTest;
                first = false;
            }
            else
            {
                if (yMinTest < yMin)
                    yMin = yMinTest;
                if (yMaxTest > yMax)
                    yMax = yMaxTest;
            }
        }

        // If we didn't have any data to work with, return the current axis limits
        if (first)
        {
            yMax = axes_.limitMax(1);
            yMin = axes_.limitMin(1);
        }
        else
        {
            // Increase the range by 5% either way
            double yDelta = (yMax - yMin) * 0.05;
            yMax += yDelta;
            yMin -= yDelta;
        }

        // Ensure a sensible range for the axes
        Axes::ensureSensibleRange(xMin, xMax);
        Axes::ensureSensibleRange(yMin, yMax);

        // Set new limits
        axes_.setRange(0, xMin, xMax);
        axes_.setRange(1, yMin, yMax);
    }
}

/*
 * Axes
 */

// Return data minima over all displayed renderables
Vec3<double> View::dataMinima()
{
    // If there are no Renderables, just return the current limits
    if (renderables_.empty())
        return Vec3<double>(axes_.limitMin(0), axes_.limitMin(1), axes_.limitMin(2));

    auto nCounted = 0;
    Vec3<double> v, minima;
    for (const auto &rend : renderables_)
    {
        // Skip this Renderable if it is not currently visible
        if (!rend->isVisible())
            continue;

        if (nCounted == 0)
            minima = rend->limitsMin();
        else
        {
            v = rend->limitsMin();
            if (v.x < minima.x)
                minima.x = v.x;
            if (v.y < minima.y)
                minima.y = v.y;
            if (v.z < minima.z)
                minima.z = v.z;
        }
        ++nCounted;
    }

    return minima;
}

// Return data maxima over all displayed renderables
Vec3<double> View::dataMaxima()
{
    // If there are no Renderables, just return the current limits
    if (renderables_.empty())
        return Vec3<double>(axes_.limitMax(0), axes_.limitMax(1), axes_.limitMax(2));

    auto nCounted = 0;
    Vec3<double> v, maxima;
    for (const auto &rend : renderables_)
    {
        // Skip this Renderable if it is not currently visible
        if (!rend->isVisible())
            continue;

        if (nCounted == 0)
            maxima = rend->limitsMax();
        else
        {
            v = rend->limitsMax();
            if (v.x > maxima.x)
                maxima.x = v.x;
            if (v.y > maxima.y)
                maxima.y = v.y;
            if (v.z > maxima.z)
                maxima.z = v.z;
        }
        ++nCounted;
    }

    return maxima;
}

// Return positive data minima over all displayed renderables
Vec3<double> View::positiveDataMinima()
{
    auto nCounted = 0;
    Vec3<double> v, minima;
    for (const auto &rend : renderables_)
    {
        // Skip this Renderable if it is not currently visible
        if (!rend->isVisible())
            continue;

        if (nCounted == 0)
            minima = rend->positiveLimitsMin();
        else
        {
            v = rend->positiveLimitsMin();
            if (v.x < minima.x)
                minima.x = v.x;
            if (v.y < minima.y)
                minima.y = v.y;
            if (v.z < minima.z)
                minima.z = v.z;
        }
        ++nCounted;
    }

    // If we didn't have any data to work with, return the current axis limits
    if (nCounted == 0)
        return Vec3<double>(axes_.limitMin(0), axes_.limitMin(1), axes_.limitMin(2));
    else
        return minima;
}

// Return positive data minima over all displayed renderables
Vec3<double> View::positiveDataMaxima()
{
    auto nCounted = 0;
    Vec3<double> v, maxima;
    for (const auto &rend : renderables_)
    {
        // Skip this Renderable if it is not currently visible
        if (!rend->isVisible())
            continue;

        if (nCounted == 0)
            maxima = rend->positiveLimitsMax();
        else
        {
            v = rend->positiveLimitsMax();
            if (v.x < maxima.x)
                maxima.x = v.x;
            if (v.y < maxima.y)
                maxima.y = v.y;
            if (v.z < maxima.z)
                maxima.z = v.z;
        }
        ++nCounted;
    }

    // If we didn't have any data to work with, return the current axis limits
    if (nCounted == 0)
        return Vec3<double>(axes_.limitMax(0), axes_.limitMax(1), axes_.limitMax(2));
    else
        return maxima;
}

// Update axis limits to represent data extent of associated collections
void View::updateAxisLimits(double xFrac, double yFrac, double zFrac)
{
    // Get transformed data extents
    auto dataMin = dataMinima();
    auto dataMax = dataMaxima();
    auto dataMinPositive = positiveDataMinima();
    auto dataMaxPositive = positiveDataMaxima();

    // The fractional values we've been passed tell us how much of the 'data' to include in the limits
    // A positive value, 0.0 < f < 1.0, tells us to shrink the maximum limit.
    // A negative value, -1.0 < f < 0.0, tells us to increase the minimum limit

    // Store the fractional values in a temporary Vector to make things easier
    Vec3<double> fractions(xFrac, yFrac, zFrac);

    // Loop over axes
    for (auto axis = 0; axis < 3; ++axis)
    {
        // Adjust limits
        if (fractions[axis] > 0.0)
        {
            dataMax[axis] = dataMin[axis] + (dataMax[axis] - dataMin[axis]) * fractions[axis];
            dataMaxPositive[axis] = dataMinPositive[axis] + (dataMaxPositive[axis] - dataMinPositive[axis]) * fractions[axis];
        }
        else
        {
            dataMin[axis] = dataMax[axis] - (dataMin[axis] - dataMax[axis]) * fractions[axis];
            dataMinPositive[axis] = dataMaxPositive[axis] - (dataMinPositive[axis] - dataMaxPositive[axis]) * fractions[axis];
        }

        // Set allowable range to avoid negative numbers if axis is now logarithmic
        if (axes_.logarithmic(axis))
        {
            axes_.setLimitMin(axis, dataMinPositive[axis]);
            axes_.setLimitMax(axis, dataMaxPositive[axis]);
        }
        else
        {
            axes_.setLimitMin(axis, dataMin[axis]);
            axes_.setLimitMax(axis, dataMax[axis]);
        }
    }

    recalculateView();
}

// Shift flat view axis limits by specified amounts
void View::shiftFlatAxisLimits(double deltaH, double deltaV)
{
    // If this is not a flat view, return now
    if (!isFlatView())
        return;

    // Set indices for target axes
    int axes[2];
    axes[0] = 0;
    axes[1] = 1;
    if (viewType_ == View::FlatXZView)
        axes[1] = 2;
    else if (viewType_ == View::FlatZYView)
    {
        axes[0] = 1;
        axes[1] = 2;
    }

    // Loop over axis indices, and set new limits
    double deltas[2];
    deltas[0] = deltaH;
    deltas[1] = deltaV;
    for (auto n = 0; n < 2; ++n)
    {
        double range = axes_.realRange(axes[n]);
        auto logarithmic = axes_.logarithmic(axes[n]);
        double ppUnit = axisPixelLength_[axes[n]] / range;

        // Flip sign of delta if the axis is inverted
        if (axes_.inverted(axes[n]))
            deltas[n] = -deltas[n];

        // Get adjusted min/max values
        double newMin, newMax;
        if (logarithmic)
        {
            newMin = pow(10, axes_.realMin(axes[n]) - deltas[n] / ppUnit);
            newMax = pow(10, axes_.realMax(axes[n]) - deltas[n] / ppUnit);
        }
        else
        {
            newMin = axes_.min(axes[n]) - deltas[n] / ppUnit;
            newMax = axes_.max(axes[n]) - deltas[n] / ppUnit;
        }

        axes_.setMin(axes[n], newMin);
        axes_.setMax(axes[n], newMax);
    }
}

// Shift flat view axis limits by specified fractional amounts
void View::shiftFlatAxisLimitsFractional(double fracH, double fracV)
{
    // Set indices for target axes
    int axes[2];
    axes[0] = 0;
    axes[1] = 1;
    if (viewType_ == View::FlatXZView)
        axes[1] = 2;
    else if (viewType_ == View::FlatZYView)
    {
        axes[0] = 1;
        axes[1] = 2;
    }

    // Determine deltas based on fractional amounts specified
    double deltaH = (axes_.max(axes[0]) - axes_.min(axes[0])) * fracH;
    double deltaV = (axes_.max(axes[1]) - axes_.min(axes[1])) * fracV;

    shiftFlatAxisLimits(deltaH, deltaV);
}

// Return axes for the view
Axes &View::axes() { return axes_; }

const Axes &View::axes() const { return axes_; }

/*
 * Style
 */

// Calculate font scaling factor
void View::calculateFontScaling()
{
    // Calculate text scaling factor
    Vec3<double> translate(0.0, 0.0, viewTranslation_.z);
    if (hasPerspective_)
        translate.z = 0.5;
    auto unit = dataToScreen(Vec3<double>(0.0, 1.0, viewTranslation_.z), projectionMatrix_, Matrix4(), translate);
    unit.y -= viewportMatrix_[1] + viewportMatrix_[3] * 0.5;
    textZScale_ = unit.y;
}

// Set font point size for axis value labels
void View::setLabelPointSize(double value)
{
    labelPointSize_ = value;

    axes_.setPrimitivesInvalid();
}

// Return font point size for axis value labels
double View::labelPointSize() const { return labelPointSize_; }

// Return font point size for titles
void View::setTitlePointSize(double value)
{
    titlePointSize_ = value;

    axes_.setPrimitivesInvalid();
}

// Return font point size for titles
double View::titlePointSize() const { return titlePointSize_; }

// Return text z scaling factor
double View::textZScale() const { return textZScale_; }

// Set whether axis text labels are drawn flat in 3D views
void View::setFlatLabelsIn3D(bool flat)
{
    flatLabelsIn3D_ = flat;

    axes_.setPrimitivesInvalid();
}

// Whether axis text labels are drawn flat in 3D views
bool View::flatLabelsIn3D() const { return flatLabelsIn3D_; }
