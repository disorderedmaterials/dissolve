// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "configuration.h"
#include "generator.h"
#include "generatorNode.h"
#include <QVariant>

// The variant of all of the types that we will examine
using GeneratorGraphInnerType = std::variant<Configuration *, Generator *, GeneratorNode *>;

// A class to contain the inner type, since we need a constructor that
// take a QVariant
class GeneratorGraphNode
{
    public:
    GeneratorGraphNode(QVariant var = {});
    GeneratorGraphInnerType value;
};

std::string nodeName(const GeneratorGraphNode &value);
std::string nodeTypeName(const GeneratorGraphNode &value);
std::string nodeTypeIcon(const GeneratorGraphNode &value);
void setNodeName(GeneratorGraphNode &value, std::string);
QVariant nodeGetValue(const GeneratorGraphNode &value);
bool nodeConnect(GeneratorGraphNode &source, int sourceIndex, GeneratorGraphNode &destionation, int destinationIndex);
bool nodeConnectable(const GeneratorGraphNode &source, int sourceIndex, const GeneratorGraphNode &destination,
                     int destinationIndex);
bool nodeDisconnect(GeneratorGraphNode &source, int sourceIndex, GeneratorGraphNode &destination, int destinationIndex);
QHash<int, QByteArray> &nodeRoleNames(Phantom<GeneratorGraphNode> proxy, QHash<int, QByteArray> &roles);
bool nodeDelete(GeneratorGraphNode &item, CoreData *coreData);
QVariant nodeData(const GeneratorGraphNode &item, int role);
bool nodeSetData(GeneratorGraphNode &item, const QVariant &value, int role);
