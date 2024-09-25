// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QAbstractListModel>
#include <variant>

typedef std::variant<double, double *> nodeValue;

double getValue(nodeValue value);

class NodeWrapper
{
    public:
    NodeWrapper(QVariant value);
    std::string name;
    int posx, posy;
    QVariant value();
    nodeValue &rawValue();

    private:
    nodeValue value_;
};

template <typename T> std::string nodeTypeName(T &value);
template <typename T> std::string nodeTypeIcon(T &value);
