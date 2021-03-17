// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/module.h"
#include "module/module.h"

ModuleKeywordBase::ModuleKeywordBase(std::string_view moduleType) { moduleType_ = moduleType; }

ModuleKeywordBase::~ModuleKeywordBase() {}

/*
 * Target Module Type
 */

// Return target Module type to allow
std::string_view ModuleKeywordBase::moduleType() const { return moduleType_; }
