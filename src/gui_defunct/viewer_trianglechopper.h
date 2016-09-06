/*
	*** Triangle Storage Class
	*** src/gui/viewer_trianglechopper.h
	Copyright T. Youngs 2012-2013

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_TRIANGLES_H
#define DUQ_TRIANGLES_H

#include "gui/viewer_primitive.h"
#include "math/matrix4.h"

// Forward Declarations
class PrimitiveInfo;

// Triangle Chopper
class TriangleChopper
{
	public:
	// Constructor / Destructor
	TriangleChopper();
	~TriangleChopper();

	private:
	// Starting z-depth of chopper
	double startZ_;
	// Number of slices
	int nSlices_;
	// Slice width
	double sliceWidth_;
	// Triangle Lists
	Primitive* triangleLists_;
	// Clear all existing trianglelists
	void clear();
	
	public:
	// Initialise structure
	void initialise(double startz, int nbins, double slicewidth);
	// Empty all stored triangles, but retain storage
	void emptyTriangles();
	// Store primitive's triangles
	void storeTriangles(PrimitiveInfo* pinfo, Matrix4& transform);
	// Sent triangles to GL (in correct order)
	void sendToGL();
};

#endif
