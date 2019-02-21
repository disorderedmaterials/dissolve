/*
	*** Renderable - Species
	*** src/gui/viewer/render/renderablespecies.cpp
	Copyright T. Youngs 2019

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

#include "gui/viewer/render/renderablespecies.h"
#include "gui/viewer/render/renderablegroupmanager.h"
#include "gui/viewer/render/view.h"

// Constructor
RenderableSpecies::RenderableSpecies(const Species* source, const char* objectTag) : Renderable(Renderable::SpeciesRenderable, objectTag), source_(source)
{
}

// Destructor
RenderableSpecies::~RenderableSpecies()
{
}

/*
 * Data
 */

// Return whether a valid data source is available (attempting to set it if not)
bool RenderableSpecies::validateDataSource()
{
	// If there is no valid source set, attempt to set it now...
	if (!source_) source_ = Species::findObject(objectTag_);

	return source_;
}

// Return version of data
const int RenderableSpecies::version() const
{
	return (source_ ? source_->version() : -99);
}

/*
 * Transform / Limits
 */

// Calculate min/max y value over specified x range (if possible in the underlying data)
bool RenderableSpecies::yRangeOverX(double xMin, double xMax, double& yMin, double& yMax)
{
	yMin = 0.0;
	yMax = 1.0;

	return true;
}

/*
 * Rendering Primitives
 */

// Update primitives and send for display
void RenderableSpecies::updateAndSendPrimitives(View& view, RenderableGroupManager& groupManager, bool forceUpdate, bool pushAndPop, const QOpenGLContext* context)
{
	// Grab axes for the View
	const Axes& axes = view.axes();

	// Grab copy of the relevant colour definition for this Collection
	ColourDefinition colourDefinition = groupManager.colourDefinition(this);

	// Check whether the primitive for this collection needs updating
	bool upToDate = true;
	if (forceUpdate) upToDate = false;
	else if (primitivesAxesVersion_!= axes.version()) upToDate = false;
	else if (!DissolveSys::sameString(primitivesColourDefinitionFingerprint_, CharString("%p@%i", group_, colourDefinition.version()), true)) upToDate = false;
	else if (primitivesDataVersion_ != version()) upToDate = false;
	else if (primitivesStyleVersion_ != displayStyleVersion()) upToDate = false;

	// TODO Need to push/pop base primitives here, and create a PrimtiveInfo list?
	// TODO Lock Axes to 3D (relative) view, rather than scaled / autostreched?

	// If the primitive is out of date, recreate it's data.
	if (!upToDate)
	{


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
	primitivesAxesVersion_ = axes.version();
	primitivesColourDefinitionFingerprint_.sprintf("%p@%i", group_, colourDefinition.version());
	primitivesDataVersion_ = version();
	primitivesStyleVersion_ = displayStyleVersion();

	return;
}

// Create line strip primitive
void RenderableSpecies::constructLineXY(const Array<double>& displayAbscissa, const Array<double>& displayValues, Primitive* primitive, const Axes& axes, const ColourDefinition& colourDefinition, double zCoordinate)
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

	// Check for a single colour style in the colourDefinition - use optimised case in that eventuality
	if (colourDefinition.style() == ColourDefinition::SingleColourStyle)
	{
		// Get the single colour
		colourDefinition.colour(0.0, colour);

		// Loop over x values
		for (int n=0; n<nX; ++n)
		{
			vertexB = primitive->defineVertex(x.constAt(n), y.constAt(n), z, nrm, colour);

			// If both vertices are valid, plot a line
			if (vertexA != -1) primitive->defineIndices(vertexA, vertexB);

			vertexA = vertexB;
		}
	}
	else
	{
		// Loop over x values
		for (int n=0; n<nX; ++n)
		{
			colourDefinition.colour(yLogarithmic ? pow(10.0, y.constAt(n) / yStretch) : y.constAt(n) / yStretch, colour);
			vertexB = primitive->defineVertex(x.constAt(n), y.constAt(n), z, nrm, colour);

			// If both vertices are valid, plot a line
			if (vertexA != -1) primitive->defineIndices(vertexA, vertexB);

			vertexA = vertexB;
		}
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
