// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/subGraph.h"
#include "nodes/edge.h"

SubGraph::SubGraph(Graph *parentGraph) : Graph(parentGraph), inputsNode_(this, inputs_) {}

/*
 * Definition
 */

// Return type of the node
std::string_view SubGraph::type() const { return "Graph"; }

// Return short summary of the node's purpose
std::string_view SubGraph::summary() const { return "A node which contains its own inner graph"; }
