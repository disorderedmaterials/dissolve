// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <memory>
#include <vector>

class GeneratorNode;
using NodeRef = std::shared_ptr<GeneratorNode>;
using ConstNodeRef = std::shared_ptr<const GeneratorNode>;

template <class N> using ConstNodeVector = std::vector<std::shared_ptr<const N>>;
