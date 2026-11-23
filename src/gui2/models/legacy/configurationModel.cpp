// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/legacy/configurationModel.h"
#include "classes/species.h"
#include "data/elements.h"
#include "nodes/structure/configuration.h"
#include <qstring.h>
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
        molecules_.config = &(*it)->configuration();
        molecules_.reset();
    }
}

void ConfigurationModel::reset()
{
    beginResetModel();
    molecules_.reset();
    endResetModel();
}

const ConfigurationMoleculesModel *ConfigurationModel::molecules() const { return &molecules_; }

int ConfigurationMoleculesModel::rowCount(const QModelIndex &parent) const { return atoms_.size(); }

QHash<int, QByteArray> ConfigurationMoleculesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ConfigurationMoleculesModelRoles::Atoms] = "atoms";
    roles[ConfigurationMoleculesModelRoles::Bonds] = "bonds";
    return roles;
}

QVariant ConfigurationMoleculesModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
        case ConfigurationMoleculesModel::ConfigurationMoleculesModelRoles::Atoms:
            return QVariant::fromValue(atoms_[index.row()].get());
        case ConfigurationMoleculesModel::ConfigurationMoleculesModelRoles::Bonds:
            return QVariant::fromValue(bonds_[index.row()].get());
    }
    return {};
}

void ConfigurationMoleculesModel::reset()
{
    beginResetModel();
    atoms_.clear();
    bonds_.clear();
    if (config)
        for (auto &mol : config->molecules())
        {
            atoms_.emplace_back(std::make_shared<ConfigurationAtomsModel>(mol.get()));
            bonds_.emplace_back(std::make_shared<ConfigurationBondsModel>(mol.get()));
        }
    endResetModel();
}

ConfigurationAtomsModel::ConfigurationAtomsModel(Molecule *mol) : mol_(mol) {}

int ConfigurationAtomsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (mol_ == nullptr)
        return 0;
    return mol_->nAtoms();
}

QVariant ConfigurationAtomsModel::data(const QModelIndex &index, int role) const
{
    auto *atom = mol_->atom(index.row());

    switch (role)
    {
        case ConfigurationAtomsModel::ConfigurationAtomsModelRoles::Element:
            return QString::fromUtf8(Elements::symbol(atom->Z()).data());
        case ConfigurationAtomsModel::ConfigurationAtomsModelRoles::X:
            return QVariant::fromValue(atom->r().x);
        case ConfigurationAtomsModel::ConfigurationAtomsModelRoles::Y:
            return QVariant::fromValue(atom->r().y);
        case ConfigurationAtomsModel::ConfigurationAtomsModelRoles::Z:
            return QVariant::fromValue(atom->r().z);
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

ConfigurationBondsModel::ConfigurationBondsModel(Molecule *mol) : mol_(mol) {}

int ConfigurationBondsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (mol_ == nullptr)
        return 0;
    return mol_->species()->bonds().size();
}

QVariant ConfigurationBondsModel::data(const QModelIndex &index, int role) const
{
    auto bond = mol_->species()->bonds()[index.row()];
    auto base = mol_->atom(bond.i()->index())->r();
    auto delta = mol_->atom(bond.i()->index())->r() - mol_->atom(bond.j()->index())->r();

    switch (role)
    {
        case ConfigurationBondsModel::ConfigurationBondsModelRoles::X:
            return QVariant::fromValue(base.x);
        case ConfigurationBondsModel::ConfigurationBondsModelRoles::Y:
            return QVariant::fromValue(base.y);
        case ConfigurationBondsModel::ConfigurationBondsModelRoles::Z:
            return QVariant::fromValue(base.z);
        case ConfigurationBondsModel::ConfigurationBondsModelRoles::THETA:
            return 90.0;
        case ConfigurationBondsModel::ConfigurationBondsModelRoles::PHI:
            return 0.0;
    }
    return {};
}

void ConfigurationBondsModel::reset()
{
    beginResetModel();
    endResetModel();
}

QHash<int, QByteArray> ConfigurationBondsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ConfigurationBondsModelRoles::X] = "rx";
    roles[ConfigurationBondsModelRoles::Y] = "ry";
    roles[ConfigurationBondsModelRoles::Z] = "rz";
    roles[ConfigurationBondsModelRoles::THETA] = "theta";
    roles[ConfigurationBondsModelRoles::PHI] = "phi";
    return roles;
}
