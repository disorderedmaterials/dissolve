// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/avgmol/avgmol.h"
#include "nodes/constants.h"

AvgMolNode::AvgMolNode(Graph *parentGraph) : Node(parentGraph) {}

std::string_view AvgMolNode::type() const { return "AvgMol"; }

std::string_view AvgMolNode::summary() const { return "Calculate Average Molecule"; }

NodeConstants::ProcessResult AvgMolNode::process() { return NodeConstants::ProcessResult::Failed; }
