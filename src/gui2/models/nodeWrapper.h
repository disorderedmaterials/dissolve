// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "gui2/models/parameterModel.h"
#include "nodes/node.h"
#include <QAbstractListModel>
#include <QPointF>
#include <map>

// A wrapper with supplemental information for a node
class NodeWrapper
{
    public:
    NodeWrapper(Node *node)
        : node_(node), inputs(std::make_unique<ParameterModel>(node->inputs())),
          outputs(std::make_unique<ParameterModel>(node->outputs())), options(std::make_unique<ParameterModel>(node->options()))
    {
    }

    // Parameter models for parameters of the node
    std::unique_ptr<ParameterModel> inputs, outputs, options;
    // Relative positions of parameters with respect to the node
    std::map<std::string, QPointF> inputsPos, outputPos;

    public:
    // Return the wrapped node
    Node *node() { return node_; }
    const Node *node() const { return node_; }
    // Does this node contain other nodes?
    bool hasInner() { return dynamic_cast<Graph *>(node_) != nullptr; }

    private:
    // Pointer to the node we're wrapping
    Node *node_;
};
