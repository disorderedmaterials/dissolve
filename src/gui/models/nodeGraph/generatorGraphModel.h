// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "generator/add.h"
#include "graphModel.h"
#include "nodeWrapper.h"

class GeneratorGraphNode
{
    public:
    GeneratorGraphNode(QVariant var = {});
    std::variant<AddGeneratorNode *, GeneratorGraphNode *> value;
};

typedef GraphModel<GeneratorGraphNode> GeneratorGraphModel;
