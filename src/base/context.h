// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include <variant>

template <typename T> struct Context
{
    using type = std::monostate;
};
