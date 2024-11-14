// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QAbstractListModel>
#include <variant>

template <typename T> struct GraphNodeContext
{
    using type = GraphNodeContext<void>;
};

// Append the roles for the type onto the QHash
template <typename T> QHash<int, QByteArray> &nodeRoleNames(QHash<int, QByteArray> &base);
// The name of the type (for delegate dispatch)
template <typename T> std::string nodeTypeName(const T &value);
// The path to the icon for the node
template <typename T> std::string nodeTypeIcon(const T &value);
// Delete the node
template <typename T> bool nodeDelete(T &value, typename GraphNodeContext<T>::type &context);
// The title of the node
template <typename T> std::string nodeName(const T &value);
// Change the title of the node
template <typename T> void setNodeName(T &value, const std::string);
// The value of the node
template <typename T> QVariant nodeGetValue(const T value);
// Link an indexed position on the source to an indexed position on the destination
template <typename T> bool nodeConnect(T &source, int sourceIndex, T &destination, int destinationIndex);
// Confirm that a connection is possible (e.g. types match and index isn't already connected)
template <typename T> bool nodeConnectable(const T &source, int sourceIndex, const T &destination, int destinationIndex);
// Unlink an indexed position on the source to an indexed position on the destination
template <typename T> bool nodeDisconnect(T &source, int sourceIndex, T &destination, int destinationIndex);

// A wrapper with supplemental information for a node
template <typename T> class NodeWrapper
{
    public:
    NodeWrapper(QVariant value) : value_(value) {}
    NodeWrapper(T value) : value_(value) {}
    int posx, posy;
    QVariant value() const { return nodeGetValue<T>(value_); }
    T &rawValue() { return value_; }
    const T &rawValue() const { return value_; }

    private:
    T value_;
};

// Get a specific piece of information from a node by index
template <typename T> QVariant nodeData(const T &, int role);

// Set a specific piece of information from a node by index
template <typename T> bool nodeSetData(T &, const QVariant &value, int role);
