// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "graphModel.h"
#include "nodeWrapper.h"

/**
   This model is for an example graph model that mostly exists as a
   proof of concept of the node graph framework.  It's used in the
   testing, but should not be used at any place in Dissolve proper.
   Once one of the production node graphs is completed, this example
   model can likely be deleted.

 **/

class CoreData;

// The contents of the example node
class nodeValue
{
    public:
    nodeValue(QVariant var = {});

    // The name of the node
    std::string name;
    // The actual data within the node
    std::variant<double, nodeValue *> value;
};

std::string nodeName(const nodeValue &value);
std::string nodeTypeName(const nodeValue &value);
std::string nodeTypeIcon(const nodeValue &value);
void setNodeName(nodeValue &value, std::string);
QVariant nodeGetValue(const nodeValue &value);
bool nodeConnect(nodeValue &source, int sourceIndex, nodeValue &destionation, int destinationIndex);
bool nodeConnectable(const nodeValue &source, int sourceIndex, const nodeValue &destination, int destinationIndex);
bool nodeDisconnect(nodeValue &source, int sourceIndex, nodeValue &destination, int destinationIndex);
QHash<int, QByteArray> &nodeRoleNames(Proxy<nodeValue> proxy, QHash<int, QByteArray> &roles);
bool nodeDelete(nodeValue &value, Proxy<nodeValue> &context);

// The graph model for the example
typedef GraphModel<nodeValue, Proxy<nodeValue>> ExampleGraphModel;
