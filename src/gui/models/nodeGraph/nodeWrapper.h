// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

// #include "gui/models/nodeGraph/parameterModel.h"
#include "gui/models/nodeGraph/parameterModel.h"
#include "nodes/node.h"
#include <QAbstractListModel>
#include <QPointF>
#include <map>

// A wrapper with supplemental information for a node
class NodeWrapper
{
    public:
    NodeWrapper(Node &value)
        : value_(&value), inputs(std::make_unique<ParameterModel>(value.inputs())),
          outputs(std::make_unique<ParameterModel>(value.outputs())), options(std::make_unique<ParameterModel>(value.options()))
    {
    }

    // Parameter models for parameters of the node
    std::unique_ptr<ParameterModel> inputs, outputs, options;
    // Relative positions of parameters with respect to the node
    std::map<std::string, QPointF> inputsPos, outputPos;

    // Get the actual value of the node
    Node &rawValue() { return *value_; }
    const Node &rawValue() const { return *value_; }

    // Does this node contain other nodes?
    bool hasInner() { return dynamic_cast<Graph *>(value_) != nullptr; }

    private:
    // The actual value of the node
    Node *value_;
};
