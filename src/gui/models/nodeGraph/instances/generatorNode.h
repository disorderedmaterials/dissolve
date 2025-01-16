// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QVariant>
#include "gui/models/nodeGraph/phantom.h"
#include "classes/coreData.h"

class GeneratorNode;

std::string nodeName(GeneratorNode *const &value);
std::string nodeTypeName(GeneratorNode *const &value);
std::string nodeTypeIcon(GeneratorNode *const &value);
bool nodeDelete(GeneratorNode *item, CoreData *coreData);
QVariant nodeData(const GeneratorNode *value, int role);
bool nodeSetData(GeneratorNode *item, const QVariant &value, int role);
