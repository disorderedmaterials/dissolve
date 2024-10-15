// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "graphModel.h"
#include "nodeWrapper.h"

// helper type for the visitor #4
template <class... Ts> struct overloaded : Ts...
{
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

class nodeValue
{
    public:
    nodeValue(QVariant var = {});
    std::string name;
    std::variant<double, nodeValue *> value;
};

typedef GraphModel<nodeValue> ExampleGraphModel;
