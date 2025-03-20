// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "add.h"
#include "atomShake.h"
#include "derivative.h"
#include "dissolve.h"
#include "dotProduct.h"
#include "integrator.h"
#include "multiply.h"
#include "node.h"
#include "subtract.h"
#include "vec3Assembly.h"
#include "vec3Decomposition.h"
#include <functional>
#include <map>
#include <memory>
#include <string>

using NodeProducer = std::function<std::unique_ptr<Node>(void)>;

// Makes unique pointer to derived node instance
template <typename T> NodeProducer makeDerivedNode()
{
    auto nodeLambda = []() -> std::unique_ptr<Node> { return std::make_unique<T>(); };
    return nodeLambda;
}

// Node registry
const std::map<std::string_view, NodeProducer> registry{
    {"AddDouble", makeDerivedNode<AddDouble>()},
    {"AddInt", makeDerivedNode<AddInt>()},
    {"AtomShake", makeDerivedNode<AtomShakeNode>()},
    {"Derivative", makeDerivedNode<DerivativeNode>()},
    {"Dissolve", makeDerivedNode<DissolveNode>()},
    {"DotProduct", makeDerivedNode<DotProductNode>()},
    {"Integrator", makeDerivedNode<Integrator1DNode>()},
    {"MultiplyDouble", makeDerivedNode<MultiplyDouble>()},
    {"MultiplyInt", makeDerivedNode<MultiplyInt>()},
    {"SubtractDouble", makeDerivedNode<SubtractDouble>()},
    {"SubtractInt", makeDerivedNode<SubtractInt>()},
    {"Vec3Assembly", makeDerivedNode<Vec3AssemblyNode>()},
    {"Vec3Decomposition", makeDerivedNode<Vec3DecompositionNode>()},
    // etc...
};
