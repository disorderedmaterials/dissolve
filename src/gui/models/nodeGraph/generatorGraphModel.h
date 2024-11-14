// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "classes/coreData.h"
#include "graphModel.h"
#include "gui/models/configurationModel.h"
#include "gui/models/nodeGraph/instances/all.h"
#include "nodeWrapper.h"

using GeneratorGraphInnerType = std::variant<Configuration *, Generator *, GeneratorNode *>;

class GeneratorGraphNode
{
    public:
    GeneratorGraphNode(QVariant var = {});
    GeneratorGraphInnerType value;
};

template <> struct GraphNodeContext<GeneratorGraphNode>
{
    using type = CoreData *;
};

class GeneratorGraphModel : public GraphModel<GeneratorGraphNode>
{
    Q_OBJECT
    Q_PROPERTY(ConfigurationModel *world READ world WRITE setWorld);

    public:
    enum PropertyIndex
    {
        Value = 0,
        Size,
        Temperature,
        AtomicDensity,
    };

    public Q_SLOTS:
    void handleReset();

    private:
    // Dissolve Model Getter
    ConfigurationModel *world();
    // Dissolve Model Setter
    void setWorld(ConfigurationModel *value);
    ConfigurationModel *world_;
    void emplace_back(int x, int y, GeneratorGraphInnerType value);
};
