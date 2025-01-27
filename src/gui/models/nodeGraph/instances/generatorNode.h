// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/coreData.h"
#include "gui/models/nodeGraph/phantom.h"
#include <QVariant>

class GeneratorNode;

std::string nodeName(GeneratorNode *const &value);
void setNodeName(GeneratorNode *value, std::string);
std::string nodeTypeName(GeneratorNode *const &value);
std::string nodeTypeIcon(GeneratorNode *const &value);
bool nodeDelete(GeneratorNode *item, CoreData *coreData);
QVariant nodeData(const GeneratorNode *value, int role);
bool nodeSetData(GeneratorNode *item, const QVariant &value, int role);

QVariant nodeGetValue(const GeneratorNode *value);
bool nodeConnect(const GeneratorNode *source, int sourceIndex, const GeneratorNode *destionation, int destinationIndex);
bool nodeConnectable(const GeneratorNode *source, int sourceIndex, const GeneratorNode *destination, int destinationIndex);
bool nodeDisconnect(const GeneratorNode *source, int sourceIndex, const GeneratorNode *destination, int destinationIndex);
QHash<int, QByteArray> &nodeRoleNames(Phantom<GeneratorNode *> proxy, QHash<int, QByteArray> &roles);
