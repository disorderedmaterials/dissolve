// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "classes/coreData.h"
#include "parameter.h"
#include <map>
#include <string>
#include <type_index>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

class Node;
class Module;
class Generator;
class Vec3Decomposition;

namespace GraphNode
{
// Parameter list type alias
using ParameterList = std::vector<Parameter>;

// Node registry
struct Attributes
{
    const std::string typeName;
    const std::string iconPath;
    std::string displayName; // Defaults to untitled-<typeName>
    ParameterList parameters;
    Attributes(const std::string &typeName, const std::string &iconPath, const ParameterList &parameters, const std::string &displayName = "untitled")
        : typeName(typeName), iconPath(iconPath), parameters(parameters), displayName(displayName + "-" + typeName)
    {
    }
};

const std::unordered_map<std::type_index, Attributes> registry{
    {std::type_info(Module), Attributes("Module", QUrl("EXAMPLE"), ParameterList{})},
    {std::type_info(Generator), Attributes("Generator", QUrl("EXAMPLE"), ParameterList{})},
    {std::type_info(Vec3Decomposition), Attributes("Vec3Decomposition", QUrl("EXAMPLE"), ParameterList{})}
    // etc...
}

// Edge connections
using IncomingConnections = std::vector<IncomingConnection>;
using OutgoingConnections = std::vector<OutgoingConnection>;
using EdgeConnections = std::map<std::string, std::pair<OutgoingConnections, IncomingConnections>>;

struct IncomingConnection
{
    Node *destinationNode;
    Parameter *inputParameter;
    IncomingConnection(Node *destinationNode, Parameter *inputParameter)
        : destinationNode(destinationNode), inputParameter(inputParameter) {};
};

struct OutgoingConnection
{
    Node *sourceNode;
    Parameter *outputParameter;
    OutgoingConnection(Node *sourceNode, Parameter *outputParameter)
        : sourceNode(sourceNode), outputParameter(outputParameter) {};
};

class EdgeDefinition
{
    public:
    EdgeDefinition() {};
    ~EdgeDefinition() = default;

    // Outgoing connections for graph edge (first in pair) by node name
    OutgoingConnections &outgoingConnections(const std::string &nodeName);
    // Incoming connections for graph edge (last in pair) by node name
    IncomingConnections &incomingConnections(const std::string &nodeName);

    private:
    // All connections
    EdgeConnections connections_;
};

}
    