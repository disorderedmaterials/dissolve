// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "classes/coreData.h"
#include "graphModel.h"
#include "gui/models/dissolveModel.h"
#include "gui/models/nodeGraph/instances/all.h"
#include "nodeWrapper.h"

// The variant of all of the types that we will examine
using GeneratorGraphInnerType = std::variant<Configuration *, Generator *, GeneratorNode *>;

// A class to contain the inner type, since we need a constructor that
// take a QVariant
class GeneratorGraphNode
{
    public:
    GeneratorGraphNode(QVariant var = {});
    GeneratorGraphInnerType value;
};

// All of these types may require access to CoreData
template <> struct GraphNodeContext<GeneratorGraphNode>
{
    using type = CoreData *;
};

// A graph node model for looking at the generators on a configuration
class GeneratorGraphModel : public GraphModel<GeneratorGraphNode>
{
    Q_OBJECT
    // The Dissolve Model that contains the Dissolve object instance we
    // are using
    Q_PROPERTY(DissolveModel *world READ world WRITE setWorld);

    public:
    // An enumeration of the different properties that we might access
    // from a node.
    enum PropertyIndex
    {
        Value = 0,
        Size,
        Temperature,
        AtomicDensity,
    };

    public Q_SLOTS:
    // Clear model and repopulate data from Dissolve
    void handleReset();

    public:
    // Dissolve Model Getter
    DissolveModel *world();
    // Dissolve Model Setter
    void setWorld(DissolveModel *value);
    // Add a node to the model as a specific location
    void emplace_back(int x, int y, GeneratorGraphInnerType value);

    private:
    // The Dissolve Model that contains the Dissolve object instance we
    // are using
    DissolveModel *world_;
};
