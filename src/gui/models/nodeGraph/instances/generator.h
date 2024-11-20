// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "generator/generator.h"
#include "gui/models/nodeGraph/nodeWrapper.h"

template <> struct GraphNodeContext<Generator *>
{
    using type = CoreData *;
};
