// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/models/nodeGraph/graphNodeContext.h"
#include "classes/coreData.h"

class GeneratorNode;


// GeneratorNodes need access to the CoreData to access all of their
// children.
template <> struct GraphNodeContext<GeneratorNode *>
{
    using type = CoreData *;
};

std::string nodeName(GeneratorNode *const &value);
std::string nodeTypeName(GeneratorNode *const &value);
std::string nodeTypeIcon(GeneratorNode *const &value);

bool nodeDelete(GeneratorNode *item, CoreData *coreData);
