/*
	*** Rendering Primitive Assembly
	*** src/gui/viewer/render/primitiveassembly.cpp
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

#include "gui/viewer/render/primitiveassembly.h"

// Constructor
PrimitiveAssembly::PrimitiveAssembly()
{
}

// Destructor
PrimitiveAssembly::~PrimitiveAssembly()
{
}

// Clear all existing primitives
void PrimitiveAssembly::clear()
{
	assembly_.clear();
}

// Forget all data, leaving arrays intact
void PrimitiveAssembly::forgetAll()
{
	assembly_.forgetData();
}

// Add PrimitiveInfo to the assembly
void PrimitiveAssembly::add(PrimitiveInfo info)
{
	assembly_.add(info);
}

/*
 * GL
 */

// Send to OpenGL (i.e. render)
void PrimitiveAssembly::sendToGL()
{
	for (int n=0; n<assembly_.nItems(); ++n) assembly_[n].sendToGL();
}
