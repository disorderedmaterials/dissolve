// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "graphModel.h"
#include "gui/models/configurationModel.h"
#include "nodeWrapper.h"

class GeneratorGraphNode
{
    public:
    GeneratorGraphNode(QVariant var = {});
    std::variant<Configuration *, GeneratorGraphNode *> value;
};

class GeneratorGraphModel : public GraphModel<GeneratorGraphNode>
{
    Q_OBJECT
    Q_PROPERTY(ConfigurationModel *world READ world WRITE setWorld);

    public Q_SLOTS:
    void handleReset();

    private:
    // Dissolve Model Getter
    ConfigurationModel *world();
    // Dissolve Model Setter
    void setWorld(ConfigurationModel *value);
    ConfigurationModel *world_;
};
