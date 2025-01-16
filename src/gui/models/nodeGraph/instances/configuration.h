// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "gui/models/nodeGraph/graphNodeContext.h"

std::string nodeName(const Configuration *value);
std::string nodeTypeName(const Configuration *value);
std::string nodeTypeIcon(const Configuration *value);

bool nodeConnect(Configuration *source, int sourceIndex, Configuration *destination, int destinationIndex);
bool nodeConnectable(const Configuration *source, int sourceIndex, const Configuration *destination, int destinationIndex);
bool nodeDisconnect(Configuration *source, int sourceIndex, Configuration *destination, int destinationIndex);
bool nodeDelete(Configuration *item, CoreData *coreData);
QVariant nodeData(const Configuration *value, int role);
bool nodeSetData(Configuration *item, const QVariant &value, int role);
