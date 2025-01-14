// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

// A template that we can specialise to associate a a context type
// with a type.
template <typename T> struct GraphNodeContext
{
    using type = GraphNodeContext<void>;
};

template <typename T> struct Proxy
{
};
