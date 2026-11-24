// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "nodes/graph.h"
#include "nodes/structure/configuration.h"
#include <QAbstractTableModel>
#include <QModelIndex>

// List the atoms in a molecule
class MoleculeAtomsModel : public QAbstractListModel
{

    enum ConfigurationAtomsModelRoles
    {
        R = Qt::UserRole + 0,
        Element = Qt::UserRole + 1,
    };

    private:
    Molecule *mol_ = nullptr;

    public:
    MoleculeAtomsModel(Molecule *mol);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    void reset();
};

Q_DECLARE_METATYPE(MoleculeAtomsModel);

// List the bonds in a molecule
class MoleculeBondsModel : public QAbstractListModel
{

    enum ConfigurationBondsModelRoles
    {
        R = Qt::UserRole + 0,
        R2 = Qt::UserRole + 1,
    };

    private:
    Molecule *mol_ = nullptr;

    public:
    MoleculeBondsModel(Molecule *mol);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    void reset();
};

Q_DECLARE_METATYPE(MoleculeBondsModel);

// List the molecules in a configurations, providing models for the atoms and bonds
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
    std::vector<std::shared_ptr<MoleculeAtomsModel>> atoms_;
    std::vector<std::shared_ptr<MoleculeBondsModel>> bonds_;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    void reset();
};

// List the Configuration nodes in a graph, providing access to the
// molecule models for each configuration
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
