/*
	*** Surface Generation - Grid
	*** src/gui/uchroma/render/surface_grid.cpp
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

// Construct grid surface representation of data
void Surface::constructGrid(PrimitiveList& primitiveList, const Axes& axes, const Array<double>& displayAbscissa, List<DisplayDataSet>& displayData, ColourScale colourScale)
{
	// Forget all data in current primitives
	primitiveList.forgetAll();

	// Get extents of displayData to use based on current axes limits
	Vec3<int> minIndex, maxIndex;
	if (!calculateExtents(axes, displayAbscissa, displayData, minIndex, maxIndex)) return;

	// Copy and transform abscissa values (still in data space) into axes coordinates
	Array<double> x(displayAbscissa, minIndex.x, maxIndex.x);
	axes.transformX(x);
	int nX = x.nItems();
	if (nX < 2) return;

	// Decide how large to make VertexChunks in Primitives
	// We will consider multiple slices at once in order to take most advantage of the post-transform cache
	// forming (abscissa.nItems()/(cacheSize-1)+1) Primitives (we divide by (cacheSize-1) because we will force an overlap
	// of one gridpoint between adjacent strips).
	const int cacheSize = 12;
	int nPrimitives = x.nItems()/(cacheSize-1) + 1;

	// Get some values from axes so we can calculate colours properly
	bool yLogarithmic = axes.logarithmic(1);
	double yStretch = axes.stretch(1);

	// Reinitialise primitive list
	primitiveList.reinitialise(nPrimitives, true, GL_LINES, true);

	// Temporary variables
	int n, offset = 0, i, nLimit, nMax;
	Vec4<GLfloat> colour;
	Vec3<double> nrm(0.0, 1.0, 0.0);
	Array<double> y;
	Array<DisplayDataSet::DataPointType> yType;

	DisplayDataSet** slices = displayData.array();
	Primitive* currentPrimitive = primitiveList[0];
	int verticesA[cacheSize], verticesB[cacheSize];
	double z;

	// Loop over abscissa indices
	while (offset <= x.nItems())
	{
		// Set nLimit to ensure we don't go beyond the end of the data arrays
		nLimit = std::min(cacheSize, x.nItems()-offset);

		// Loop over remaining displayData
		for (int slice = minIndex.z; slice <= maxIndex.z; ++slice)
		{
			// Grab arrays
			y.copy(slices[slice]->y(), minIndex.x+offset, minIndex.x+offset+nLimit-1);
			yType.copy(slices[slice]->yType(), minIndex.x+offset, minIndex.x+offset+nLimit-1);
			axes.transformY(y, yType);
			z = axes.transformZ(slices[slice]->z());

			// Generate vertices for this row
			for (n=0; n<nLimit; ++n)
			{
				i = offset+n;
				if (yType.constAt(n) != DisplayDataSet::NoPoint)
				{
					// A value exists here, so define a vertex
					colourScale.colour(yLogarithmic ? pow(10.0, y.constAt(n) / yStretch) : y.constAt(n) / yStretch, colour);
					verticesB[n] = currentPrimitive->defineVertex(x.constAt(i), y.constAt(n), z, nrm, colour);

					// If the previous vertex on this row also exists, draw a line here
					if ((n != 0) && (verticesB[n-1] != -1)) currentPrimitive->defineIndices(verticesB[n-1], verticesB[n]);
				}
				else verticesB[n] = -1;
			}

			// Draw lines across slices (if slice != 0)
			if (slice != 0)
			{
				nMax = (maxIndex.z-slice) > 1 ? nLimit-1 : nLimit;
				for (n=0; n<nMax; ++n)
				{
					if ((verticesA[n] != -1) && (verticesB[n] != -1)) currentPrimitive->defineIndices(verticesA[n], verticesB[n]);
				}
			}

			// Store current vertices for next pass
			for (n=0; n<cacheSize; ++n) verticesA[n] = verticesB[n];
		}

		// Move to next primitive and increase offset
		currentPrimitive = currentPrimitive->next;
		offset += cacheSize-1;
	}
}
