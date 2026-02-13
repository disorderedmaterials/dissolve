// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/speciesSite.h"
#include "constants.h"

SpeciesSiteNode::SpeciesSiteNode(Graph *parentGraph) : Node(parentGraph), speciesSite_(species_, type_)
{
  addInput("Species", "Species on which the site is located", species_);
  addOption("Type", "The kind of site", type_);
  addPointerOutput("SpeciesSite", "The Species Site", speciesSite_);
}

std::string_view SpeciesSiteNode::type() const { return "SpeciesSite"; }
std::string_view SpeciesSiteNode::summary() const { return "Species Site"; }

NodeConstants::ProcessResult SpeciesSiteNode::process()
{
    if (!species_)
      return NodeConstants::ProcessResult::Failed;
    speciesSite_ = SpeciesSite(species_, type_);
    return NodeConstants::ProcessResult::Success;
}
