// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "nodeWrapper.h"

// helper type for the visitor #4
template <class... Ts> struct overloaded : Ts...
{
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template <> QVariant nodeGetValue<nodeValue>(const nodeValue value)
{
    return std::visit(overloaded{[](double arg) { return arg; }, [](double *arg) { return *arg; }}, value.value);
}

template <> std::string nodeTypeName<nodeValue>(const nodeValue &value)
{
    return std::visit(overloaded{[](double arg) { return "number"; }, [](double *arg) { return "number"; }}, value.value);
}

template <> std::string nodeTypeIcon<nodeValue>(const nodeValue &value)
{
    return std::visit(overloaded{[](double arg) { return "file:/home/adam/Code/dissolve/src/gui/icons/open.svg"; },
                                 [](double *arg) { return "file:/home/adam/Code/dissolve/src/gui/icons/open.svg"; }},
                      value.value);
}

template <> std::string nodeName<nodeValue>(const nodeValue &value) { return value.name; }

nodeValue::nodeValue(QVariant var) : value(var.toDouble()) {}
