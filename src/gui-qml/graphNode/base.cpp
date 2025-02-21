// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "base.h"

Node::Node(const std::type_index typeIndex)
{
	auto id = GraphNode::typeIndex[typeIndex];
    attributes_ = GraphNode::registry[id];
}

/*
 * Getters
 */

// Display name of node
std::string &Node::displayName() const
{
	return attributes_.displayName;
}

// Icon url for node
QUrl &Node::iconPath() const 
{ 
	return attributes_.iconPath;
}

// Parameters
Node::ParameterList &Node::parameters() const
{ 
	return attributes_.parameters;
}

/*
 * Setters
 */

// Set display name of node
void &Node::setDisplayName(const std::string &displayName) 
{ 
	attributes_.displayName = displayName;
}