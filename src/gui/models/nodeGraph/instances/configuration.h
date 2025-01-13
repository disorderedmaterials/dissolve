// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "gui/models/nodeGraph/graphNodeContext.h"

// Configurations need access to the CoreData to access all of their
// children.
template <> struct GraphNodeContext<Configuration *>
{
    using type = CoreData *;
};

std::string nodeName(Configuration *const &value);
