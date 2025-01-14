// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/models/nodeGraph/instances/all.h"
#include <QAbstractListModel>
#include <concepts>
#include <variant>

/**
   This file contains a series of templates that need to be overloaded
   to allow a type to be displayed as a node.  This is essentially a
   C++ Concept, but without all the compiler checks and niceties that
   we will get with C++20.

   A concept was chosen over class methods because:

   1) This allows raw types (e.g. `double`) to be used as nodes.
   Using class methods requires that all data must be wrapped in a
   class.

   2) The GUI constraints aren't needed for the command line version.
   If we used class methods, then the node methods would need to be
   part of the class even in the command line version.  As some of the
   methods require Qt types (e.g. QHash, QVariant), then Qt would
   suddenly become a dependency of the command line code.
 **/

// Delete the node
template <typename T> bool nodeDelete(T &value, typename GraphNodeContext<T>::type &context);

template <typename T>
concept Graphable = requires(T a, const std::string name, T b, int sourceIndex, int destinationIndex, Proxy<T> proxy,
                             QHash<int, QByteArray> &baseHash) {
    {
        nodeRoleNames(proxy, baseHash)
    } -> std::same_as<QHash<int, QByteArray> &>;
    {
        nodeName(a)
    } -> std::same_as<std::string>;
    {
        nodeTypeName(a)
    } -> std::same_as<std::string>;
    {
        nodeTypeIcon(a)
    } -> std::same_as<std::string>;
    {
        setNodeName(a, name)
    };
    {
        nodeGetValue(a)
    } -> std::same_as<QVariant>;
    {
        nodeConnect(a, sourceIndex, b, destinationIndex)
    } -> std::same_as<bool>;
    {
        nodeConnectable(a, sourceIndex, b, destinationIndex)
    } -> std::same_as<bool>;
    {
        nodeDisconnect(a, sourceIndex, b, destinationIndex)
    } -> std::same_as<bool>;
};

// A wrapper with supplemental information for a node
template <Graphable T> class NodeWrapper
{
    public:
    NodeWrapper(QVariant value) : value_(value) {}
    NodeWrapper(T value) : value_(value) {}
    // The X and Y coordinates of the node on the screen.
    int posx, posy;

    // Get the value wrapped in a QVariant (needed for QML)
    QVariant value() const { return nodeGetValue<T>(value_); }
    // Get the actual value of the node
    T &rawValue() { return value_; }
    const T &rawValue() const { return value_; }

    private:
    // The actual value of the node
    T value_;
};

// Get a specific piece of information from a node by index
template <typename T> QVariant nodeData(const T &, int role);

// Set a specific piece of information from a node by index
template <typename T> bool nodeSetData(T &, const QVariant &value, int role);
