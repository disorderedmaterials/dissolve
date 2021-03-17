// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/render/renderablefactory.h"
#include "gui/render/renderabledata1d.h"
#include "gui/render/renderabledata2d.h"
#include "gui/render/renderabledata3d.h"
#include "gui/render/renderablespecies.h"

// Create Renderable of specified type
std::shared_ptr<Renderable> RenderableFactory::create(Renderable::RenderableType renderableType, std::string_view objectTag)
{
    std::shared_ptr<Renderable> renderable = nullptr;

    if (renderableType == Renderable::Data1DRenderable)
        renderable = std::make_shared<RenderableData1D>(Data1D::findObject(objectTag), objectTag);
    else if (renderableType == Renderable::Data2DRenderable)
        renderable = std::make_shared<RenderableData2D>(Data2D::findObject(objectTag), objectTag);
    else if (renderableType == Renderable::Data3DRenderable)
        renderable = std::make_shared<RenderableData3D>(Data3D::findObject(objectTag), objectTag);
    else
        fmt::print("Don't know how to create a Renderable of type '{}' (object tag = '{}').\n",
                   Renderable::renderableTypes().keyword(renderableType), objectTag);

    return renderable;
}
