/*
	*** Renderable Factory
	*** src/gui/viewer/render/renderablefactory.cpp
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

#include "gui/viewer/render/renderablefactory.h"
#include "gui/viewer/render/renderabledata1d.h"

// Create Renderable of specified type
Renderable* RenderableFactory::create(Renderable::RenderableType renderableType, const char* objectTag)
{
	Renderable* renderable = NULL;
	Data1D* data1d;

	if (renderableType == Renderable::Data1DRenderable) renderable = new RenderableData1D(Data1D::findObject(objectTag), objectTag);
	else Messenger::error("Don't know how to create a Renderable of type '%s' (object tag = '%s'.\n", Renderable::renderableType(renderableType), objectTag);

	return renderable;
}
