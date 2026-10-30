// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/legacy/configurationModel.h"
#include "nodes/structure/configuration.h"
#include <iostream>
#include <qvariant.h>

/*
 * QAbstractItemModel overrides
 */

int ConfigurationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (root_ == nullptr)
        return 0;
    return root_->findType<ConfigurationNode>().size();
}

QVariant ConfigurationModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (!index.isValid())
        return {};

    auto nodes = root_->findType<ConfigurationNode>();
    if (index.row() >= nodes.size())
        return {};
    return QString::fromStdString(std::string(nodes[index.row()]->name()));
}

Graph *ConfigurationModel::graph() const { return root_; }

void ConfigurationModel::setGraph(Graph *node)
{
    beginResetModel();
    root_ = node;
    endResetModel();
}

QString ConfigurationModel::name() const { return QString::fromStdString(name_); }

void ConfigurationModel::setName(QString name)
{
    name_ = name.toStdString();
    auto nodes = root_->findType<ConfigurationNode>();
    auto it = std::find_if(nodes.begin(), nodes.end(), [this](const ConfigurationNode *x) { return x->name() == name_; });
    if (it != nodes.end())
    {
        atoms_.config = &(*it)->configuration();
        atoms_.reset();
    }
}

void ConfigurationModel::reset()
{
    beginResetModel();
    endResetModel();
}

const ConfigurationAtomsModel *ConfigurationModel::atoms() const { return &atoms_; }

int ConfigurationAtomsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    std::cout << "Tried to count atoms." << std::endl;
    if (config == nullptr)
        return 0;
    return config->nAtoms();
}

QVariant ConfigurationAtomsModel::data(const QModelIndex &index, int role) const
{
    auto &atom = config->atom(index.row());

    switch (role)
    {
        case ConfigurationAtomsModel::ConfigurationAtomsModelRoles::Element:
            return QVariant::fromValue(atom.Z());
        case ConfigurationAtomsModel::ConfigurationAtomsModelRoles::X:
            return QVariant::fromValue(atom.r().x);
        case ConfigurationAtomsModel::ConfigurationAtomsModelRoles::Y:
            return QVariant::fromValue(atom.r().y);
        case ConfigurationAtomsModel::ConfigurationAtomsModelRoles::Z:
            return QVariant::fromValue(atom.r().z);
    }
    return {};
}

void ConfigurationAtomsModel::reset()
{
    beginResetModel();
    endResetModel();
}

QHash<int, QByteArray> ConfigurationAtomsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ConfigurationAtomsModelRoles::X] = "rx";
    roles[ConfigurationAtomsModelRoles::Y] = "ry";
    roles[ConfigurationAtomsModelRoles::Z] = "rz";
    roles[ConfigurationAtomsModelRoles::Element] = "element";
    return roles;
}
