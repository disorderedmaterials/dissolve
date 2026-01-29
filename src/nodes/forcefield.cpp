// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "forcefield.h"
#include "data/ff/library.h"
#include <toml11/toml.hpp>
#include <toml11/toml/parser.hpp>

ForcefieldNode::ForcefieldNode(Graph *parentGraph) : Node(parentGraph)
{
    addOutput<std::shared_ptr<Forcefield>>("Forcefield", "Created forcefield", ff_);
}

std::string_view ForcefieldNode::type() const { return "Forcefield"; }

std::string_view ForcefieldNode::summary() const { return "Produce a forcefield"; }

NodeConstants::ProcessResult ForcefieldNode::process() { return NodeConstants::ProcessResult::Unchanged; }

std::shared_ptr<Forcefield> &ForcefieldNode::forcefield() { return ff_; }
const std::shared_ptr<Forcefield> &ForcefieldNode::forcefield() const { return ff_; }

// Serialise any hidden content
void ForcefieldNode::serialiseInternal(SerialisedValue &target) const { target["forcefield"] = ff_->name(); }

// Deserialise any hidden content
void ForcefieldNode::deserialiseInternal(const SerialisedValue &node)
{
    ff_ = ForcefieldLibrary::forcefield(std::string_view(node.at("forcefield").as_string()));
}
