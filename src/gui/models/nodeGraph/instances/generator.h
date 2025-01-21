// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "generator/generator.h"
#include "gui/models/nodeGraph/phantom.h"

std::string nodeName(Generator *const &value);
std::string nodeTypeName(Generator *const &value);
std::string nodeTypeIcon(Generator *const &value);
bool nodeDelete(Generator *item, CoreData *coreData);
QVariant nodeData(Generator *value, int role);
bool nodeSetData(Generator *item, const QVariant &value, int role);
