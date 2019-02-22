/*
	*** Rendering Primitive Assembly
	*** src/gui/viewer/render/primitiveassembly.h
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

#ifndef DISSOLVE_PRIMITIVEASSEMBLY_H
#define DISSOLVE_PRIMITIVEASSEMBLY_H

#include "gui/viewer/render/primitiveinfo.h"
#include "templates/array.h"

// Forward Declarations
/* none */

// Rendering Primitive List
class PrimitiveAssembly
{
	public:
	// Constructor / Destructor
	PrimitiveAssembly();
	~PrimitiveAssembly();


	/*
	 * Data
	 */
	private:
	// Array of PrimitiveInfo that constitute our assembly
	Array<PrimitiveInfo> assembly_;

	public:
	// Clear existing data
	void clear();
	// Forget all data, leaving array intact
	void forgetAll();
	// Add PrimitiveInfo to the assembly
	void add(PrimitiveInfo info);


	/*
	 * GL
	 */
	public:
	// Send to OpenGL (i.e. render)
	void sendToGL();
};

#endif
