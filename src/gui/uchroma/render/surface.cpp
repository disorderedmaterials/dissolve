/*
	*** Surface Generation
	*** src/gui/uchroma/render/surface.cpp
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

#include "gui/uchroma/render/surface.h"
#include "gui/uchroma/classes/axes.h"

// Construct normal / colour data for vertex specified
Vec3<double> Surface::constructVertexNormals(const Array<double>& abscissa, int index, DisplayDataSet* targetDataSet, DisplayDataSet* previousDataSet, DisplayDataSet* nextDataSet, int nPoints)
{
	Vec3<double> normals;

	// Grab references to first target array
	//const Array<double>& y = targetDataSet->y();

	// Check whether previous / next slice information is available
	if ((previousDataSet == NULL) && (nextDataSet == NULL))
	{
		// Neither previous nor next slices exist - the normal will depend only on the previous and/or next points

		// Check on index provided
		if (index == 0)
		{
			// First point in array, so no previous points available
		}
		else if (index == (nPoints-1))
		{
			// Last point in array, so no next point available
		}
		else
		{
			// Point somewhere in middle of array, so have previous and next points
		}
	}
	else if (previousDataSet == NULL)
	{
		// No previous slice available, so normals depend on this and next slice only

		// Check on index provided
		if (index == 0)
		{
			// First point in array, so no previous points available
		}
		else if (index == (nPoints-1))
		{
			// Last point in array, so no next point available
		}
		else
		{
			// Point somewhere in middle of array, so have previous and next points
		}
	}
	else if (nextDataSet == NULL)
	{
		// No next slice available, so normals depend on this and previous slice only

		// Check on index provided
		if (index == 0)
		{
			// First point in array, so no previous points available
		}
		else if (index == (nPoints-1))
		{
			// Last point in array, so no next point available
		}
		else
		{
			// Point somewhere in middle of array, so have previous and next points
		}
	}
	else
	{
		// Both previous and next slices available, so use all available data
	}

	return Vec3<double>();
}

// Construct normal / colour data for slice specified
void Surface::constructSurfaceStrip(const Array<double>& x, const Array<double>& y, double z, const Axes& axes, Array< Vec3<double> >& normals, Array< Vec4<GLfloat> >& colours, const ColourScale& colourScale, const Array<double>& yPrev, double zPrev, const Array<double>& yNext, double zNext)
{
	normals.clear();
	colours.clear();
	if ((yPrev.nItems() == 0) && (yNext.nItems() == 0)) return;
	
	// Grab references to target arrays
	int nX = x.nItems();
	if (nX < 2) return;

	// Get colour data
	int n;
	QColor colour;
	double yScale = axes.stretch(1);
	if (axes.logarithmic(1))
	{
		for (n=0; n<nX; ++n)
		{
			colour = colourScale.colour(pow(10.0, y.constAt(n) / yScale));
			colours.add(Vec4<GLfloat>(colour.redF(), colour.greenF(), colour.blueF(), colour.alphaF()));
		}
	}
	else
	{
		for (n=0; n<nX; ++n)
		{
			colour = colourScale.colour(y.constAt(n) / yScale);
			colours.add(Vec4<GLfloat>(colour.redF(), colour.greenF(), colour.blueF(), colour.alphaF()));
		}
	}

	// Calculate normals
	Vec3<double> v1, v2, v3;
	double dz;
	if ((yPrev.nItems() != 0) && (yNext.nItems() != 0))
	{
		// Grab other array references
		dz = zPrev - zNext;

		// -- First point
		v1.set(x.constAt(1) - x.constAt(0), y.constAt(1) - y.constAt(0), 0);
		v2.set(0.0, yNext.constAt(0) - yPrev.constAt(0), dz);
		normals.add(v1 * v2);
// 		v3 = v1 * v2;
// 		v3.normalise();
// 		normals.add(v3);
// 		printf("Norm %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", 0, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		// -- Points 1 to N-2
		for (n=1; n< nX -1; ++n)
		{
			v1.set(x.constAt(n+1) - x.constAt(n-1), y.constAt(n+1) - y.constAt(n-1), 0.0);
			v2.set(0.0, yNext.constAt(n) - yPrev.constAt(n), dz);
			normals.add(v1 * v2);
// 			v3 = v1 * v2;
// 			v3.normalise();
// 			normals.add(v3);
// 			printf("Norm %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", n, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		}
		// -- Last point
		v1.set(x.constAt(nX -1) - x.constAt(nX -2), y.constAt(nX -1) - y.constAt(nX -2), 0.0);
		v2.set(0.0, yPrev.constAt(nX -1) - yNext.constAt(nX -1), dz);
		normals.add(v1 * v2);
// 		v3 = v1 * v2;
// 		v3.normalise();
// 		normals.add(v3);
// 		printf("Norm %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", nPoints-1, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
	}
	else if (yPrev.nItems() != 0)
	{
		// Grab other array reference
		dz = zPrev - z;

		// -- First point
		v1.set(x.constAt(1) - x.constAt(0), y.constAt(1) - y.constAt(0), 0.0);
		v2.set(0.0, y.constAt(0) - yPrev.constAt(0), dz);
		normals.add(v1 * v2);
// 		v3 = v1 * v2;
// 		v3.normalise();
// 		normals.add(v3);
// 		printf("Last %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", 0, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		// -- Points 1 to N-2
		for (n=1; n< nX -1; ++n)
		{
			v1.set(x.constAt(n+1) - x.constAt(n-1), y.constAt(n+1) - y.constAt(n-1), 0.0);
			v2.set(0.0, y.constAt(n) - yPrev.constAt(n), dz);
			normals.add(v1 * v2);
// 			v3 = v1 * v2;
// 			v3.normalise();
// 			normals.add(v3);
// 			printf("Last %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", n, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		}
		// -- Last point
		v1.set(x.constAt(nX-1) - x.constAt(nX-2), y.constAt(nX-1) - y.constAt(nX-2), 0.0);
		v2.set(0.0, y.constAt(nX -1) - yPrev.constAt(nX -1), dz);
		normals.add(v1 * v2);
// 		v3 = v1 * v2;
// 		v3.normalise();
// 		normals.add(v3);
// 		printf("Last %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", nPoints-1, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
	}
	else
	{
		// Grab other array reference
		dz = z - zNext;

		// -- First point
		v1.set(x.constAt(1) - x.constAt(0), y.constAt(1) - y.constAt(0), 0.0);
		v2.set(0.0, yNext.constAt(0) - y.constAt(0), dz);
		normals.add(v1 * v2);
// 		v3 = v1 * v2;
// 		v3.normalise();
// 		normals.add(v3);
// 		printf("Frst %i = (%f %f %f) * (%f %f %f) = (%f %f %f)\n", 0, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		// -- Points 1 to N-2
		for (n=1; n< nX -1; ++n)
		{
			v1.set(x.constAt(n+1) - x.constAt(n-1), y.constAt(n+1) - y.constAt(n-1), 0.0);
			v2.set(0.0, yNext.constAt(n) - y.constAt(n), dz);
			normals.add(v1 * v2);
// 			v3 = v1 * v2;
// 			v3.normalise();
// 			normals.add(v3);
// 			printf("Frst %i = (%f %f %f) * (%f %f %f) = (%f %f %f)\n", n, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		}
		// -- Last point
		v1.set(x.constAt(nX-1) - x.constAt(nX-2), y.constAt(nX-1) - y.constAt(nX-2), 0.0);
		v2.set(0.0, yNext.constAt(nX -1) - y.constAt(nX -1), dz);
		normals.add(v1 * v2);
// 		v3 = v1 * v2;
// 		v3.normalise();
// 		normals.add(v3);
// 		printf("Frst %i = (%f %f %f) * (%f %f %f) = (%f %f %f)\n", nPoints-1, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
	}

	// Normalise normals
	for (n=0; n<normals.nItems(); ++n) normals[n].normalise();

	// Swap signs of normals to 'undo' directionality imposed by axis inversions, to make surface lighting correct
	if (axes.inverted(0) || axes.inverted(1) || axes.inverted(2))
	{
		Vec3<double> multiplier(1.0, 1.0, 1.0);
		if (axes.inverted(0)) multiplier.multiply(Vec3<double>(1.0, -1.0, -1.0));
		if (axes.inverted(1)) multiplier.multiply(Vec3<double>(-1.0, 1.0, -1.0));
		if (axes.inverted(2)) multiplier.multiply(Vec3<double>(-1.0, -1.0, 1.0));
		for (n=0; n<normals.nItems(); ++n) normals[n].multiply(multiplier);
	}
}

// Calculate integer index extents for display data given supplied axes
bool Surface::calculateExtents(const Axes& axes, const Array<double>& abscissa, List<DisplayDataSet>& displayData, Vec3<int>& minIndex, Vec3<int>& maxIndex)
{
	// Grab some stuff from the pane's axes
	Vec3<double> axisMin(axes.min(0), axes.min(1), axes.min(2));
	Vec3<double> axisMax(axes.max(0), axes.max(1), axes.max(2));

	// Get x index limits
	for (minIndex.x = 0; minIndex.x < abscissa.nItems(); ++minIndex.x) if (abscissa.constAt(minIndex.x) >= axisMin.x) break;
	if (minIndex.x == abscissa.nItems()) return false;
	for (maxIndex.x = abscissa.nItems()-1; maxIndex.x >= 0; --maxIndex.x) if (abscissa.constAt(maxIndex.x) <= axisMax.x) break;
	if (maxIndex.x < 0) return false;

	// Get z index limits
	for (minIndex.z = 0; minIndex.z < displayData.nItems(); ++minIndex.z) if (displayData[minIndex.z]->z() >= axisMin.z) break;
	if (minIndex.z == displayData.nItems()) return false;
	for (maxIndex.z = displayData.nItems()-1; maxIndex.z >= 0; --maxIndex.z)  if (displayData[maxIndex.z]->z() <= axisMax.z) break;
	if (maxIndex.z < 0) return false;

	return true;
}
