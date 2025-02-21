// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "classes/coreData.h"
#include "connection.h"
#include "node.h"
#include "parameter.h"

class Node : public std::enable_shared_from_this<Node>, Serialisable<const CoreData &>
{
    public:
    explicit Node(const std::type_index typeIndex) : attributes_(GraphNode::registry[typeIndex]) {};
    virtual ~Node() = default;

    /*
     * Getters
     */

    // Display name of node
    std::string &displayName() const;
    // Icon url for node
    const QUrl &iconPath() const;
    // Parameters
    GraphNode::ParameterList &parameters() const;

    /*
     * Setters
     */

    // Set display name of node
    void setDisplayName(const std::string &displayName);

    private:
    // Display name of node
    GraphNode::Attributes attributes_;
    // Edge definition
    GraphNode::EdgeDefinition *edgeDefinition_;
};