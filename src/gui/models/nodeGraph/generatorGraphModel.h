// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "classes/coreData.h"
#include "gui/models/nodeGraph/instances/generatorNode.h"
#include "graphModel.h"
#include "gui/models/dissolveModel.h"
#include "nodeWrapper.h"

Q_DECLARE_METATYPE(GeneratorNode *)

// A graph node model for looking at the generators on a configuration
class GeneratorGraphModel : public GraphModel<GeneratorNode*, CoreData *>
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
        KeywordModel,
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
    void emplace_back(int x, int y, GeneratorNode* value);

    private:
    // The Dissolve Model that contains the Dissolve object instance we
    // are using
    DissolveModel *world_;
};
