// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/dissolve.h"

DissolveGraph::DissolveGraph(Dissolve &dissolve) : Graph(nullptr), dissolve_(dissolve) {}

/*
 * Definitions (Virtuals)
 */

// Return node name
std::string_view DissolveGraph::name() const { return "Root"; }

// Return type of the node
std::string_view DissolveGraph::type() const { return "Dissolve"; }

// Return short summary of the node's purpose
std::string_view DissolveGraph::summary() const { return "Parent node of all simulations"; }

// Return dissolve
Dissolve &DissolveGraph::dissolve() const { return dissolve_; }

// Return world pool
const ProcessPool &DissolveGraph::processPool() const { return dissolve_.worldPool(); }
