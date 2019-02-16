/*
	*** Renderable - Data1D
	*** src/gui/viewer/render/renderabledata1d.cpp
	Copyright T. Youngs 2013-2019

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

#include "gui/viewer/render/renderabledata1d.h"
#include "gui/viewer/render/renderablegroupmanager.h"
#include "gui/viewer/render/view.h"

// Constructor
RenderableData1D::RenderableData1D(const Data1D& source) : Renderable(Renderable::Data1DRenderable), source_(source)
{
}

// Destructor
RenderableData1D::~RenderableData1D()
{
}

/*
 * Data
 */

// Return version of data
const int RenderableData1D::version() const
{
	return source_.version();
}

// Return identifying tag/name for contained data
const char* RenderableData1D::objectIdentifier() const
{
	return source_.objectTag();
}

/*
 * Transform / Limits
 */

// Transform data according to current settings
void RenderableData1D::transformData()
{
	// If the transformed data is already up-to-date, no need to do anything
	if (transformDataVersion_ == version()) return;

	// Copy original data and transform it now. We do this even if the transformers are disabled, since they may have previously been active
	transformedData_ = source_;
	Transformer::transform(transformedData_, transforms_[0], transforms_[1]);

	transformMin_ = 0.0;
	transformMax_ = 0.0;
	transformMinPositive_ = 0.1;
	transformMaxPositive_ = -1.0;
	
	transformMin_.set(transformedData_.constXAxis().firstValue(), transformedData_.minValue(), 0.0);
	transformMax_.set(transformedData_.constXAxis().lastValue(), transformedData_.maxValue(), 0.0);

	// Now determine minimum positive limits - loop over points in data, searching for first positive, non-zero value
	for (int n=0; n<transformedData_.nValues(); ++n)
	{
		// X
		if (transformedData_.constXAxis(n) > 0.0)
		{
			if (transformedData_.constXAxis(n) < transformMinPositive_.x) transformMinPositive_.x = transformedData_.constXAxis(n);
			if (transformedData_.constXAxis(n) > transformMaxPositive_.x) transformMaxPositive_.x = transformedData_.constXAxis(n);
		}
		// Y
		if (transformedData_.constValue(n) > 0.0)
		{
			if (transformedData_.constValue(n) < transformMinPositive_.y) transformMinPositive_.y = transformedData_.constValue(n);
			if (transformedData_.constValue(n) > transformMaxPositive_.y) transformMaxPositive_.y = transformedData_.constValue(n);
		}
	}
	
	transformMinPositive_.z = 1.0;
	transformMaxPositive_.z = 1.0;

	// Check maximum positive values (since all datapoints might have been negative
	if (transformMaxPositive_.x < 0.0) transformMaxPositive_.x = 1.0;
	if (transformMaxPositive_.y < 0.0) transformMaxPositive_.y = 1.0;
	if (transformMaxPositive_.z < 0.0) transformMaxPositive_.z = 1.0;

	// Update the transformed data 'version'
	transformDataVersion_ = version();
}

// Return reference to transformed data
const Data1D& RenderableData1D::transformedData()
{
	// If no transforms are enabled, just return the original data
	if ((!transforms_[0].enabled()) && (!transforms_[1].enabled())) return source_;

	// Make sure the transformed data is up-to-date
	transformData();

	return transformedData_;
}

// Calculate min/max y value over specified x range (if possible in the underlying data)
bool RenderableData1D::yRangeOverX(double xMin, double xMax, double& yMin, double& yMax)
{
	// Ensure transformed data is up-to-date
	transformData();

	// Grab reference to transformed data
	const Data1D& data = transformedData();

	for (int n=0; n<data.nValues(); ++n)
	{
		if (data.constXAxis(n) < xMin) continue;
		else if (data.constXAxis(n) > xMax) break;

		if (n == 0)
		{
			yMin = data.constValue(n);
			yMax = yMin;
		}
		else
		{
			if (data.constValue(n) < yMin) yMin = data.constValue(n);
			else if (data.constValue(n) > yMax) yMax = data.constValue(n);
		}
	}

	return true;
}

/*
 * Rendering Primitives
 */

// Update primitives and send for display
void RenderableData1D::updateAndSendPrimitives(View& view, RenderableGroupManager& groupManager, bool forceUpdate, bool pushAndPop, const QOpenGLContext* context)
{
	// Grab axes for the target ViewPane
	const Axes& axes = view.axes();

	// Grab copy of the relevant colour definition for this Collection
	ColourDefinition colour = groupManager.colourDefinition(this);

	// Check whether the primitive for this collection needs updating
	bool upToDate = true;
	if (forceUpdate) upToDate = false;
	else if (primitivesAxesVersion_!= axes.displayVersion()) upToDate = false;
	else if (!DissolveSys::sameString(primitivesColourDefinitionFingerprint_, CharString("%s%i", groupName(), colour.colourVersion()), true)) upToDate = false;
	else if (primitivesDataVersion_ != version()) upToDate = false;
	else if (primitivesStyleVersion_ != displayStyleVersion()) upToDate = false;

	// If the primitive is out of date, recreate it's data.
	if (!upToDate)
	{
		// Forget all current data
		primitives_.forgetAll();


		// Recreate primitive depending on current style
		switch (displayStyle())
		{
			case (RenderableData1D::LineXYStyle):
				primitives_.reinitialise(1, true, GL_LINE_STRIP, true);
				constructLineXY(transformedData().constXAxis(), transformedData().constValues(), primitives_[0], axes, colour.colourScale());
				break;
// 			case (RenderableData1D::LineZYStyle):
// 				Surface::constructLineZY(primitive_, axes, collection_->displayAbscissa(), collection_->displayData(), colour.colourScale());
// 				break;
// 			case (RenderableData1D::GridStyle):
// 				Surface::constructGrid(primitive_, axes, collection_->displayAbscissa(), collection_->displayData(), colour.colourScale());
// 				break;
// 			case (RenderableData1D::SurfaceStyle):
// 			case (RenderableData1D::UnlitSurfaceStyle):
// 				Surface::constructFull(primitive_, axes, collection_->displayAbscissa(), collection_->displayData(), colour.colourScale());
// 				break;
			default:
				printf("Internal Error: Display style %i not accounted for in RenderableData1D::updateAndSendPrimitive().\n", displayStyle());
				break;
		}

		// Pop old primitive instance (unless flagged not to)
		if ((!pushAndPop) && (primitives_.nInstances() != 0)) primitives_.popInstance(context);
	
		// Push a new instance to create the new display list / vertex array
		primitives_.pushInstance(context);
	}

	// Send primitive
	sendToGL();

	// Pop current instance (if flagged)
	if (pushAndPop) primitives_.popInstance(context);

	// Store version points for the up-to-date primitive
	primitivesAxesVersion_ = axes.displayVersion();
	primitivesColourDefinitionFingerprint_.sprintf("%s%i", groupName(), colour.colourVersion());
	primitivesDataVersion_ = version();
	primitivesStyleVersion_ = displayStyleVersion();

	return;
}

// Create line strip primitive
void RenderableData1D::constructLineXY(const Array<double>& displayAbscissa, const Array<double>& displayValues, Primitive* primitive, const Axes& axes, ColourScale colourScale, double zCoordinate)
{
// 	// Get extents of displayData to use based on current axes limits
// 	Vec3<int> minIndex, maxIndex;
// 	if (!calculateExtents(axes, displayAbscissa, displayData, minIndex, maxIndex)) return;
// 	int nZ = (maxIndex.z - minIndex.z) + 1;

	// Copy and transform abscissa values (still in data space) into axes coordinates
// 	Array<double> x(displayAbscissa, minIndex.x, maxIndex.x);
	Array<double> x = displayAbscissa;
	axes.transformX(x);
	int nX = x.nItems();
	if (nX < 2) return;

	// Get some values from axes so we can calculate colours properly
	bool yLogarithmic = axes.logarithmic(1);
	double yStretch = axes.stretch(1);

	// Temporary variables
	Vec4<GLfloat> colour(0,0,0,1);
	Vec3<double> nrm(0.0, 1.0, 0.0);

	// Create lines for slices
	int vertexA, vertexB;
	// Grab y and z values
	Array<double> y = displayValues;
	axes.transformY(y);
	double z = axes.transformZ(zCoordinate);

	// Set vertexA to -1 so we don't draw a line at n=0
	vertexA = -1;

	// Loop over x values
	for (int n=0; n<nX; ++n)
	{
		colourScale.colour(yLogarithmic ? pow(10.0, y.constAt(n) / yStretch) : y.constAt(n) / yStretch, colour);
		//printf("LINEXY Colour %i %i = %f %f %f %f\n", slice, n, colour[0], colour[1], colour[2], colour[3]);
		vertexB = primitive->defineVertex(x.constAt(n), y.constAt(n), z, nrm, colour);

		// If both vertices are valid, plot a line
		if (vertexA != -1) primitive->defineIndices(vertexA, vertexB);

		vertexA = vertexB;
	}
}

// Calculate integer index extents for display data given supplied axes
// bool Surface::calculateExtents(const Axes& axes, const Array<double>& abscissa, List<DisplayDataSet>& displayData, Vec3<int>& minIndex, Vec3<int>& maxIndex)
// {
// 	// Grab some stuff from the pane's axes
// 	Vec3<double> axisMin(axes.min(0), axes.min(1), axes.min(2));
// 	Vec3<double> axisMax(axes.max(0), axes.max(1), axes.max(2));
// 
// 	// Get x index limits
// 	for (minIndex.x = 0; minIndex.x < abscissa.nItems(); ++minIndex.x) if (abscissa.constAt(minIndex.x) >= axisMin.x) break;
// 	if (minIndex.x == abscissa.nItems()) return false;
// 	for (maxIndex.x = abscissa.nItems()-1; maxIndex.x >= 0; --maxIndex.x) if (abscissa.constAt(maxIndex.x) <= axisMax.x) break;
// 	if (maxIndex.x < 0) return false;
// 
// 	// Get z index limits
// 	for (minIndex.z = 0; minIndex.z < displayData.nItems(); ++minIndex.z) if (displayData[minIndex.z]->z() >= axisMin.z) break;
// 	if (minIndex.z == displayData.nItems()) return false;
// 	for (maxIndex.z = displayData.nItems()-1; maxIndex.z >= 0; --maxIndex.z)  if (displayData[maxIndex.z]->z() <= axisMax.z) break;
// 	if (maxIndex.z < 0) return false;
// 
// 	return true;
// }
