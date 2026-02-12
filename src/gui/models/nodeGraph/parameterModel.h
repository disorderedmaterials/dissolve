// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

// #include "gui/models/nodeGraph/parameterModel.h"
#include "nodes/node.h"
#include "nodes/parameter.h"
#include <QAbstractListModel>
#include <qabstractitemmodel.h>

// GUI Model for the parameters of a Node
class ParameterModel : public QAbstractListModel
{
    public:
    ParameterModel(Node::NodeParameterMap &values);

    // Return number of parameters (required by QAbstractListModel)
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    // Access edge by QModelIndex.  The correct role can be found in the roleNames function.
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Update parameter info
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    // Return the mapping between role index and QML value name.  This is required by QAbstractListModel
    QHash<int, QByteArray> roleNames() const override;

    private:
    // A reference to the parameters from the model
    Node::NodeParameterMap &values_;
};
