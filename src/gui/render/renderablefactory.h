/*
	*** Renderable Factory
	*** src/gui/render/renderablefactory.cpp
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

#ifndef DISSOLVE_RENDER_RENDERABLEFACTORY_H
#define DISSOLVE_RENDER_RENDERABLEFACTORY_H

#include "gui/render/renderable.h"

// Forward Declarations
/* none */

class RenderableFactory
{
	public:
	// Create Renderable of specified type, from specified data (referenced by objectTag)
	static Renderable* create(Renderable::RenderableType renderableType, const char* objectTag);
};

#endif
