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
std::string nodeTypeName(Configuration *const &value);
std::string nodeTypeIcon(Configuration *const &value);

bool nodeConnect(Configuration *&source, int sourceIndex, Configuration *&destionation, int destinationIndex);
bool nodeConnectable(const Configuration *&source, int sourceIndex, const Configuration *&destination, int destinationIndex);
bool nodeDisconnect(Configuration *&source, int sourceIndex, Configuration *&destination, int destinationIndex);
