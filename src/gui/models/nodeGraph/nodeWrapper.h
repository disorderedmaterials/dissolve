// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"
#include <QAbstractListModel>

// A wrapper with supplemental information for a node
class NodeWrapper
{
    public:
    NodeWrapper(Node &value) : value_(value) {}
    // The X and Y coordinates of the node on the screen.
    int posx, posy;

    // Get the actual value of the node
    Node &rawValue() { return value_; }
    const Node &rawValue() const { return value_; }

    private:
    // The actual value of the node
    Node &value_;
};
