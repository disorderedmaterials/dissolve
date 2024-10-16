// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "graphModel.h"
#include "nodeWrapper.h"

class nodeValue
{
    public:
    nodeValue(QVariant var = {});
    std::string name;
    std::variant<double, nodeValue *> value;
};

typedef GraphModel<nodeValue> ExampleGraphModel;
