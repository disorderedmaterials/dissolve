// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/coreData.h"
#include "gui/models/nodeGraph/phantom.h"
#include <QVariant>

class GeneratorNode;

std::string nodeName(GeneratorNode *const &value);
std::string nodeTypeName(GeneratorNode *const &value);
std::string nodeTypeIcon(GeneratorNode *const &value);
bool nodeDelete(GeneratorNode *item, CoreData *coreData);
QVariant nodeData(const GeneratorNode *value, int role);
bool nodeSetData(GeneratorNode *item, const QVariant &value, int role);
