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
    return std::visit(overloaded{[](double arg) { return QVariant::fromValue(arg); },
                                 [](nodeValue *arg)
                                 {
                                     if (arg)
                                         return nodeGetValue<nodeValue>(*arg);
                                     QVariant empty;
                                     return empty;
                                 }},
                      value.value);
}

template <> std::string nodeTypeName<nodeValue>(const nodeValue &value)
{
    return std::visit(overloaded{[](double arg) { return "number"; }, [](nodeValue *arg) { return "ptr"; }}, value.value);
}

template <> std::string nodeTypeIcon<nodeValue>(const nodeValue &value)
{
    return std::visit(overloaded{[](double arg) { return "file:/home/adam/Code/dissolve/src/gui/icons/open.svg"; },
                                 [](nodeValue *arg) { return "file:/home/adam/Code/dissolve/src/gui/icons/open.svg"; }},
                      value.value);
}

template <> std::string nodeName<nodeValue>(const nodeValue &value) { return value.name; }

template <> void setNodeName<nodeValue>(nodeValue &value, const std::string name) { value.name = name; }

nodeValue::nodeValue(QVariant var)
{
    if (var.isNull())
        value = nullptr;
    else
        value = var.toDouble();
}
