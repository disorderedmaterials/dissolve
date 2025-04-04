// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/dissolve.h"

DissolveGraph::DissolveGraph(Dissolve &dissolve) : Graph(nullptr), dissolve_(dissolve) {}

// Return dissolve
Dissolve &DissolveGraph::dissolve() const { return dissolve_; }

// Return world pool
const ProcessPool &DissolveGraph::processPool() const { return dissolve_.worldPool(); }
