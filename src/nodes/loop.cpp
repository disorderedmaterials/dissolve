// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/loop.h"

LoopGraph::LoopGraph(Graph *parentGraph) : Graph(parentGraph) {}

/*
 * Definitions (Virtuals)
 */

// Return node name
std::string_view LoopGraph::name() const { return "Loop"; }

// Return type of the node
std::string_view LoopGraph::type() const { return "Loop"; }

// Return short summary of the node's purpose
std::string_view LoopGraph::summary() const { return "Loop the contained graph"; }
