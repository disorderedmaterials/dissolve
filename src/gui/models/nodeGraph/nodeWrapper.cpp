// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "nodeWrapper.h"

NodeWrapper::NodeWrapper(nodeValue value) : value_(value) {}
QVariant NodeWrapper::value() { return getValue(value_); }
nodeValue &NodeWrapper::rawValue() { return value_; }

// helper type for the visitor #4
template <class... Ts> struct overloaded : Ts...
{
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

double getValue(nodeValue value)
{
    return std::visit(overloaded{[](double arg) { return arg; }, [](double *arg) { return *arg; }}, value);
}
