// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"
#include "nodes/parameter.h"
#include <QAbstractListModel>
#include <qabstractitemmodel.h>
#include <vector>

// GUI Model for the parameters of a Node
class ParameterModel : public QAbstractListModel
{
    Q_OBJECT;

    public:
    ParameterModel(Node::NodeParameterMap &values);

    private:
    // A reference to the parameters from the model
    Node::NodeParameterMap &values_;
    //
    std::vector<std::pair<std::string, std::string>> renamed_;

    public:
    // Returns the reference to the parameters from the model
    Node::NodeParameterMap &values();
    const Node::NodeParameterMap &values() const;
    //
    void rename(const std::string &currentName, const std::string &newName);

    public:
    // Reset parameter model
    Q_INVOKABLE void resetParameters();
    //
    Q_INVOKABLE QString renamedFrom(QString newName);
    //
    Q_INVOKABLE bool renameInProgress(QString newName);
    //
    Q_INVOKABLE void renameComplete(QString newName);

    /*
     * QAbstractListModel overrides
     */
    public:
    // Return number of parameters (required by QAbstractListModel)
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    // Access edge by QModelIndex.  The correct role can be found in the roleNames function.
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    // Update parameter info
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    // Return the mapping between role index and QML value name.  This is required by QAbstractListModel
    QHash<int, QByteArray> roleNames() const override;
};
