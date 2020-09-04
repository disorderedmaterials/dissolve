/*
    *** Renderable Factory
    *** src/gui/render/renderablefactory.cpp
    Copyright T. Youngs 2013-2020

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

#include "gui/render/renderablefactory.h"
#include "gui/render/renderabledata1d.h"
#include "gui/render/renderabledata2d.h"
#include "gui/render/renderabledata3d.h"
#include "gui/render/renderablespecies.h"

// Create Renderable of specified type
Renderable *RenderableFactory::create(Renderable::RenderableType renderableType, std::string_view objectTag)
{
    Renderable *renderable = nullptr;

    if (renderableType == Renderable::Data1DRenderable)
        renderable = new RenderableData1D(Data1D::findObject(objectTag), objectTag);
    else if (renderableType == Renderable::Data2DRenderable)
        renderable = new RenderableData2D(Data2D::findObject(objectTag), objectTag);
    else if (renderableType == Renderable::Data3DRenderable)
        renderable = new RenderableData3D(Data3D::findObject(objectTag), objectTag);
    else if (renderableType == Renderable::SpeciesRenderable)
        renderable = new RenderableSpecies(Species::findObject(objectTag), objectTag);
    else
        fmt::print("Don't know how to create a Renderable of type '{}' (object tag = '{}').\n",
                   Renderable::renderableTypes().keyword(renderableType), objectTag);

    return renderable;
}
