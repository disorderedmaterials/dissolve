// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/parameter.h"
#include "nodes/node.h"

ParameterBase::ParameterBase(Node *parent, std::string_view name, std::string_view description, std::type_index storedDataType,
                             std::type_index contextDataType)
    : parent_(parent), name_(name), description_(description), storedDataType_(storedDataType),
      contextDataType_(contextDataType)
{
}

/*
 * Definition
 */

// Set node parent
void ParameterBase::setParent(Node *parent) { parent_ = parent; }

// Return the parameter name
std::string_view ParameterBase::name() const { return name_; }

// Return the parameter description
std::string_view ParameterBase::description() const { return description_; }

// Return the stored data type
std::type_index ParameterBase::storedDataType() const { return storedDataType_; }

// Return the owner of the parameter
Node *ParameterBase::parent() const { return parent_; }

// Set flag(s) for the parameter
void ParameterBase::setFlags(const Flags<ParameterBase::ParameterFlags> &flags) { flags_ += flags; }

// Remove flag for the parameter
void ParameterBase::removeFlag(ParameterBase::ParameterFlags flag) { flags_ -= flag; }

// Return current flags
const Flags<ParameterBase::ParameterFlags> &ParameterBase::flags() const { return flags_; }

/*
 * Data
 */

// Flag that an update is required in the parent node
void ParameterBase::setParentUpdateRequired() const { parent_->setUpdateRequired(); }

// Clear data in the parent node
void ParameterBase::clearDataInParent() const { parent_->clearData(); }

// Mark edges for re-pull in parent node
void ParameterBase::markIncomingEdgesForPull() const { parent_->markIncomingEdgesForPull(this); }

// Perform any updates after a successful setData()
void ParameterBase::updateAfterSet() const
{
    // Changing parameters always flags an update as being required, unless the NoUpdate flag is set
    if (!flags_.isSet(NoUpdate))
        setParentUpdateRequired();

    // Setting some parameters forces any local data to be cleared
    if (flags_.isSet(ClearData))
        clearDataInParent();
}