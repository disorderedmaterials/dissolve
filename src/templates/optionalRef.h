// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <memory>
#include <optional>

template <class T> using OptionalReferenceWrapper = std::optional<std::reference_wrapper<T>>;
