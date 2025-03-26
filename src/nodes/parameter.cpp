// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/parameter.h"
#include "nodes/node.h"

ParameterBase::ParameterBase(Node *parent, std::string_view name, std::string_view description, std::type_index type)
    : parent_(parent), name_(name), description_(description), type_(type)
{
}

/*
 * Definition
 */

// Return the parameter name
std::string_view ParameterBase::name() const { return name_; }

// Return the parameter description
std::string_view ParameterBase::description() const { return description_; }

// Return the parameter type
std::type_index ParameterBase::type() const { return type_; }

// Return the owner of the parameter
Node *ParameterBase::parent() const { return parent_; }

// Set flag(s) for the parameter
void ParameterBase::setFlags(const Flags<ParameterBase::ParameterFlags> &flags) { flags_ += flags; }

// Return current flags
const Flags<ParameterBase::ParameterFlags> &ParameterBase::flags() const { return flags_; }

/*
 * Data
 */

// Invalidate the parent node (e.g. because our value has changed and we are an Invalidating parameter)
void ParameterBase::invalidateParent() const { return parent_->invalidate(); }
