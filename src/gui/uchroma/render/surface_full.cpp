/*
	*** Sufrace Generation - Full
	*** src/gui/uchroma/render/surface_full.cpp
	Copyright T. Youngs 2013-2018

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

// Construct full surface representation of data
void Surface::constructFull(PrimitiveList& primitiveList, const Axes& axes, const Array<double>& displayAbscissa, List<DisplayDataSet>& displayData, ColourScale colourScale)
{
	// Forget all data in current primitives
	primitiveList.forgetAll();

	// Get extents of displayData to use based on current axes limits
	Vec3<int> minIndex, maxIndex;
	if (!calculateExtents(axes, displayAbscissa, displayData, minIndex, maxIndex)) return;
	int nZ = (maxIndex.z - minIndex.z) + 1;

	// Copy and transform abscissa values (still in data space) into axes coordinates
	Array<double> x(displayAbscissa, minIndex.x, maxIndex.x);
	axes.transformX(x);
	int nX = x.nItems();

	// Check for specific actions when we have a low number of display datasets
	if (nZ == 1)
	{
		// Special case, if there is exactly one dataset, draw a standard XY line surface instead
		constructLineXY(primitiveList, axes, displayAbscissa, displayData, colourScale);
		return;
	}
	if (nX == 1)
	{
		// Special case, if there is exactly one dataset, draw a standard XY line surface instead
		constructLineZY(primitiveList, axes, displayAbscissa, displayData, colourScale);
		return;
	}

	// Resize primitive list so it's large enough for our needs
	primitiveList.reinitialise(nZ-1, false, GL_TRIANGLES, true);

	// Temporary variables
	Array< Vec3<double> > normA, normB;
	Array<double> yA, yB, yC;
	Array<DisplayDataSet::DataPointType> typeA, typeB, typeC;
	Array< Vec4<GLfloat> > colourA, colourB;
	QColor colour;
	double zA, zB = 0.0, zC = 0.0;
	Vec3<double> nrm(0.0, 1.0, 0.0);

	// Construct first slice data and set initial min/max values
	yA.copy(displayData[minIndex.z]->y(), minIndex.x, maxIndex.x);
	typeA.copy(displayData[minIndex.z]->yType(), minIndex.x, maxIndex.x);
	axes.transformY(yA, typeA);
	zA = axes.transformZ(displayData[minIndex.z]->z());
	if ((minIndex.z+1) <= maxIndex.z)	// Safety check - this should always be true because of the checks above
	{
		yB.copy(displayData[minIndex.z+1]->y(), minIndex.x, maxIndex.x);
		typeB.copy(displayData[minIndex.z+1]->yType(), minIndex.x, maxIndex.x);
		axes.transformY(yB, typeB);
		zB = axes.transformZ(displayData[minIndex.z+1]->z());
	}
	constructSurfaceStrip(x, yA, zA, axes, normA, colourA, colourScale, yC, 0.0, yB, zB);

	// Create triangles in strips between the previous and target Y/Z values
	int nBit, nPlusOneBit, totalBit;
	int vertexAn = -1, vertexBn = -1, vertexAnPlusOne = -1, vertexBnPlusOne = -1;
	Primitive* currentPrimitive = primitiveList[0];
	for (int index = minIndex.z+1; index <=maxIndex.z; ++index)
	{
		// Grab next data (if we are not at the end of the index range)
		if (index < maxIndex.z)
		{
			yC.copy(displayData[index+1]->y(), minIndex.x, maxIndex.x);
			typeC.copy(displayData[index+1]->yType(), minIndex.x, maxIndex.x);
			axes.transformY(yC, typeC);
			zC = axes.transformZ(displayData[index+1]->z());
		}
		else yC.clear();

		// Construct data for current slice
		constructSurfaceStrip(x, yB, zB, axes, normB, colourB, colourScale, yA, zA, yC, zC);

		// Use a simple bit to quickly determine which triangles to draw, given possible lack of datapoints in slices
		//
		//			n	n+1	n	n+1
		//	Slice A		4-------1	4-------1	0 = Both	5 = None
		//			| ....TR|	|TL.... |	1 = BL		6 = None
		//			|   ....|	|....   |	2 = TL		7 = None
		//			|BL   ..|	|..   BR|	3 = None	8 = TR
		//	Slice B		8-------2	8-------2	4 = BR		9+= None

		// Set initial bit, and generate initial vertices
		nBit = 0;
		if (typeA.constAt(0) == DisplayDataSet::NoPoint)
		{
			nBit += 4;
			vertexAn = -1;
		}
		else vertexAn = currentPrimitive->defineVertex(x.constAt(0), yA.constAt(0), zA, normA[0], colourA[0]);
		if (typeB.constAt(0) == DisplayDataSet::NoPoint)
		{
			nBit += 8;
			vertexBn = -1;
		}
		else vertexBn = currentPrimitive->defineVertex(x.constAt(0), yB.constAt(0), zB, normB[0], colourB[0]);

		for (int n=0; n<nX-1; ++n)
		{
			// Construct bit for n+1
			nPlusOneBit = 0;
			if (typeA.constAt(n+1) == DisplayDataSet::NoPoint) nPlusOneBit += 1;
			if (typeB.constAt(n+1) == DisplayDataSet::NoPoint) nPlusOneBit += 2;
			totalBit = nBit + nPlusOneBit;

			// Reset indices for current (n+1) column
			vertexAnPlusOne = -1;
			vertexBnPlusOne = -1;

			// Add triangles for this quadrant
			if (totalBit == 0)
			{
				// Draw both
				if (vertexAn == -1) vertexAn = currentPrimitive->defineVertex(x.constAt(n), yA.constAt(n), zA, normA[n], colourA[n]);
				if (vertexBn == -1) vertexBn = currentPrimitive->defineVertex(x.constAt(n), yB.constAt(n), zB, normB[n], colourB[n]);
				vertexAnPlusOne = currentPrimitive->defineVertex(x.constAt(n+1), yA.constAt(n+1), zA, normA[n+1], colourA[n+1]);
				vertexBnPlusOne = currentPrimitive->defineVertex(x.constAt(n+1), yB.constAt(n+1), zB, normB[n+1], colourB[n+1]);
				currentPrimitive->defineIndices(vertexAn, vertexAnPlusOne, vertexBnPlusOne);
				currentPrimitive->defineIndices(vertexAn, vertexBn, vertexBnPlusOne);
			}
			else if (totalBit == 1)
			{
				// Bottom left corner only
				if (vertexAn == -1) vertexAn = currentPrimitive->defineVertex(x.constAt(n), yA.constAt(n), zA, normA[n], colourA[n]);
				if (vertexBn == -1) vertexBn = currentPrimitive->defineVertex(x.constAt(n), yB.constAt(n), zB, normB[n], colourB[n]);
				vertexBnPlusOne = currentPrimitive->defineVertex(x.constAt(n+1), yB.constAt(n+1), zB, normB[n+1], colourB[n+1]);
				currentPrimitive->defineIndices(vertexAn, vertexBnPlusOne, vertexBn);
			}
			else if (totalBit == 2)
			{
				// Top left corner only
				if (vertexAn == -1) vertexAn = currentPrimitive->defineVertex(x.constAt(n), yA.constAt(n), zA, normA[n], colourA[n]);
				if (vertexBn == -1) vertexBn = currentPrimitive->defineVertex(x.constAt(n), yB.constAt(n), zB, normB[n], colourB[n]);
				vertexAnPlusOne = currentPrimitive->defineVertex(x.constAt(n+1), yA.constAt(n+1), zA, normA[n+1], colourA[n]);
				currentPrimitive->defineIndices(vertexAn, vertexAnPlusOne, vertexBn);
			}
			else if (totalBit == 4)
			{
				// Bottom right corner only
				if (vertexBn == -1) vertexBn = currentPrimitive->defineVertex(x.constAt(n), yB.constAt(n), zB, normB[n], colourB[n]);
				vertexAnPlusOne = currentPrimitive->defineVertex(x.constAt(n+1), yA.constAt(n+1), zA, normA[n+1], colourA[n+1]);
				vertexBnPlusOne = currentPrimitive->defineVertex(x.constAt(n+1), yB.constAt(n+1), zB, normB[n+1], colourB[n+1]);
				currentPrimitive->defineIndices(vertexAnPlusOne, vertexBnPlusOne, vertexBn);
			}
			else if (totalBit == 8)
			{
				// Top right corner only
				if (vertexAn == -1) vertexAn = currentPrimitive->defineVertex(x.constAt(n), yA.constAt(n), zA, normA[n], colourA[n]);
				vertexAnPlusOne = currentPrimitive->defineVertex(x.constAt(n+1), yA.constAt(n+1), zA, normA[n+1], colourA[n+1]);
				vertexBnPlusOne = currentPrimitive->defineVertex(x.constAt(n+1), yB.constAt(n+1), zB, normB[n+1], colourB[n+1]);
				currentPrimitive->defineIndices(vertexAn, vertexAnPlusOne, vertexBnPlusOne);
			}

			// Store new nBit for next index
			nBit = nPlusOneBit*4;
			vertexAn = vertexAnPlusOne;
			vertexBn = vertexBnPlusOne;
		}

		// Shuffle data backwards...
		yA = yB;
		zA = zB;
		typeA = typeB;
		normA = normB;
		colourA = colourB;
		yB = yC;
		zB = zC;
		typeB = typeC;
		
		// Increment primitive pointer
		currentPrimitive = currentPrimitive->next;
	}
}
