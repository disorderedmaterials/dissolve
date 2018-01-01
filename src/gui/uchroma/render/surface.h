/*
	*** Surface Creation
	*** src/gui/uchroma/render/surface.h
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

#ifndef DUQ_UCHROMASURFACE_H
#define DUQ_UCHROMASURFACE_H

#include "gui/uchroma/render/primitivelist.h"
#include "gui/uchroma/classes/displaydataset.h"
#include "gui/uchroma/classes/colourscale.h"

// Forward Declarations
class Axes;

// Surface Generation
class Surface
{
	private:
	// Construct normals for requested vertex TODO
	static Vec3<double> constructVertexNormals(const Array<double>& abscissa, int index, DisplayDataSet* targetDataSet, DisplayDataSet* previousDataSet, DisplayDataSet* nextDataSet, int nPoints);
	// Construct normal / colour surface data for dataset specified
	static void constructSurfaceStrip(const Array<double>& x, const Array<double>& y, double z, const Axes& axes, Array< Vec3<double> >& normals, Array< Vec4<GLfloat> >& colours, const ColourScale& colourScale, const Array<double>& yPrev, double zPrev, const Array<double>& yNext, double zNext);
	// Calculate integer index extents for display data given supplied axes
	static bool calculateExtents(const Axes& axes, const Array<double>& abscissa, List<DisplayDataSet>& displayData, Vec3<int>& minIndex, Vec3<int>& maxIndex);

	public:
	// Construct line surface representation of data in XY slices
	static void constructLineXY(PrimitiveList& primitiveList, const Axes& axes, const Array<double>& displayAbscissa, List<DisplayDataSet>& displayData, ColourScale colourScale);
	// Construct line surface representation of data in ZY slices
	static void constructLineZY(PrimitiveList& primitiveList, const Axes& axes, const Array<double>& displayAbscissa, List<DisplayDataSet>& displayData, ColourScale colourScale);
	// Construct line surface representation of data
	static void constructGrid(PrimitiveList& primitiveList, const Axes& axes, const Array<double>& displayAbscissa, List<DisplayDataSet>& displayData, ColourScale colourScale);
	// Construct full surface representation of data
	static void constructFull(PrimitiveList& primitiveList, const Axes& axes, const Array<double>& displayAbscissa, List<DisplayDataSet>& displayData, ColourScale colourScale);
};

#endif
