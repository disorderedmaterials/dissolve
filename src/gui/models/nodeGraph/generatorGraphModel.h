// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "generator/add.h"
#include "graphModel.h"
#include "gui/models/configurationModel.h"
#include "nodeWrapper.h"

class GeneratorGraphNode
{
    public:
    GeneratorGraphNode(QVariant var = {});
    std::variant<AddGeneratorNode *, GeneratorGraphNode *> value;
};

class GeneratorGraphModel : public GraphModel<GeneratorGraphNode>
{
    Q_OBJECT
    Q_PROPERTY(ConfigurationModel *world READ world WRITE setWorld);

    private:
    // Dissolve Model Getter
    ConfigurationModel *world();
    // Dissolve Model Setter
    void setWorld(ConfigurationModel *value);
    ConfigurationModel *world_;
};
