/*
	*** View Pane
	*** src/gui/uchroma/classes/viewpane.cpp
	Copyright T. Youngs 2013-2019

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/uchroma/classes/viewpane.h"
#include "gui/uchroma/classes/collection.h"
#include "gui/uchroma/classes/viewlayout.h"
#include "math/cuboid.h"
#include "gui/uchroma/uchromabase.h"
#include <algorithm>
#include <cmath>

// Static Members and constants
template<class ViewPane> RefList<ViewPane,int> ObjectStore<ViewPane>::objects_;
template<class ViewPane> int ObjectStore<ViewPane>::objectCount_;
template<class ViewPane> int ObjectStore<ViewPane>::objectType_ = ObjectInfo::UChromaViewPaneObject;
template<class ViewPane> const char* ObjectStore<ViewPane>::objectTypeName_ = "UChromaViewPane";
const double ViewPane::zOffset_ = -10.0;

// Constructor
ViewPane::ViewPane(ViewLayout& parent) : ListItem<ViewPane>(), ObjectStore<ViewPane>(this), parent_(parent), axes_(*this)
{
	// Geometry / position
	bottomEdge_ = 0;
	leftEdge_ = 0;
	height_ = 0;
	width_ = 0;
	aspectRatio_ = 1.0;

	// Projection / view
	hasPerspective_ = false;
	perspectiveFieldOfView_ = 20.0;
	viewportMatrix_[0] = 0;
	viewportMatrix_[1] = 0;
	viewportMatrix_[2] = 0;
	viewportMatrix_[3] = 0;
	viewportVersion_ = 0;
	viewTranslation_.set(0.0, 0.0, zOffset_);
	viewViewportUsedAt_ = -1;
	viewAxesUsedAt_ = -1;
	viewRotationPoint_ = 0;
	viewRotationInversePoint_ = -1;

	// Role
	role_ = ViewPane::StandardRole;
	viewType_ = ViewPane::AutoStretchedView;
	autoFollowType_ = ViewPane::NoAutoFollow;

	// Style
	boundingBox_ = ViewPane::NoBox;
	boundingBoxPlaneY_ = 0.0;
	labelPointSize_ = 16.0;
	titlePointSize_ = 18.0;
	textZScale_ = -1.0;
	flatLabelsIn3D_ = false;

	// GL
	interactionPrimitive_.setNoInstances();
	interactionBoxPrimitive_.setNoInstances();
	boundingBoxPrimitive_.setNoInstances();
}

// Destructor
ViewPane::~ViewPane()
{
}

// Copy constructor
ViewPane::ViewPane(const ViewPane& source) : ObjectStore<ViewPane>(this), parent_(source.parent()), axes_(*this)
{
	(*this) = source;
}

// Assignment operator
void ViewPane::operator=(const ViewPane& source)
{
	// Geometry / position
	name_ = source.name_;
	bottomEdge_ = source.bottomEdge_;
	leftEdge_ = source.leftEdge_;
	height_ = source.height_;
	width_ = source.width_;

	// Projection / view	
	hasPerspective_ = source.hasPerspective_;
	perspectiveFieldOfView_ = source.perspectiveFieldOfView_;
	viewportMatrix_[0] = source.viewportMatrix_[0];
	viewportMatrix_[1] = source.viewportMatrix_[1];
	viewportMatrix_[2] = source.viewportMatrix_[2];
	viewportMatrix_[3] = source.viewportMatrix_[3];
	viewportVersion_ = 0;
	viewRotation_ = source.viewRotation_;
	viewRotationPoint_ = source.viewRotationPoint_;
	viewRotationInverse_ = source.viewRotationInverse_;
	viewRotationInversePoint_ = source.viewRotationInversePoint_;
	viewTranslation_ = source.viewTranslation_;
	viewViewportUsedAt_ = -1;
	viewAxesUsedAt_ = -1;

	// Collection / Axes
	axes_ = source.axes_;

	// Style
	boundingBox_ = source.boundingBox_;
	boundingBoxPlaneY_ = source.boundingBoxPlaneY_;
	labelPointSize_ = source.labelPointSize_;
	titlePointSize_ = source.titlePointSize_;
	textZScale_ = source.textZScale_;
	flatLabelsIn3D_ = source.flatLabelsIn3D_;

	// Role
	role_ = source.role_;
	viewType_ = source.viewType_;
	// -- Don't perform a literal copy of the paneTargets_. Re-add them instead...
	paneTargets_.clear();
// 	RefList<ViewPane,bool> paneTargets_;	TODO
	// -- Don't perform a literal copy of the collectionTargets_. Re-add them instead...
	collectionTargets_.clear();
	for (TargetData* target = source.collectionTargets_.first(); target != NULL; target = target->next) addCollectionTarget(target->collection());

	// Additional / Generated data
	// -- Should be handled by performing re-addition of target collections above
}

/*
 * Parent
 */

// Return parent
ViewLayout& ViewPane::parent() const
{
	return parent_;
}

// Set as modified (call parent routine)
void ViewPane::paneChanged()
{
	// Pass the modification notification upwards
	parent_.paneChanged(this);
}

/*
 * Position / Geometry
 */

// Set name of pane
void ViewPane::setName(const char* name)
{
	name_ = name;

	paneChanged();
}

// Return name of pane
const char* ViewPane::name() const
{
	return name_.get();
}

// Set bottom left position
void ViewPane::setBottomLeft(int bottom, int left)
{
	bottomEdge_ = bottom;
	leftEdge_ = left;

	paneChanged();
}

// Return bottom edge of pane
int ViewPane::bottomEdge() const
{
	return bottomEdge_;
}

// Return left edge of pane
int ViewPane::leftEdge() const
{
	return leftEdge_;
}

// Set pane size (in terms of columns and rows)
void ViewPane::setSize(int w, int h)
{
	width_ = w;
	height_ = h;

	paneChanged();
}

// Return width of pane (in columns)
int ViewPane::width() const
{
	return width_;
}

// Return height of pane (in rows)
int ViewPane::height() const
{
	return height_;
}

// Move specified handle by specified amount
void ViewPane::moveHandle(PaneHandle handle, int deltaX, int deltaY)
{
	Vec4<int> newGeometry = geometryAfterHandleMove(handle, deltaX, deltaY);
	leftEdge_ = newGeometry.x;
	bottomEdge_ = newGeometry.y;
	width_ = newGeometry.z;
	height_ = newGeometry.w;

	paneChanged();
}

// Return geometry that would result after moving the specified handle
Vec4<int> ViewPane::geometryAfterHandleMove(PaneHandle handle, int deltaX, int deltaY)
{
	// Result contains leftEdge_, bottomEdge_, width_, height_
	Vec4<int> result(leftEdge_, bottomEdge_, width_, height_);
	switch (handle)
	{
		case (ViewPane::BottomLeftHandle):
			if ((deltaX < 0) || ((result.z - deltaX) > 0)) { result.x += deltaX; result.z -= deltaX; }
			if ((deltaY < 0) || ((result.w - deltaY) > 0)) { result.y += deltaY; result.w -= deltaY; }
			break;
		case (ViewPane::BottomMiddleHandle):
			if ((deltaY < 0) || ((result.w - deltaY) > 0)) { result.y += deltaY; result.w -= deltaY; }
			break;
		case (ViewPane::BottomRightHandle):
			if ((deltaX > 0) || ((result.z + deltaX) > 0)) result.z += deltaX;
			if ((deltaY < 0) || ((result.w - deltaY) > 0)) { result.y += deltaY; result.w -= deltaY; }
			break;
		case (ViewPane::MiddleLeftHandle):
			if ((deltaX < 0) || ((result.z - deltaX) > 0)) { result.x += deltaX; result.z -= deltaX; }
			break;
		case (ViewPane::MiddleRightHandle):
			if ((deltaX > 0) || ((result.z + deltaX) > 0)) result.z += deltaX;
			break;
		case (ViewPane::TopLeftHandle):
			if ((deltaX < 0) || ((result.z - deltaX) > 0)) { result.x += deltaX; result.z -= deltaX; }
			if ((deltaY > 0) || ((result.w + deltaY) > 0)) result.w += deltaY;
			break;
		case (ViewPane::TopMiddleHandle):
			if ((deltaY > 0) || ((result.w + deltaY) > 0)) result.w += deltaY;
			break;
		case (ViewPane::TopRightHandle):
			if ((deltaX > 0) || ((result.z + deltaX) > 0)) result.z += deltaX;
			if ((deltaY > 0) || ((result.w + deltaY) > 0)) result.w += deltaY;
			break;
		default:
			Messenger::print("Unhandled PaneHandle in ViewPane::geometryAfterHandleMove().\n");
			break;
	}

	return result;
}

// Recalculate viewport matrix based on grid pixel dimensions provided
void ViewPane::recalculateViewport(int gridPixelWidth, int gridPixelHeight, int nColumns, int nRows, int widthRemainder, int heightRemainder)
{
	viewportMatrix_[0] = gridPixelWidth * leftEdge_;
	viewportMatrix_[1] = gridPixelHeight * bottomEdge_;
	viewportMatrix_[2] = gridPixelWidth * width_;
	viewportMatrix_[3] = gridPixelHeight * height_;
	aspectRatio_ = double(viewportMatrix_[2]) / double(viewportMatrix_[3]);
	++viewportVersion_;

	// Add on the remainder if the top or right edges are extreme
	if ((leftEdge_+width_) == nColumns) viewportMatrix_[2] += widthRemainder;
	if ((bottomEdge_+height_) == nRows) viewportMatrix_[3] += heightRemainder;

	// Recalculate projection matrix
	if ((viewType_ == ViewPane::NormalView) || (viewType_ == ViewPane::AutoStretchedView))
	{
		projectionMatrix_ = calculateProjectionMatrix(hasPerspective_, zOffset_);
	}
	else projectionMatrix_ = calculateProjectionMatrix(false, zOffset_);

	calculateFontScaling();
}

// Translate viewport by specified pixel amounts
void ViewPane::translateViewport(int deltaX, int deltaY)
{
	viewportMatrix_[0] += deltaX;
	viewportMatrix_[1] += deltaY;
}

// Return viewport matrix
const GLuint* ViewPane::viewportMatrix() const
{
	return viewportMatrix_;
}

// Return whether the specified coordinate is in this pane
bool ViewPane::containsCoordinate(int layoutX, int layoutY)
{
	if (layoutX < int(viewportMatrix_[0])) return false;
	if (layoutX > int(viewportMatrix_[0]+viewportMatrix_[2])) return false;
	if (layoutY < int(viewportMatrix_[1])) return false;
	if (layoutY > int(viewportMatrix_[1]+viewportMatrix_[3])) return false;
	return true;
}

// Return whether the specified grid reference is in this pane
bool ViewPane::containsGridReference(int gridX, int gridY)
{
	if (gridX < leftEdge_) return false;
	if (gridX >= (leftEdge_+width_)) return false;
	if (gridY < bottomEdge_) return false;
	if (gridY >= (bottomEdge_+height_)) return false;
	return true;
}

/*
 * Role
 */

// Role of pane
const char* RoleKeywords[ViewPane::nPaneRoles] = { "Extractor", "SliceMonitor", "Standard" };

// Convert text string to PaneRole
ViewPane::PaneRole ViewPane::paneRole(const char* s)
{
	for (int n=0; n<ViewPane::nPaneRoles; ++n) if (DissolveSys::sameString(s, RoleKeywords[n])) return (ViewPane::PaneRole) n;
	return ViewPane::nPaneRoles;
}

// Convert PaneRole to text string
const char* ViewPane::paneRole(ViewPane::PaneRole role)
{
	return RoleKeywords[role];
}

// Set role of this pane
void ViewPane::setRole(ViewPane::PaneRole role)
{
	role_ = role;

	paneChanged();
}

// Return role of this pane
ViewPane::PaneRole ViewPane::role() const
{
	return role_;
}

// Add target pane for role
void ViewPane::addPaneTarget(ViewPane* pane)
{
	paneTargets_.add(pane);

	paneChanged();
}

// Remove target pane for role
void ViewPane::removePaneTarget(ViewPane* pane)
{
	paneTargets_.remove(pane);

	paneChanged();
}

// Return whether specified pane is a target
bool ViewPane::paneIsTarget(ViewPane* pane)
{
	return paneTargets_.contains(pane);
}

// Return target panes for role
RefListItem<ViewPane,bool>* ViewPane::paneTargets()
{
	return paneTargets_.first();
}

// Add target collection for role
void ViewPane::addCollectionTarget(Collection* collection)
{
	TargetData* target = collectionTargets_.add(*this);
	target->initialise(collection);

	if (parent_.uChromaBase().currentEditStateGroup()) parent_.uChromaBase().addEditState(objectInfo(), EditState::ViewPaneAddCollectionTargetQuantity, collection->objectId(), collection->objectId(), -1, -1);

	// Add collection to a group, if it has a group name
	if (collection->hasGroupName()) collectionGroupManager_.addToGroup(collection);

	paneChanged();
}

// Remove target collection for role
void ViewPane::removeCollectionTarget(Collection* collection)
{
	TargetData* target = collectionIsTarget(collection);
	if (!target)
	{
		Messenger::printVerbose("Internal Error: Tried to remove collection '%s' from pane '%s', but it is not a target there.\n", collection->name(), name_.get());
		return;
	}

	if (parent_.uChromaBase().currentEditStateGroup()) parent_.uChromaBase().addEditState(objectInfo(), EditState::ViewPaneRemoveCollectionTargetQuantity, collection->objectId(), collection->objectId(), -1, -1);

	// Remove the target from our list
	collectionTargets_.remove(target);

	// Remove the Collection from its group
	collectionGroupManager_.removeFromGroup(collection);

	paneChanged();
}

// Return whether specified collection is a target
TargetData* ViewPane::collectionIsTarget(Collection* collection)
{
	for (TargetData* target = collectionTargets_.first(); target != NULL; target = target->next) if (target->collection() == collection) return target;
	return NULL;
}

// Return target collections for role
TargetData* ViewPane::collectionTargets()
{
	return collectionTargets_.first();
}

/*
 * Projection / View
 */

// View types
const char* ViewTypeKeywords[ViewPane::nViewTypes] = { "Normal", "AutoStretched", "FlatXY", "FlatXZ", "FlatZY", "Linked" };

// Convert text string to ViewType
ViewPane::ViewType ViewPane::viewType(const char* s)
{
	for (int n=0; n<ViewPane::nViewTypes; ++n) if (DissolveSys::sameString(s, ViewTypeKeywords[n])) return (ViewPane::ViewType) n;
	return ViewPane::nViewTypes;
}

// Convert ViewType to text string
const char* ViewPane::viewType(ViewPane::ViewType vt)
{
	return ViewTypeKeywords[vt];
}

// AutoFollow types
const char* AutoFollowTypeKeywords[ViewPane::nAutoFollowTypes] = { "None", "All", "X" };

// Convert text string to AutoFollowType
ViewPane::AutoFollowType ViewPane::autoFollowType(const char* s)
{
	for (int n=0; n<ViewPane::nAutoFollowTypes; ++n) if (DissolveSys::sameString(s, AutoFollowTypeKeywords[n])) return (ViewPane::AutoFollowType) n;
	return ViewPane::nAutoFollowTypes;
}

// Convert AutoFollowType to text string
const char* ViewPane::autoFollowType(ViewPane::AutoFollowType aft)
{
	return AutoFollowTypeKeywords[aft];
}

// Return calculated projection matrix
Matrix4 ViewPane::calculateProjectionMatrix(bool hasPerspective, double orthoZoom) const
{
	Matrix4 result;

	GLdouble top, bottom, right, left;
	GLdouble nearClip = 0.5, farClip = 2000.0;

	if (hasPerspective)
	{
		// Use reversed top and bottom values so we get y-axis (0,1,0) pointing up
		top = tan(perspectiveFieldOfView_ / DEGRAD) * 0.5;
		bottom = -top;
		left = -aspectRatio_*top;
		right = aspectRatio_*top;
		result.setColumn(0, (nearClip*2.0) / (right-left), 0.0, 0.0, 0.0);
		result.setColumn(1, 0.0, (nearClip*2.0) / (top-bottom), 0.0, 0.0);
		result.setColumn(2, (right+left)/(right-left), (top+bottom)/(top-bottom), -(farClip+nearClip)/(farClip-nearClip), -1.0);
		result.setColumn(3, 0.0, 0.0, -(2.0*nearClip*farClip) / (farClip-nearClip), 0.0);
		// Equivalent to the following code:
		// glMatrixMode(GL_PROJECTION);
		// glLoadIdentity();
		// top = tan(prefs.perspectiveFov() / DEGRAD) * prefs.clipNear();
		// bottom = -top;
		// glFrustum(aspect*bottom, aspect*top, bottom, top, prefs.clipNear(), prefs.clipFar());
		// glGetDoublev(GL_PROJECTION_MATRIX, modelProjectionMatrix_.matrix());
	}
	else
	{
		top = -tan(perspectiveFieldOfView_ / DEGRAD) * orthoZoom;
		bottom = -top;
		left = -aspectRatio_*top;
		right = aspectRatio_*top;

		result.setColumn(0, 2.0 / (right-left), 0.0, 0.0, (right+left)/(right-left));
		result.setColumn(1, 0.0, 2.0 / (top-bottom), 0.0, (top+bottom)/(top-bottom));
		result.setColumn(2, 0.0, 0.0, -1.0/farClip, 0.0);
		result.setColumn(3, 0.0, 0.0, 0.0, 1.0);
		// Equivalent to the following code:
		// glMatrixMode(GL_PROJECTION);
		// glLoadIdentity();
		// top = tan(prefs.perspectiveFov() / DEGRAD) * prefs.clipNear();
		// bottom = -top;
		// glOrtho(aspect*top, aspect*bottom, top, bottom, -prefs.clipFar(), prefs.clipFar());
		// glGetDoublev(GL_PROJECTION_MATRIX, modelProjectionMatrix_.matrix());
	}

	return result;
}

// Set view type
void ViewPane::setViewType(ViewPane::ViewType vt)
{
	viewType_ = vt;

	// Forcibly turn off perspective if this is a flat view
	if ((viewType_ >= ViewPane::FlatXYView) && (viewType_ <= ViewPane::FlatZYView)) setHasPerspective(false);
}

// Return view type
ViewPane::ViewType ViewPane::viewType() const
{
	return viewType_;
}

// Return whether view type is flat
bool ViewPane::isFlatView() const
{
	return ((viewType_ >= ViewPane::FlatXYView) && (viewType_ <= ViewPane::FlatZYView));
}

// Return projection matrix
Matrix4 ViewPane::projectionMatrix() const
{
	return projectionMatrix_;
}

// Set whether this pane uses perspective
void ViewPane::setHasPerspective(bool perspective)
{
	hasPerspective_ = perspective;

	projectionMatrix_ = calculateProjectionMatrix(hasPerspective_, viewTranslation_.z);

	calculateFontScaling();
}

// Return whether this pane uses perspective
bool ViewPane::hasPerspective() const
{
	return hasPerspective_;
}

// Update transformation (view) matrix
void ViewPane::setViewRotation(Matrix4& mat)
{
	// If this is a two-dimensional or linked view, ignore the request
	if ((viewType_ != ViewPane::NormalView) && (viewType_ != ViewPane::AutoStretchedView)) return;

	viewRotation_ = mat;

	++viewRotationPoint_;
}

// Update single column of view matrix
void ViewPane::setViewRotationColumn(int column, double x, double y, double z)
{
	// If this is a two-dimensional or linked view, ignore the request
	if ((viewType_ != ViewPane::NormalView) && (viewType_ != ViewPane::AutoStretchedView)) return;
	
	viewRotation_.setColumn(column, x, y, z, 0.0);

	++viewRotationPoint_;
}

// Rotate view matrix about x and y by amounts specified
void ViewPane::rotateView(double dx, double dy)
{
	// If this is a two-dimensional or linked view, ignore the request
	if ((viewType_ != ViewPane::NormalView) && (viewType_ != ViewPane::AutoStretchedView)) return;

	Matrix4 A;
	A.createRotationXY(dx, dy);	// TODO Create Matrix4::applyPreRotationXY() function.
	viewRotation_.preMultiply(A);

	++viewRotationPoint_;
}

// Return view rotation
Matrix4 ViewPane::viewRotation() const
{
	return viewRotation_;
}

// Return view rotation inverse
Matrix4 ViewPane::viewRotationInverse()
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
void ViewPane::setViewTranslation(double x, double y, double z)
{
	viewTranslation_.set(x, y, z);
	if (!hasPerspective_) projectionMatrix_ = calculateProjectionMatrix(false, viewTranslation_.z);
	calculateFontScaling();
}

// Translate view matrix by amounts specified
void ViewPane::translateView(double dx, double dy, double dz)
{
	// If this is a two-dimensional or linked view, ignore the request
	if ((viewType_ != ViewPane::NormalView) && (viewType_ != ViewPane::AutoStretchedView)) return;

	viewTranslation_.add(dx, dy, dz);
	if ((!hasPerspective_) && (fabs(dz) > 1.0e-4)) projectionMatrix_ = calculateProjectionMatrix(false, viewTranslation_.z);
	calculateFontScaling();
}

// Return current view translation
Vec3<double> ViewPane::viewTranslation() const
{
	return viewTranslation_;
}

// Return full view matrix (rotation + translation)
Matrix4 ViewPane::viewMatrix()
{
	Matrix4 viewMatrix;

	// Apply translation to centre of axes coordinates
	viewMatrix.createTranslation(-axes_.coordCentre());

	// Apply rotation matrix about this local centre
	viewMatrix.preMultiply(viewRotation_);

	// Apply translation to apply view shift and zoom (the latter only if using perspective)
	viewMatrix.applyPreTranslation(viewTranslation_.x, viewTranslation_.y, hasPerspective_ ? viewTranslation_.z : 0.0 );

	return viewMatrix;
}

// Project given model coordinates into world coordinates
Vec3<double> ViewPane::modelToWorld(Vec3<double> modelr)
{
	Vec3<double> worldr;
	Matrix4 vmat;
	Vec4<double> pos, temp;

	// Projection formula is : worldr = P x M x modelr
	pos.set(modelr, 1.0);
	// Get the world coordinates of the atom - Multiply by modelview matrix 'view'
	vmat = viewMatrix();
	temp = vmat * pos;
	worldr.set(temp.x, temp.y, temp.z);

	return worldr;
}

// Project given model coordinates into screen coordinates
Vec3<double> ViewPane::modelToScreen(Vec3<double> modelr)
{
	Vec4<double> screenr, tempscreen;
	Vec4<double> worldr;
	Matrix4 vmat;
	Vec4<double> pos;

	// Projection formula is : worldr = P x M x modelr
	pos.set(modelr, 1.0);

	// Get the world coordinates of the point - Multiply by modelview matrix 'view'
	vmat = viewMatrix();
	worldr = vmat * pos;
	screenr = projectionMatrix_ * worldr;
	screenr.x /= screenr.w;
	screenr.y /= screenr.w;
	screenr.x = viewportMatrix_[0] + viewportMatrix_[2]*(screenr.x+1)*0.5;
	screenr.y = viewportMatrix_[1] + viewportMatrix_[3]*(screenr.y+1)*0.5;
	screenr.z = screenr.z / screenr.w;

	return Vec3<double>(screenr.x, screenr.y, screenr.z);
}

// Project given model coordinates into screen coordinates using supplied projection matrix, rotation matrix and translation vector
Vec3<double> ViewPane::modelToScreen(Vec3<double> modelr, Matrix4 projectionMatrix, Matrix4 rotationMatrix, Vec3<double> translation)
{
	Vec4<double> screenr, tempscreen;
	Vec4<double> worldr;
	Matrix4 vmat;
	Vec4<double> pos;

	// Projection formula is : worldr = P x M x modelr
	pos.set(modelr, 1.0);

	// Get the world coordinates of the point - Multiply by modelview matrix 'view'
	vmat = rotationMatrix;
	vmat.applyPreTranslation(translation);
	worldr = vmat * pos;
	screenr = projectionMatrix * worldr;
	screenr.x /= screenr.w;
	screenr.y /= screenr.w;
	screenr.x = viewportMatrix_[0] + viewportMatrix_[2]*(screenr.x+1)*0.5;
	screenr.y = viewportMatrix_[1] + viewportMatrix_[3]*(screenr.y+1)*0.5;
	screenr.z = screenr.z / screenr.w;

	return Vec3<double>(screenr.x, screenr.y, screenr.z);
}

// Return z translation necessary to display coordinates supplied, assuming the identity view matrix
double ViewPane::calculateRequiredZoom(double xMax, double yMax, double fraction)
{
	// The supplied x and y extents should indicate the number of units in those directions
	// from the origin that are to be displaye on-screen. The 'fraction' indicates how much of the
	// available range on-screen to use, allowing a margin to be added. A value of '1.0' would
	// put the extent with the highest units on the very edge of the display.

	Matrix4 viewMatrix, projectionMatrix = calculateProjectionMatrix(hasPerspective_, zOffset_);
	Vec4<double> rScreen, rWorld, rModel(xMax, yMax, 0.0, 1.0);
	Vec3<double> translation(0.0, 0.0, -1.0);

	// Sanity check
	if (viewportMatrix_[2] == 0) return 1.0;
	if (viewportMatrix_[3] == 0) return 1.0;

	// Calculate target screen coordinate
	int targetX = viewportMatrix_[0] + (1.0 + fraction) * viewportMatrix_[2] * 0.5;
	int targetY = viewportMatrix_[1] + (1.0 + fraction) * viewportMatrix_[3] * 0.5;

	int count = 0;
	do
	{
		// If not using perspective, must recalculate the projection matrix
		if (!hasPerspective_) projectionMatrix = calculateProjectionMatrix(false, translation.z);

		// Project the point : worldr = P x M x modelr
		viewMatrix.setIdentity();
		viewMatrix.applyPreTranslation(translation);
		rWorld = viewMatrix * rModel;
		rScreen = projectionMatrix * rWorld;
		rScreen.x /= rScreen.w;
		rScreen.y /= rScreen.w;
		rScreen.x = viewportMatrix_[0] + viewportMatrix_[2]*(rScreen.x+1)*0.5;
		rScreen.y = viewportMatrix_[1] + viewportMatrix_[3]*(rScreen.y+1)*0.5;
		rScreen.z = rScreen.z / rScreen.w;

		// Increase zoom distance
		translation.z -= std::max( std::max(rScreen.x / targetX, rScreen.y / targetY), 1.0);

		// Limit the number of iterations so we can never get into an infinite loop
		if (++count == 1000) break;

	} while ((rScreen.x > targetX) || (rScreen.y > targetY));

	return translation.z;
}

// Convert screen coordinates into model space coordinates
Vec3<double> ViewPane::screenToModel(int x, int y, double z)
{
	static Vec3<double> modelr;
	Vec4<double> temp, worldr;
	int newx, newy;
	double dx, dy;

	// Grab full transformation matrix (not just rotation matrix) and invert
	Matrix4 itransform = viewMatrix();
	itransform.invert();

	// Project points at guide z-position and two other points along literal x and y to get scaling factors for screen coordinates
	worldr.set(0.0,0.0,z, 1.0);
	temp = projectionMatrix_ * worldr;
	newx = viewportMatrix_[0] + viewportMatrix_[2]*(temp.x / temp.w + 1.0)*0.5;
	newy = viewportMatrix_[1] + viewportMatrix_[3]*(temp.y / temp.w + 1.0)*0.5;

	for (int n=0; n<10; ++n)
	{
		// Determine new (better) coordinate from a yardstick centred at current world coordinates
		temp = projectionMatrix_ * Vec4<double>(worldr.x+1.0, worldr.y+1.0, worldr.z, worldr.w);
		dx = viewportMatrix_[0] + viewportMatrix_[2]*(temp.x / temp.w + 1.0)*0.5 - newx;
		dy = viewportMatrix_[1] + viewportMatrix_[3]*(temp.y / temp.w + 1.0)*0.5 - newy;

		worldr.add((x-newx)/dx, (y-newy)/dy, 0.0, 0.0);
// 		printf ("N=%i", n); worldr.print();
		temp = projectionMatrix_ * worldr;
		newx = viewportMatrix_[0] + viewportMatrix_[2]*(temp.x / temp.w + 1.0)*0.5;
		newy = viewportMatrix_[1] + viewportMatrix_[3]*(temp.y / temp.w + 1.0)*0.5;
// 		printf("NEW dx = %f, dy = %f, wantedxy = %f, %f\n", newx, newy, x, y);
		if ((x == newx) && (y == newy)) break;
	}

	// Finally, invert to model coordinates
	modelr = itransform * Vec3<double>(worldr.x, worldr.y, worldr.z);

	return modelr;
}

// Calculate selection axis coordinate from supplied screen coordinates
double ViewPane::screenToAxis(int axis, int x, int y, bool clamp)
{
	// Check for a valid axis
	if (axis == -1) return 0.0;

// 	printf("Test: min=%f, max=%f\n", min_[0], max_[0]);
// 	rMouseLast_.print();
// 	axisCoordMin_[0].print();
	// Project axis coordinates to get a screen-based yardstick
	Vec3<double> axmin = modelToScreen(axes_.coordMin(axis));
	Vec3<double> axmax = modelToScreen(axes_.coordMax(axis));
// 	axmin.print();
// 	axmax.print();

	// Calculate vectors between axis minimum and mouse position (AM) and axis maximum (AB)
	Vec3<double> ab(axmax.x - axmin.x, axmax.y - axmin.y, 0.0);
	Vec3<double> am(x - axmin.x, y - axmin.y, 0.0);
	Vec3<double> amNorm = am, abNorm = ab;
	double ratio = am.magnitude() / ab.magnitude();
	abNorm.normalise();
	amNorm.normalise();
// 	double angle = acos(abNorm.dp(amNorm));
//	printf("Angle = %f, %f\n", angle, angle * DEGRAD);

	// Calculate slice axis value - no need to account for inverted axes here, since this is accounted for in the vectors axmin and axmax
	double axisValue;
	if (axes_.logarithmic(axis)) axisValue = pow(10, abNorm.dp(amNorm)*ratio * (log10(axes_.max(axis)) - log10(axes_.min(axis))) + log10(axes_.min(axis)));
	else axisValue = abNorm.dp(amNorm)*ratio * (axes_.max(axis) - axes_.min(axis)) + axes_.min(axis);
//	printf("slicevalue = %f (%f)\n", axisValue, abNorm.dp(amNorm)*ratio);

	// Clamp value to data range
	if (clamp)
	{
		if (axisValue < axes_.min(axis)) axisValue = axes_.min(axis);
		else if (axisValue > axes_.max(axis)) axisValue = axes_.max(axis);
	// 	printf("ACMAG = %f, X = %f\n", ratio, axisValue);
	}

	return axisValue;
}

// Recalculate current view parameters (e.g. for 2D, autoStretched 3D etc.)
void ViewPane::recalculateView(bool force)
{
	// Check viewport size to try and avoid nan errors
	if ((viewportMatrix_[2] == 0) || (viewportMatrix_[3] == 0)) return;

	// If the view is neither flat nor autoStretched, there is nothing to do here...
	if (viewType_ == ViewPane::NormalView) return;

	// If we are already up-to-date (w.r.t. the associated axes) then we can also return now
	bool upToDate = true;
	if (force) upToDate = false;
	else if (viewAxesUsedAt_ != axes().axesVersion()) upToDate = false;
	else if (viewViewportUsedAt_ != viewportVersion_) upToDate = false;

	if (upToDate) return;

	int axis;

	// Calculate ourselves a 'standard' projection matrix
	if (viewType_ == ViewPane::AutoStretchedView) projectionMatrix_ = calculateProjectionMatrix(hasPerspective_, viewTranslation_.z);
	else projectionMatrix_ = calculateProjectionMatrix(false, zOffset_);

	// Create a temporary, orthographic projection matrix
	Matrix4 tempProjection = calculateProjectionMatrix(false, zOffset_);

	// To begin, set the stretch factors to our best first estimate, dividing the pane width by the range of the axes
	// Doing this first will allow us to get much better values for the pixel overlaps we need later on
	// -- Project a point one unit each along X and Y and subtract off the viewport centre coordinate in order to get literal 'pixels per unit' for (screen) X and Y 
	Vec3<double> unit = modelToScreen(Vec3<double>(1.0, 1.0, 0.0), tempProjection, Matrix4());
	unit.x -= viewportMatrix_[0] + viewportMatrix_[2]/2.0;
	unit.y -= viewportMatrix_[1] + viewportMatrix_[3]/2.0;
	unit.z = unit.y;

	// Get axis min/max, accounting for logarithmic axes
	Vec3<double> axisMin, axisMax;
	for (axis=0; axis<3; ++axis)
	{
		axisMin[axis] = axes_.logarithmic(axis) ? log10(axes_.min(axis)) : axes_.min(axis);
		axisMax[axis] = axes_.logarithmic(axis) ? log10(axes_.max(axis)) : axes_.max(axis);
	}

	// Decide how we will set stretch factors for each axis (initially set to standard xyy)
	int axisX = 0, axisY = 1;
	Vec3<int> axisDir(0,1,1);
	if (viewType_ == ViewPane::FlatXZView) axisY = 2;
	else if (viewType_ == ViewPane::FlatZYView)
	{
		axisDir.set(1,1,0);
		axisX = 2;
	}

	// Set axis stretch factors to fill available pixel width/height
	for (axis=0; axis<3; ++axis)
	{
		axes_.setStretch(axis, viewportMatrix_[axisDir[axis]+2] / (unit[axisDir[axis]] * (axes_.realRange(axis))));
		if (!std::isnormal(axes_.stretch(axis))) axes_.setStretch(axis, 1.0);
	}
	
	const double margin = 10.0;
	Matrix4 viewMat, B, viewMatrixInverse;
	double tempMin, tempMax;
	Vec3<double> coordMin[3], coordMax[3], labelMin, labelMax, a, b, globalMin, globalMax;
	FontInstance& fontInstance = parent_.uChromaBase().viewer()->fontInstance();

	// Iterate for a few cycles
	for (int cycle = 0; cycle < 5; ++cycle)
	{
		// We will now calculate more accurate stretch factors to apply to the X and Y axes.
		// Project the axis limits on to the screen using the relevant viewmatrix + coordinate centre translation
		viewMat.createTranslation(-axes().coordCentre());
		if (viewType_ == ViewPane::FlatXZView) viewMat.applyPreRotationX(90.0);
		else if (viewType_ == ViewPane::FlatZYView) viewMat.applyPreRotationY(-90.0);

		// Calculate view rotation matrix inverse
		viewMatrixInverse = viewMat;
		viewMatrixInverse.removeTranslationAndScaling();
		viewMatrixInverse.invert();

		// Calculate coordinates and global extremes over axes and labels
		globalMin.set(1e9,1e9,1e9);
		globalMax = -globalMin;
		labelMin = globalMin;
		labelMax = -labelMin;
		for (axis=0; axis<3; ++axis)
		{
			// Skip third (i.e. 'z') axis
			if ((axis != axisX) && (axis != axisY)) continue;

			// Project axis min/max coordinates onto screen
			a = modelToScreen(axes_.coordMin(axis), tempProjection, viewMat);
			b = modelToScreen(axes_.coordMax(axis), tempProjection, viewMat);
			coordMin[axis].set(std::min(a.x,b.x), std::min(a.y,b.y),  std::min(a.z,b.z)); 
			coordMax[axis].set(std::max(a.x,b.x), std::max(a.y,b.y),  std::max(a.z,b.z)); 

			// Update global min/max
			for (int n=0; n<3; ++n)
			{
				if (coordMin[axis][n] < globalMin[n]) globalMin[n] = coordMin[axis][n];
				if (coordMax[axis][n] > globalMax[n]) globalMax[n] = coordMax[axis][n];
			}

			// Get bounding cuboid for axis text
			Cuboid cuboid;
			cuboid = axes_.labelPrimitive(axis).boundingCuboid(fontInstance, viewMatrixInverse, textZScale_);
			cuboid = axes_.titlePrimitive(axis).boundingCuboid(fontInstance, viewMatrixInverse, textZScale_, cuboid);

			// Project cuboid extremes and store projected coordinates
			a = modelToScreen(cuboid.minima(), tempProjection, viewMat);
			b = modelToScreen(cuboid.maxima(), tempProjection, viewMat);

			// Update global and label min/max
			for (int n=0; n<3; ++n)
			{
				tempMin = std::min(a[n],b[n]);
				tempMax = std::max(a[n],b[n]);
				if (tempMin < globalMin[n]) globalMin[n] = tempMin;
				if (tempMax > globalMax[n]) globalMax[n] = tempMax;
				if (tempMin < labelMin[n]) labelMin[n] = tempMin;
				if (tempMax > labelMax[n]) labelMax[n] = tempMax;
			}
		}

		// Now have screen coordinates of all necessary objects (axes and labels)
		// Calculate total width and height of objects as they are arranged
		double globalWidth = globalMax.x - globalMin.x;
		double globalHeight = globalMax.y - globalMin.y;
		axisPixelLength_[axisX] = coordMax[axisX].x - coordMin[axisX].x;
		axisPixelLength_[axisY] = coordMax[axisY].y - coordMin[axisY].y;
// 		double labelWidth = labelMax.x - labelMin.x;
// 		double labelHeight = labelMax.y - labelMin.y;

		// Now, we know the width and height of the axis on its own, and the extra 'added' by the labels, so work out how much we need to shrink the axis by
		double deltaWidth = (viewportMatrix_[2] - 2*margin) - globalWidth;
		double deltaHeight = (viewportMatrix_[3] - 2*margin) - globalHeight;

		// So, need to lose deltaWidth and deltaHeight pixels from the axis exents - we'll do this by scaling the stretchfactor
		double factor = axisPixelLength_[axisX] / (axisPixelLength_[axisX] - deltaWidth);
		axes_.setStretch(axisX, axes_.stretch(axisX) * factor);
		factor = axisPixelLength_[axisY] / (axisPixelLength_[axisY] - deltaHeight);
		axes_.setStretch(axisY, axes_.stretch(axisY) * factor);
	}
	
	// Set new rotation matrix and translation vector (if not AutoStretchedView)
	if (viewType_ > ViewPane::AutoStretchedView)
	{
		viewRotation_.setIdentity();
		if (viewType_ == ViewPane::FlatXZView) viewRotation_.applyPreRotationX(90.0);
		else if (viewType_ == ViewPane::FlatZYView) viewRotation_.applyPreRotationY(-90.0);

		// Set a translation in order to set the margins as requested
		// The viewTranslation_ is applied in 'normal' coordinate axes, so viewTranslation_.x is along screen x etc.
		viewTranslation_.zero();
		viewTranslation_[0] = (margin - (globalMin.x - viewportMatrix_[0])) / unit.x;
		viewTranslation_[1] = (margin - (globalMin.y - viewportMatrix_[1])) / unit.y;
	}

	// Recalculate font scaling
	calculateFontScaling();

	// Store new versions of view
	viewAxesUsedAt_ = axes().axesVersion();
	viewViewportUsedAt_ = viewportVersion_;
}

// Reset view
void ViewPane::resetViewMatrix()
{
	// Reset view matrix only if a normal or autostretched view
	if (viewType_ <= ViewPane::AutoStretchedView)
	{
		viewRotation_.setIdentity();
		viewTranslation_.set(0.0, 0.0, 0.0);

		// If a Normal view, reset the stretch factors
// 		if (viewType_ == ViewPane::NormalView)
// 		{
// 			axes_.setStretch(0, 1.0);
// 			axes_.setStretch(1, 1.0);
// 			axes_.setStretch(2, 1.0);
// 		}

		// Calculate zoom to show all data
		viewTranslation_.z = calculateRequiredZoom(axes_.realRange(0)*0.5*axes_.stretch(0), axes_.realRange(1)*0.5*axes_.stretch(1), 0.9);

		// Recalculate projection matrix
		projectionMatrix_ = calculateProjectionMatrix(hasPerspective_, viewTranslation_.z);
	}
	else
	{
		// Recalculate projection matrix
		projectionMatrix_ = calculateProjectionMatrix(false, zOffset_);
	}

	calculateFontScaling();
}

// Set display limits to show all available data
void ViewPane::showAllData(double xFrac, double yFrac, double zFrac)
{
	updateAxisLimits(xFrac, yFrac, zFrac);

	// Set axes limits to the extreme data values, making sure we have a sensible (i.e. non-zero range)
	for (int axis = 0; axis < 3; ++axis)
	{
		// Grab axis limits and make sure the limits are sensible, expanding only if the range is zero
		double limitMin = axes_.limitMin(axis);
		double limitMax = axes_.limitMax(axis);
		Axes::ensureSensibleRange(limitMin, limitMax, true);

		axes_.setRange(axis, limitMin, limitMax);
// 		axes_.setToLimit(axis, true);
// 		axes_.setToLimit(axis, false);
	}
}

// Zoom to specified region
void ViewPane::zoomTo(Vec3<double> limit1, Vec3<double> limit2)
{
	// The provided limits should be in local axis coordinates, and may not be in min/max order
	Vec3<double> newMin(std::min(limit1.x, limit2.x), std::min(limit1.y, limit2.y), std::min(limit1.z, limit2.z));
	Vec3<double> newMax(std::max(limit1.x, limit2.x), std::max(limit1.y, limit2.y), std::max(limit1.z, limit2.z));

	// Check the view type and set relevant coordinates
	if (isFlatView())
	{
		int axisX = 0, axisY = 1;
		if (viewType_ == ViewPane::FlatXZView) axisY = 2;
		else if (viewType_ == ViewPane::FlatZYView) axisX = 2;
		axes_.setRange(axisX, newMin.get(axisX), newMax.get(axisX));
		axes_.setRange(axisY, newMin.get(axisY), newMax.get(axisY));
	}
	else
	{
		// 3D view, so set all three axes
		for (int axis = 0; axis < 3; ++axis)
		{
			axes_.setMin(axis, newMin.get(axis));
			axes_.setMax(axis, newMax.get(axis));
		}
	}
}

// Set auto-follow type in effect
void ViewPane::setAutoFollowType(AutoFollowType aft)
{
	autoFollowType_ = aft;
}

// Cycle auto-follow type in effect
void ViewPane::cycleAutoFollowType()
{
	autoFollowType_ = (ViewPane::AutoFollowType) ((autoFollowType_+1)%nAutoFollowTypes);
}

// Auto-follow type in effect
ViewPane::AutoFollowType ViewPane::autoFollowType() const
{
	return autoFollowType_;
}

// Set axis limits based on current auto-follow type
void ViewPane::autoFollowData()
{
	if (autoFollowType_ == ViewPane::NoAutoFollow) return;
	else if (autoFollowType_ == ViewPane::AllAutoFollow) showAllData();
	else if (autoFollowType_ == ViewPane::XFollow)
	{
		// This is the window x 'width' we will follow
		const double width = 20.0;

		// Establish min / max limits on x axis
		double xMin = transformedDataMinima().x;
		double xMax = transformedDataMaxima().x;
		if ((xMax - xMin) > width) xMin = xMax - width;

		// Get y range over the horizontal range we've established
		bool first = true;
		double yMin, yMax, yMinTest, yMaxTest;
		for (TargetData* target = collectionTargets_.first(); target != NULL; target = target->next)
		{
			// Loop over display targets
			for (TargetPrimitive* prim = target->displayPrimitives(); prim != NULL; prim = prim->next)
			{
				// Get limits for the associated collection
				prim->collection()->yRangeOverX(xMin, xMax, yMinTest, yMaxTest);
				if (first)
				{
					yMin = yMinTest;
					yMax = yMaxTest;
					first = false;
				}
				else
				{
					if (yMinTest < yMin) yMin = yMinTest;
					if (yMaxTest > yMax) yMax = yMaxTest;
				}
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
			double yDelta = (yMax - yMin)*0.05;
			yMax += yDelta;
			yMin -= yDelta;
		}
	
		// Ensure a sensible range for the axes
		Axes::ensureSensibleRange(xMin, xMax, true);
		Axes::ensureSensibleRange(yMin, yMax, false);

		// Set new limits
		axes_.setRange(0, xMin, xMax);
		axes_.setRange(1, yMin, yMax);
	}
}

/*
 * Axes
 */

// Return absolute minimum transformed values over all displayed collections
Vec3<double> ViewPane::transformedDataMinima()
{
	// Set starting values from first display collection we find
	int nCounted = 0;
	Vec3<double> v, minima;
	for (TargetData* target = collectionTargets_.first(); target != NULL; target = target->next)
	{
		// Loop over display targets
		for (TargetPrimitive* prim = target->displayPrimitives(); prim != NULL; prim = prim->next)
		{
			if (nCounted == 0) minima = prim->collection()->transformMin();
			else
			{
				v = target->collection()->transformMin();
				if (v.x < minima.x) minima.x = v.x;
				if (v.y < minima.y) minima.y = v.y;
				if (v.z < minima.z) minima.z = v.z;
			}
			nCounted += prim->collection()->nValues();
		}
	}

	// If we didn't have any data to work with, return the current axis limits
	if (nCounted == 0) return Vec3<double>(axes_.limitMin(0), axes_.limitMin(1), axes_.limitMin(2));
	else return minima;
}

// Return absolute maximum transformed values over all associated collections
Vec3<double> ViewPane::transformedDataMaxima()
{
	// Set starting values from first display collection we find
	int nCounted = 0;
	Vec3<double> v, maxima;
	for (TargetData* target = collectionTargets_.first(); target != NULL; target = target->next)
	{
		// Loop over display targets
		for (TargetPrimitive* prim = target->displayPrimitives(); prim != NULL; prim = prim->next)
		{
			// Check number of datapoints in target
			if (prim->collection()->nValues() == 0) continue;

			// Set limits...
			if (nCounted == 0) maxima = prim->collection()->transformMax();
			else
			{
				v = target->collection()->transformMax();
				if (v.x > maxima.x) maxima.x = v.x;
				if (v.y > maxima.y) maxima.y = v.y;
				if (v.z > maxima.z) maxima.z = v.z;
			}
			nCounted += prim->collection()->nValues();
		}
	}

	// If we didn't have any data to work with, return the current axis limits
	if (nCounted == 0) return Vec3<double>(axes_.limitMax(0), axes_.limitMax(1), axes_.limitMax(2));
	else return maxima;
}

// Return absolute minimum positive transformed values over all associated collections
Vec3<double> ViewPane::transformedDataPositiveMinima()
{
	// Set starting values from first display collection we find
	int nCounted = 0;
	Vec3<double> v, minima;
	for (TargetData* target = collectionTargets_.first(); target != NULL; target = target->next)
	{
		// Loop over display targets
		for (TargetPrimitive* prim = target->displayPrimitives(); prim != NULL; prim = prim->next)
		{
			if (nCounted == 0) minima = prim->collection()->transformMinPositive();
			else
			{
				v = target->collection()->transformMinPositive();
				if (v.x < minima.x) minima.x = v.x;
				if (v.y < minima.y) minima.y = v.y;
				if (v.z < minima.z) minima.z = v.z;
			}
			nCounted += prim->collection()->nValues();
		}
	}

	// If we didn't have any data to work with, return the current axis limits
	if (nCounted == 0) return Vec3<double>(axes_.limitMin(0), axes_.limitMin(1), axes_.limitMin(2));
	else return minima;
}

// Return absolute maximum positive transformed values over all associated collections
Vec3<double> ViewPane::transformedDataPositiveMaxima()
{
	// Set starting values from first display collection we find
	int nCounted = 0;
	Vec3<double> v, maxima;
	for (TargetData* target = collectionTargets_.first(); target != NULL; target = target->next)
	{
		// Loop over display targets
		for (TargetPrimitive* prim = target->displayPrimitives(); prim != NULL; prim = prim->next)
		{
			if (nCounted == 0) maxima = prim->collection()->transformMaxPositive();
			else
			{
				v = target->collection()->transformMaxPositive();
				if (v.x > maxima.x) maxima.x = v.x;
				if (v.y > maxima.y) maxima.y = v.y;
				if (v.z > maxima.z) maxima.z = v.z;
			}
			nCounted += prim->collection()->nValues();
		}
	}

	// If we didn't have any data to work with, return the current axis limits
	if (nCounted == 0) return Vec3<double>(axes_.limitMax(0), axes_.limitMax(1), axes_.limitMax(2));
	else return maxima;
}

// Return axes for this pane
Axes& ViewPane::axes()
{
	return axes_;
}

// Update axis limits to represent data extent of associated collections
void ViewPane::updateAxisLimits(double xFrac, double yFrac, double zFrac)
{
	// Get transformed data extents
	Vec3<double> dataMin = transformedDataMinima();
	Vec3<double> dataMax = transformedDataMaxima();
	Vec3<double> dataMinPositive = transformedDataPositiveMinima();
	Vec3<double> dataMaxPositive = transformedDataPositiveMaxima();

	// The fractional values we've been passed tell us how much of the 'data' to include in the limits
	// A positive value, 0.0 < f < 1.0, tells us to shrink the maximum limit.
	// A negative value, -1.0 < f < 0.0, tells us to increase the minimum limit

	// Store the fractional values in a temporary Vector to make things easier
	Vec3<double> fractions(xFrac, yFrac, zFrac);

	// Loop over axes
	for (int axis = 0; axis < 3; ++axis)
	{
		// Adjust limits
		if (fractions[axis] > 0.0)
		{
			dataMax[axis] = dataMin[axis] + (dataMax[axis] - dataMin[axis])*fractions[axis];
			dataMaxPositive[axis] = dataMinPositive[axis] + (dataMaxPositive[axis] - dataMinPositive[axis])*fractions[axis];
		}
		else
		{
			dataMin[axis] = dataMax[axis] - (dataMin[axis] - dataMax[axis])*fractions[axis];
			dataMinPositive[axis] = dataMaxPositive[axis] - (dataMinPositive[axis] - dataMaxPositive[axis])*fractions[axis];
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
void ViewPane::shiftFlatAxisLimits(double deltaH, double deltaV)
{
	// If this is not a flat view, return now
	if (!isFlatView()) return;

	// Set indices for target axes
	int axes[2];
	axes[0] = 0;
	axes[1] = 1;
	if (viewType_ == ViewPane::FlatXZView) axes[1] = 2;
	else if (viewType_ == ViewPane::FlatZYView)
	{
		axes[0] = 1;
		axes[1] = 2;
	}

	// Loop over axis indices, and set new limits
	double deltas[2];
	deltas[0] = deltaH;
	deltas[1] = deltaV;
	for (int n=0; n<2; ++n)
	{
		double range = axes_.realRange(axes[n]);
		bool logarithmic = axes_.logarithmic(axes[n]);
		double ppUnit = axisPixelLength_[axes[n]] / range;

		// Flip sign of delta if the axis is inverted
		if (axes_.inverted(axes[n])) deltas[n] = -deltas[n];

		// Get adjusted min/max values
		double newMin, newMax;
		if (logarithmic)
		{
			newMin = pow(10, axes_.realMin(axes[n]) - deltas[n]/ppUnit);
			newMax = pow(10, axes_.realMax(axes[n]) - deltas[n]/ppUnit);
		}
		else
		{
			newMin = axes_.min(axes[n]) - deltas[n]/ppUnit;
			newMax = axes_.max(axes[n]) - deltas[n]/ppUnit;
		}

		axes_.setMin(axes[n], newMin);
		axes_.setMax(axes[n], newMax);
	}
}

// Shift flat view axis limits by specified fractional amounts
void ViewPane::shiftFlatAxisLimitsFractional(double fracH, double fracV)
{
	// Set indices for target axes
	int axes[2];
	axes[0] = 0;
	axes[1] = 1;
	if (viewType_ == ViewPane::FlatXZView) axes[1] = 2;
	else if (viewType_ == ViewPane::FlatZYView)
	{
		axes[0] = 1;
		axes[1] = 2;
	}

	// Determine deltas based on fractional amounts specified
	double deltaH = (axes_.max(axes[0]) - axes_.min(axes[0])) * fracH;
	double deltaV = (axes_.max(axes[1]) - axes_.min(axes[1])) * fracV;

	shiftFlatAxisLimits(deltaH, deltaV);
}

// Update current slices for all collections displayed in this pane
void ViewPane::collectionsUpdateCurrentSlices(int axis, double axisValue)
{
	for (TargetData* target = collectionTargets_.first(); target != NULL; target = target->next)
	{
		for (TargetPrimitive* prim = target->displayPrimitives(); prim != NULL; prim = prim->next)
		{
			prim->collection()->updateCurrentSlice(axis, axisValue);
		}
	}
}

/*
 * Style
 */

// Calculate font scaling factor
void ViewPane::calculateFontScaling()
{
	// Calculate text scaling factor
	Vec3<double> translate(0.0, 0.0, zOffset_);
	if (hasPerspective_) translate.z = 0.5;
	Vec3<double> unit = modelToScreen(Vec3<double>(0.0, 1.0, viewTranslation_.z), projectionMatrix_, Matrix4(), translate);
	unit.y -= viewportMatrix_[1] + viewportMatrix_[3]*0.5;
	textZScale_ = unit.y;
}

// Set current bounding box type
void ViewPane::setBoundingBox(ViewPane::BoundingBox type)
{
	boundingBox_ = type;

	// Bounding boxes are generated as part of the axes primitives, so need to regenerate them
	// TODO regenerateAxes_ = true;
}

// Return current bounding box type
ViewPane::BoundingBox ViewPane::boundingBox()
{
	return boundingBox_;
}

// Set y intercept for plane bounding box
void ViewPane::setBoundingBoxPlaneY(double value)
{
	boundingBoxPlaneY_ = value;

	// Bounding boxes are generated as part of the axes primitives, so need to regenerate them
	// TODO 0regenerateAxes_ = true;
}

// Return y intercept for plane bounding box
double ViewPane::boundingBoxPlaneY()
{
	return boundingBoxPlaneY_;
}

// Set font point size for axis value labels
void ViewPane::setLabelPointSize(double value)
{
	labelPointSize_ = value;

	axes_.setPrimitivesInvalid();
}

// Return font point size for axis value labels
double ViewPane::labelPointSize()
{
	return labelPointSize_;
}

// Return font point size for titles
void ViewPane::setTitlePointSize(double value)
{
	titlePointSize_ = value;

	axes_.setPrimitivesInvalid();
}

// Return font point size for titles
double ViewPane::titlePointSize()
{
	return titlePointSize_;
}

// Return text z scaling factor
double ViewPane::textZScale()
{
	return textZScale_;
}

// Set whether axis text labels are drawn flat in 3D views
void ViewPane::setFlatLabelsIn3D(bool flat)
{
	flatLabelsIn3D_ = flat;

	axes_.setPrimitivesInvalid();
}

// Whether axis text labels are drawn flat in 3D views
bool ViewPane::flatLabelsIn3D()
{
	return flatLabelsIn3D_;
}

// Return collection group manager for this ViewPane
CollectionGroupManager& ViewPane::collectionGroupManager()
{
	return collectionGroupManager_;
}

/*
 * Interaction
 */

// Return axis title at specified coordinates (if any)
int ViewPane::axisTitleAt(int screenX, int screenY)
{
	// Get view matrix inverse and FontInstance
	Matrix4 viewRotInverse = viewRotationInverse();
	FontInstance& fontInstance = parent_.uChromaBase().viewer()->fontInstance();

	Vec3<double> labelMin, labelMax;
	for (int axis=0; axis<3; ++axis)
	{
		// Reset bounding box extreme values
		labelMin.set(1e9,1e9,1e9);
		labelMax = -labelMin;

		// Calculate orthogonal bounding cuboid for this axis title (local coordinates)
		Cuboid cuboid = axes_.titlePrimitive(axis).boundingCuboid(fontInstance, viewRotInverse, textZScale_);

		// Determine whether the screen point specified is within the projected cuboid
		if (cuboid.isPointWithinProjection(screenX, screenY, viewMatrix(), projectionMatrix_, viewportMatrix_)) return axis;
	}
	return -1;
}

/*
 * GL
 */

// Create bounding box
void ViewPane::createBoundingBox(int type, double planeY)
{
	boundingBoxPrimitive_.forgetAll();
	
	if (type == 0) return;
	else if (type == 1)
	{
		// Plane in XZ, spanning data range   // TODO
// 		boundingBoxPrimitive_.plotLine(Vec3<double>(axisCoordMin_[0].x, planeY, axisCoordMin_[2].z), Vec3<double>(axisCoordMin_[0].x, planeY, axisCoordMax_[2].z));
// 		boundingBoxPrimitive_.plotLine(Vec3<double>(axisCoordMin_[0].x, planeY, axisCoordMax_[2].z), Vec3<double>(axisCoordMax_[0].x, planeY, axisCoordMax_[2].z));
// 		boundingBoxPrimitive_.plotLine(Vec3<double>(axisCoordMax_[0].x, planeY, axisCoordMax_[2].z), Vec3<double>(axisCoordMax_[0].x, planeY, axisCoordMin_[2].z));
// 		boundingBoxPrimitive_.plotLine(Vec3<double>(axisCoordMax_[0].x, planeY, axisCoordMin_[2].z), Vec3<double>(axisCoordMin_[0].x, planeY, axisCoordMin_[2].z));
	}
}


// Update interaction primitive
void ViewPane::updateInteractionPrimitive(int axis)
{
	const int nPoints = 16;
	
	interactionPrimitive_.initialise(GL_TRIANGLES, false);
	interactionPrimitive_.forgetAll();
	interactionBoxPrimitive_.initialise(GL_LINES, false);
	interactionBoxPrimitive_.forgetAll();

	if (axis == -1) return;

	// Grab axes, and knock out values in the supplied vectors which correspond to the activated axis
	Vec3<double> axisMinA, axisMinB, axisMaxA, axisMaxB;
	axisMinA[(axis+1)%3] = axes_.coordMin((axis+1)%3)[(axis+1)%3];
	axisMaxA[(axis+1)%3] = axes_.coordMax((axis+1)%3)[(axis+1)%3];
	axisMinB[(axis+2)%3] = axes_.coordMin((axis+2)%3)[(axis+2)%3];
	axisMaxB[(axis+2)%3] = axes_.coordMax((axis+2)%3)[(axis+2)%3];
	axisMinA[axis] = 0.0;
	axisMaxA[axis] = 0.0;
	axisMinB[axis] = 0.0;
	axisMaxB[axis] = 0.0;

	// Create 'bounding box' for slice primitive
	Vec3<double> normal(0.0, 0.0, 1.0);
	
	interactionBoxPrimitive_.defineVertex(axisMinA + axisMinB, normal);
	interactionBoxPrimitive_.defineVertex(axisMinA + axisMaxB, normal);
	interactionBoxPrimitive_.defineVertex(axisMaxA + axisMaxB, normal);
	interactionBoxPrimitive_.defineVertex(axisMaxA + axisMinB, normal);
	interactionBoxPrimitive_.defineIndices(0,1);
	interactionBoxPrimitive_.defineIndices(1,2);
	interactionBoxPrimitive_.defineIndices(2,3);
	interactionBoxPrimitive_.defineIndices(3,0);

	// Work out deltas for each direction
	Vec3<double> deltaA, deltaB, pos;
	deltaA = (axisMaxA - axisMinA) / nPoints;
	deltaB = (axisMaxB - axisMinB) / nPoints;

	// Set normal
	normal.zero();
	normal[axis] = 1.0;

	// Construct plane
	GLuint a, b, c, d;
	for (int n=0; n<nPoints; ++n)
	{
		pos = axisMinA + axisMinB + deltaA*n;
		for (int m=0; m<nPoints; ++m)
		{
			a = interactionPrimitive_.defineVertex(pos, normal);
			b = interactionPrimitive_.defineVertex(pos + deltaA, normal);
			c = interactionPrimitive_.defineVertex(pos + deltaA + deltaB, normal);
			d = interactionPrimitive_.defineVertex(pos + deltaB, normal);
			interactionPrimitive_.defineIndices(a, b, c);
			interactionPrimitive_.defineIndices(c, d, a);
			pos += deltaB;
		}
	}
}

// Return interaction primitive
Primitive& ViewPane::interactionPrimitive()
{
	return interactionPrimitive_;
}

// Return interaction box primitive
Primitive& ViewPane::interactionBoxPrimitive()
{
	return interactionBoxPrimitive_;
}

// Return bounding box primitive
Primitive& ViewPane::boundingBoxPrimitive()
{
	return boundingBoxPrimitive_;
}

/*
 * Signalling
 */

// Process supplied Collection signal if it is relevant to this pane
UChromaSignal::SignalAction ViewPane::processCollectionSignal(UChromaSignal::CollectionSignal signal, Collection* collection)
{
	switch (signal)
	{
		// Collection Created
		// -- Role Relevance : ???
		case (UChromaSignal::CollectionCreatedSignal):
			return UChromaSignal::IgnoreSignal;
			break;
		// Collection Deleted
		// -- Role Relevance : All
		case (UChromaSignal::CollectionDeletedSignal):
			// If the emitting collection is a current target, remove it from our list
			if (!collectionIsTarget(collection)) return UChromaSignal::IgnoreSignal;
			removeCollectionTarget(collection);
			break;
		// Current Slice Changed
		// -- Role Relevance : SliceMonitorRole
		case (UChromaSignal::CollectionSliceChangedSignal):
			if (role_ != ViewPane::SliceMonitorRole) return UChromaSignal::IgnoreSignal;
			if (!collectionIsTarget(collection)) 
			{
				addCollectionTarget(collection);
				return UChromaSignal::IgnoreSignal;
			}
			// Update axes limits
			updateAxisLimits();
			break;
		// Extracted Data Added
		// -- Role Relevance : Extractor
		case (UChromaSignal::CollectionSliceExtractedSignal):
			if (role_ != ViewPane::ExtractorRole) return UChromaSignal::IgnoreSignal;
			addCollectionTarget(collection);
			updateAxisLimits();
			return UChromaSignal::IgnoreSignal;
			break;
		// Data Changed
		// -- Role Relevance : ???
		case (UChromaSignal::CollectionDataChangedSignal):
			return UChromaSignal::IgnoreSignal;
			break;
		default:
			return UChromaSignal::IgnoreSignal;
	}

	return UChromaSignal::AcceptSignal;
}
