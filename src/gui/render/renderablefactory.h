// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "gui/render/renderable.h"

// Forward Declarations
/* none */

class RenderableFactory
{
    public:
    // Create Renderable of specified type, from specified data (referenced by objectTag)
    static Renderable *create(Renderable::RenderableType renderableType, std::string_view objectTag);
};
