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

    private:
    Molecule *mol_ = nullptr;

    public:
    ConfigurationAtomsModel(Molecule *mol);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    void reset();
};

Q_DECLARE_METATYPE(ConfigurationAtomsModel);

class ConfigurationBondsModel : public QAbstractListModel
{

    enum ConfigurationBondsModelRoles
    {
        X = Qt::UserRole + 0,
        Y = Qt::UserRole + 1,
        Z = Qt::UserRole + 2,
        X2 = Qt::UserRole + 3,
        Y2 = Qt::UserRole + 4,
        Z2 = Qt::UserRole + 5,
    };

    private:
    Molecule *mol_ = nullptr;

    public:
    ConfigurationBondsModel(Molecule *mol);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    void reset();
};

Q_DECLARE_METATYPE(ConfigurationBondsModel);

class ConfigurationMoleculesModel : public QAbstractListModel
{
    Q_OBJECT

    enum ConfigurationMoleculesModelRoles
    {
        Atoms = Qt::UserRole + 0,
        Bonds = Qt::UserRole + 1,
    };

    public:
    Configuration *config = nullptr;
    std::vector<std::shared_ptr<ConfigurationAtomsModel>> atoms_;
    std::vector<std::shared_ptr<ConfigurationBondsModel>> bonds_;

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
    Q_PROPERTY(const ConfigurationMoleculesModel *molecules READ molecules);

    private:
    // Root graph that we pull nodes from
    Graph *root_ = nullptr;
    // Name of configuration we are viewing
    //
    // We use a string instead of an int because the user might add
    // new configuration and change the order
    std::string name_;
    ConfigurationMoleculesModel molecules_;

    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Graph *graph() const;
    void setGraph(Graph *node);

    QString name() const;
    void setName(QString name);

    Q_INVOKABLE void reset();
    const ConfigurationMoleculesModel *molecules() const;
};
