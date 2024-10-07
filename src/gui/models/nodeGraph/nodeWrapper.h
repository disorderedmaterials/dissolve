// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QAbstractListModel>
#include <variant>

class nodeValue
{
    public:
    nodeValue(QVariant var = {});
    std::string name;
    std::variant<double, double *> value;
};

template <typename T> std::string nodeTypeName(const T &value);
template <typename T> std::string nodeTypeIcon(const T &value);
template <typename T> std::string nodeName(const T &value);
template <typename T> QVariant nodeGetValue(const nodeValue value);

template <typename T> class NodeWrapper
{
    public:
    NodeWrapper(QVariant value) : value_(value) {}
    int posx, posy;
    QVariant value() { return nodeGetValue<T>(value_); }
    T &rawValue() { return value_; }

    private:
    T value_;
};
