// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/primitiveList.h"

PrimitiveList::PrimitiveList() {}

PrimitiveList::~PrimitiveList() {}

// Clear all existing Primitives
void PrimitiveList::clear() { primitives_.clear(); }

// Forget all data, leaving arrays intact
void PrimitiveList::forgetAll()
{
    for (auto &prim : primitives_)
        prim->forgetAll();
}

// Resize list so it is large enough to accommodate specified number of Primitives
void PrimitiveList::reinitialise(int newSize, GLenum type, bool colourData)
{
    // Add enough primitives to match the new size
    primitives_.clear();
    primitives_.reserve(newSize);
    std::generate_n(std::back_inserter(primitives_), newSize, [] { return std::make_unique<Primitive>(); });

    // Loop over all current primitives and set information
    for (auto &prim : primitives_)
        prim->initialise(type, colourData);
}

// Add a new Primitive to the end of the list
Primitive *PrimitiveList::add(GLenum type, bool colourData)
{
    auto *newPrim = primitives_.emplace_back(std::make_unique<Primitive>()).get();
    newPrim->initialise(type, colourData);

    return newPrim;
}

// Register an existing Primitive with the list
void PrimitiveList::add(Primitive *primitive) { primitives_.emplace_back(primitive); }

// Return number of primitives in the list
int PrimitiveList::nPrimitives() const { return primitives_.size(); }

// Remove specified Primitive
void PrimitiveList::remove(Primitive *primitive)
{
    primitives_.erase(
        std::remove_if(primitives_.begin(), primitives_.end(), [primitive](auto &prim) { return prim.get() == primitive; }),
        primitives_.end());
}

// Return total number of defined vertices
int PrimitiveList::nDefinedVertices()
{
    auto totalVertices = 0;
    for (auto &prim : primitives_)
        totalVertices += prim->nDefinedVertices();
    return totalVertices;
}

// Return total number of defined indices
int PrimitiveList::nDefinedIndices()
{
    auto totalIndices = 0;
    for (auto &prim : primitives_)
        totalIndices += prim->nDefinedIndices();
    return totalIndices;
}

// Push instance layer
void PrimitiveList::pushInstance(const QOpenGLContext *context)
{
    for (auto &prim : primitives_)
        prim->pushInstance(context);
}

// Pop topmost instance layer
void PrimitiveList::popInstance(const QOpenGLContext *context)
{
    for (auto &prim : primitives_)
        prim->popInstance(context);
}

// Return number of instances of topmost primitive
int PrimitiveList::nInstances()
{
    if (primitives_.empty())
        return 0;
    else
        return primitives_.front()->nInstances();
}

// Send to OpenGL (i.e. render)
void PrimitiveList::sendToGL()
{
    for (auto &prim : primitives_)
        prim->sendToGL();
}

/*
 * Operators
 */

Primitive *PrimitiveList::operator[](int index) { return primitives_[index].get(); }
