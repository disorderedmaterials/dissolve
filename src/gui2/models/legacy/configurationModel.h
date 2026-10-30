// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "nodes/graph.h"
#include "nodes/structure/configuration.h"
#include <QAbstractTableModel>
#include <QModelIndex>

class ConfigurationAtomsModel : public QAbstractListModel
{

    enum ConfigurationAtomsModelRoles
    {
        X = Qt::UserRole + 0,
        Y = Qt::UserRole + 1,
        Z = Qt::UserRole + 2,
        Element = Qt::UserRole + 3,
    };

    public:
    Configuration *config = nullptr;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    void reset();
};

class ConfigurationModel : public QAbstractListModel
{
    Q_OBJECT
    // The main graph
    Q_PROPERTY(Graph *graph READ graph WRITE setGraph);
    Q_PROPERTY(QString name READ name WRITE setName);
    Q_PROPERTY(const ConfigurationAtomsModel *atoms READ atoms);

    private:
    // Root graph that we pull nodes from
    Graph *root_ = nullptr;
    // Name of configuration we are viewing
    //
    // We use a string instead of an int because the user might add
    // new configuration and change the order
    std::string name_;
    ConfigurationAtomsModel atoms_;

    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Graph *graph() const;
    void setGraph(Graph *node);

    QString name() const;
    void setName(QString name);

    Q_INVOKABLE void reset();
    const ConfigurationAtomsModel *atoms() const;
};
