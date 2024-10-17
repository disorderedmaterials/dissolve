// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QAbstractListModel>
#include <variant>

template <typename T> QHash<int, QByteArray> &nodeRoleNames(QHash<int, QByteArray> &base, int startingIndex);
template <typename T> std::string nodeTypeName(const T &value);
template <typename T> std::string nodeTypeIcon(const T &value);
template <typename T> std::string nodeName(const T &value);
template <typename T> void setNodeName(T &value, const std::string);
template <typename T> QVariant nodeGetValue(const T value);
template <typename T> bool nodeConnect(T &source, int sourceIndex, T &destination, int destinationIndex);
template <typename T> bool nodeConnectable(const T &source, int sourceIndex, const T &destination, int destinationIndex);
template <typename T> bool nodeDisconnect(T &source, int sourceIndex, T &destination, int destinationIndex);

template <typename T> class NodeWrapper
{
    public:
    NodeWrapper(QVariant value) : value_(value) {}
    int posx, posy;
    QVariant value() const { return nodeGetValue<T>(value_); }
    T &rawValue() { return value_; }
    const T &rawValue() const { return value_; }

    private:
    T value_;
};

template <typename T> QVariant nodeData(const T&, int role);
template <typename T> bool nodeSetData(T&, const QVariant &value, int role);
