// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/models/nodeGraph/instances/all.h"
#include <QAbstractListModel>
#include <concepts>
#include <variant>

/**
   This file contains a C++20 concept which defines whether a type can
   be shown as a graph.  Any type T which contains implementations for
   all of the functions listed below can be displayed in a graph.  A
   second template parameter Context was also necessary to implement
   deletion as most nodes would need to know *where* they were being
   deleted from.

   A concept was chosen over class methods because:

   1) This allows raw types (e.g. `double`) to be used as nodes.
   Using class methods requires that all data must be wrapped in a
   class.

   2) The GUI constraints aren't needed for the command line version.
   If we used class methods, then the node methods would need to be
   part of the class even in the command line version.  As some of the
   methods require Qt types (e.g. QHash, QVariant), then Qt would
   suddenly become a dependency of the command line code.

   This code is a direct conversion of the template code that came
   before.  Truth be told, the connection functions should likely be
   split out into a separate Concept (allowing us to check for
   connection correctness as compile time), but that is outside of the
   scope of this PR.
 **/

template <typename T, typename Context>
concept Graphable = requires(T a, Context context, const std::string name, T b, int sourceIndex, int destinationIndex,
                             Phantom<T> proxy, QHash<int, QByteArray> &baseHash, int role, QVariant value) {
    {
        nodeDelete(a, context)
    } -> std::same_as<bool>;
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
    {
        nodeData(a, role)
    } -> std::convertible_to<QVariant>;
    {
        nodeSetData(a, value, role)
    } -> std::convertible_to<bool>;
};

// A wrapper with supplemental information for a node
template <typename T, typename Context>
    requires Graphable<T, Context>
class NodeWrapper
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
