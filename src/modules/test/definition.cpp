// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/test/test.h"

// Return type of module
std::string_view TestModule::type() const { return "Test"; }
